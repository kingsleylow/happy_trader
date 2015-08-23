/*
 *  Copyright (c) 2000-2003 Barak Weichselbaum <barak@komodia.com>
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Contact info:
 * -------------
 *
 * Site:					http://www.komodia.com
 * Main contact:			barak@komodia.com
 * For custom projects, 
 * consulting, or other
 * paid services:			sales@komodia.com
 */

#include "stdafx.h"
#include "Win32ReadWriteLock.h"

#include "OSManager.h"
#include "GenericMutex.h"
#include "GenericSemaphore.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

CWin32ReadWriteLock::CWin32ReadWriteLock(int iMaximumReaders) : CGenericReadWriteLock(iMaximumReaders),
																m_pSemaphore(NULL),
																m_pMutex(NULL),
																m_bWriter(FALSE),
																m_bEscalation(FALSE),
																m_iReaders(0)
{
	//Allocate the mutex and the semaphores
	m_pMutex=COSManager::CreateMutex();
	m_pSemaphore=COSManager::CreateSemaphore(iMaximumReaders,iMaximumReaders);
}

CWin32ReadWriteLock::~CWin32ReadWriteLock()
{
	//Delete the mutexs and semaphores
	delete m_pMutex;
	delete m_pSemaphore;
}

BOOL CWin32ReadWriteLock::LockRead(unsigned long ulMSTimeout)
{
	//First lock the mutex and check the read flag
	CMutexAutoRelease aRelease(m_pMutex,TRUE);

	//Check the flag
	if (m_bEscalation || m_bWriter)
		//Can't lock
		return FALSE;

	//Check if we will be locked ?
	if (m_iReaders==GetMaximumReaders())
	{
		//Release the lock and try to get it
		aRelease.Release();

		//Lock the semaphore
		if (m_pSemaphore->Aquire(ulMSTimeout))
			return FALSE;
		else
		{
			//Relock it
			aRelease.Aquire();

			//Check if there are writers ?
			if (m_bEscalation || m_bWriter)
			{
				//So soon ?
				//Release what we got
				m_pSemaphore->Release();

				//Exit
				return FALSE;
			}
			else
				//Increase the count
				++m_iReaders;
		}
	}
	else if (m_pSemaphore->Aquire(ulMSTimeout))
		return FALSE;
	else
		//Increase the semaphore and our count
		++m_iReaders;

	//Done
	return TRUE;
}

void CWin32ReadWriteLock::UnlockRead()
{
	//First lock the mutex and check the read flag
	CMutexAutoRelease aRelease(m_pMutex,TRUE);

	//Do we have readers
	if (m_iReaders)
	{
		//Release the semaphore
		m_pSemaphore->Release();

		//Decrease the count
		--m_iReaders;
	}
}

BOOL CWin32ReadWriteLock::LockWrite()
{
	//First lock the mutex and check the read flag
	CMutexAutoRelease aRelease(m_pMutex,TRUE);

	//Check the flag
	if (m_iReaders || m_bWriter)
		//Can't lock
		return FALSE;

	//Indicate we are writing
	m_bWriter=TRUE;

	//Done
	return TRUE;
}

void CWin32ReadWriteLock::UnlockWrite()
{
	//First lock the mutex and check the read flag
	CMutexAutoRelease aRelease(m_pMutex,TRUE);

	//Do we have readers
	if (m_bWriter)
	{
		//Not writing
		m_bWriter=FALSE;

		//Remove escalation flag
		m_bEscalation=FALSE;
	}
}

BOOL CWin32ReadWriteLock::Escalate(unsigned long ulMSTimeout,
								   unsigned long ulIterations)
{
	//Try to lock for write
	if (LockWrite())
		return TRUE;

	//Try to escalate 
	CMutexAutoRelease aRelease(m_pMutex,TRUE);

	//Change the escalation status
	m_bEscalation=TRUE;

	//Exit from the mutex
	aRelease.Release();

	//And wait to see if we can continue
	for (unsigned long ulCounter=0;ulCounter<ulIterations;++ulCounter)
	{
		//Can we write
		if (LockWrite())
			return TRUE;

		//Wait
		Sleep(ulMSTimeout);
	}

	//Failed to escalate
	return FALSE;
}

void CWin32ReadWriteLock::StopEscalation()
{
	//First lock the mutex and check the read flag
	CMutexAutoRelease aRelease(m_pMutex,TRUE);

	//Remove escalation flag
	m_bEscalation=FALSE;
}

KOMODIA_NAMESPACE_END
