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

#if !defined(AFX_OSMANAGER_H__0D62630C_E1FF_4CB1_900B_2B6FFF66F33C__INCLUDED_)
#define AFX_OSMANAGER_H__0D62630C_E1FF_4CB1_900B_2B6FFF66F33C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericThread.h"

KOMODIA_NAMESPACE_START

class CGenericCriticalSection;
class CGenericSemaphore;
class CGenericMutex;
class CGenericEvent;
class CGenericReadWriteLock;

class COSManager  
{
public:
	//Create an event according to the OS
	static CGenericEvent* CreateEvent(BOOL bInitialState=FALSE,
									  BOOL bManualReset=FALSE,
									  const char* pEventName=NULL);

	//Create a mutex according to the OS
	static CGenericMutex* CreateMutex(const char* pMutexName=NULL);

	//Create a semaphore according to the OS
	static CGenericSemaphore* CreateSemaphore(long lInitialCount,
											  long lMaxCount,
											  const char* pSemaphoreName=NULL);

	//Create a thread according to the OS
	static CGenericThread* CreateThread(CGenericThread::LPGenericThreadProc pThreadProc);

	//Create a critical section according to the OS
	static CGenericCriticalSection* CreateCriticalSection();

	//Create a read write lock according to the OS
	static CGenericReadWriteLock* CreateReadWriteLock(int iMaximumReaders);
private:
	//Private Ctors and Dtors
	COSManager();
	~COSManager();

	//No copy Ctor
	COSManager(const COSManager& rManager);

	//No copy operator
	COSManager& operator=(const COSManager& rManager);
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_OSMANAGER_H__0D62630C_E1FF_4CB1_900B_2B6FFF66F33C__INCLUDED_)
