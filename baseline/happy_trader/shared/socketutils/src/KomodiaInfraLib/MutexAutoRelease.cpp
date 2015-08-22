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
#include "MutexAutoRelease.h"

#include "GenericMutex.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

CMutexAutoRelease::CMutexAutoRelease(CGenericMutex* pMutex,
									 BOOL bLock,
									 unsigned long ulMSTimeout) : m_pMutex(pMutex),
																  m_ulEntryCount(0)
{
	if (bLock)
		Aquire(ulMSTimeout);
}

CMutexAutoRelease::~CMutexAutoRelease()
{
	//Check if valid
	if (m_pMutex)
		//How many entries
		for (unsigned long ulCounter=0;ulCounter<m_ulEntryCount;++ulCounter)
			m_pMutex->Release();
}

BOOL CMutexAutoRelease::Aquire(unsigned long ulMSTimeout)
{
	if (m_pMutex)
		//Lock it
		if (m_pMutex->Aquire(ulMSTimeout))
			return TRUE;
		else
		{
			//Increase the count
			++m_ulEntryCount;

			//And exit
			return FALSE;
		}
	else
		return FALSE;
}

BOOL CMutexAutoRelease::Release()
{
	if (m_pMutex && m_ulEntryCount)
	{
		//Decrease the count
		--m_ulEntryCount;

		//Try to release
		if (!m_pMutex->Release())
		{
			//Increase the count
			++m_ulEntryCount;

			//Exit
			return FALSE;
		}
		else
			return TRUE;
	}
	else
		return TRUE;
}

KOMODIA_NAMESPACE_END
