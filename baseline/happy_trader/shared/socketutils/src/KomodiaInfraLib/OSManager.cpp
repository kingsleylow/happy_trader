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
#include "OSManager.h"

#include "Win32CriticalSection.h"
#include "Win32Thread.h"
#include "Win32Semaphore.h"
#include "Win32Mutex.h"
#include "Win32Event.h"
#include "Win32ReadWriteLock.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

COSManager::COSManager()
{
}

COSManager::~COSManager()
{
}

CGenericEvent* COSManager::CreateEvent(BOOL bInitialState,
									   BOOL bManualReset,
									   const char* pEventName)
{
#ifdef WIN32
	return new CWin32Event(bInitialState,
						   bManualReset,
						   pEventName);
#else
	return NULL;
#endif
}

CGenericCriticalSection* COSManager::CreateCriticalSection()
{
#ifdef WIN32
	return new CWin32CriticalSection;
#else
	return NULL;
#endif
}

CGenericThread* COSManager::CreateThread(CGenericThread::LPGenericThreadProc pThreadProc)
{
#ifdef WIN32
	return new CWin32Thread(pThreadProc);
#else
	return NULL;
#endif
}

CGenericSemaphore* COSManager::CreateSemaphore(long lInitialCount,
											   long lMaxCount,
											   const char* pSemaphoreName)
{
#ifdef WIN32
	return new CWin32Semaphore(lInitialCount,
							   lMaxCount,
							   pSemaphoreName);
#else
	return NULL;
#endif
}

CGenericMutex* COSManager::CreateMutex(const char* pMutexName)
{
#ifdef WIN32
	return new CWin32Mutex(pMutexName);
#else
	return NULL;
#endif
}

CGenericReadWriteLock* COSManager::CreateReadWriteLock(int iMaximumReaders)
{
#ifdef WIN32
	return new CWin32ReadWriteLock(iMaximumReaders);
#else
	return NULL;
#endif
}

KOMODIA_NAMESPACE_END
