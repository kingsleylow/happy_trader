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
#include "Win32Semaphore.h"

#include <string>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

CWin32Semaphore::CWin32Semaphore(long lInitialCount,
								 long lMaxCount,
								 const char* pSemaphoreName) : CGenericSemaphore(lInitialCount,
																				 lMaxCount,
																				 pSemaphoreName),
															   m_hSemaphore(0),
															   m_bDontDelete(FALSE)
{
	//Create the semaphore
	m_hSemaphore=CreateSemaphore(NULL,
								 lInitialCount,
								 lMaxCount,
								 pSemaphoreName);

	//Did we create the semaphore?
	if (!m_hSemaphore)
		throw std::string("Failed to create semaphore!");

	//Check if we are the owners
	if (pSemaphoreName && 
		GetLastError()!=ERROR_ALREADY_EXISTS)
		SetFirst();
}

CWin32Semaphore::~CWin32Semaphore()
{
	if (m_hSemaphore)
		//Close the semaphore
		CloseHandle(m_hSemaphore);
}

BOOL CWin32Semaphore::Aquire(unsigned long ulMSTimeout)
{
	DWORD dwWaitResult;

	//Wait for the semaphore
	dwWaitResult=WaitForSingleObject(m_hSemaphore,
									 ulMSTimeout);

	//Check why did we quit
	return dwWaitResult==WAIT_TIMEOUT;
}

long CWin32Semaphore::Release(long lAmount)
{
	long lOld;
	BOOL bResult;

	//Release it
	bResult=ReleaseSemaphore(m_hSemaphore,
							 lAmount,
							 &lOld);

	//Check if it's an error
	if (bResult)
		return lOld;
	else
		return -1;
}

HANDLE CWin32Semaphore::GetHandle()const
{
	return m_hSemaphore;
}

void CWin32Semaphore::ReleaseObject()
{
	Release();
}

KOMODIA_NAMESPACE_END
