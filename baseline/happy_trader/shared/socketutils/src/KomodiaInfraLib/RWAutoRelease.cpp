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
#include "RWAutoRelease.h"

#include "GenericReadWriteLock.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

CRWAutoRelease::CRWAutoRelease(CGenericReadWriteLock* pLock,
							   BOOL bRead,
							   BOOL bLock) : m_pLock(pLock),
											 m_bLockedRead(FALSE),
											 m_bLockedWrite(FALSE),
											 m_bEscalated(FALSE)
{
	//Do we need to lock 
	if (bLock)
		if (bRead)
			if (LockRead())
				m_bLockedRead=TRUE;
			else
				;
		else 
			if (LockWrite())
				m_bLockedWrite=TRUE;
}

CRWAutoRelease::~CRWAutoRelease()
{
	//Are we locked some how
	if (m_bLockedRead)
		UnlockRead();
	else if (m_bLockedWrite)
		UnlockWrite();

	//Are we escalated
	if (m_bEscalated)
		StopEscalation();
}

BOOL CRWAutoRelease::LockRead(unsigned long ulMSTimeout)
{
	if (m_pLock)
		if (m_pLock->LockRead(ulMSTimeout))
		{
			//We are locked for read
			m_bLockedRead=TRUE;

			//Exit
			return FALSE;
		}
		else
			return FALSE;
	else
		return FALSE;
}

void CRWAutoRelease::UnlockRead()
{
	if (m_pLock && m_bLockedRead)
	{
		//Unlock us
		m_pLock->UnlockRead();

		//Not locked anymore
		m_bLockedRead=FALSE;
	}
}

BOOL CRWAutoRelease::LockWrite()
{
	if (m_pLock)
		if (m_pLock->LockWrite())
		{
			//We are locked for write
			m_bLockedWrite=TRUE;

			//Exit
			return TRUE;
		}
		else
			return FALSE;
	else
		return FALSE;
}

void CRWAutoRelease::UnlockWrite()
{
	//Are we locked
	if (m_pLock && m_bLockedWrite)
	{
		//Unlock us
		m_pLock->UnlockWrite();

		//Set the flag
		m_bLockedWrite=FALSE;

		//Not escalated
		m_bEscalated=FALSE;
	}
}

BOOL CRWAutoRelease::Escalate(unsigned long ulMSTimeout,
							  unsigned long ulIterations)
{
	if (m_pLock)
		if (m_pLock->Escalate(ulMSTimeout,ulIterations))
		{
			//We are locked for write
			m_bLockedWrite=TRUE;

			//And for escalation
			m_bEscalated=TRUE;

			//Exit
			return TRUE;
		}
		else
			return FALSE;
	else
		return FALSE;
}

void CRWAutoRelease::StopEscalation()
{
	if (m_pLock && m_bEscalated)
	{
		//Stop it
		m_pLock->StopEscalation();

		//Set the flag
		m_bEscalated=FALSE;
	}
}

KOMODIA_NAMESPACE_END
