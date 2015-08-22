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

#if !defined(AFX_WIN32THREAD_H__7351FE98_D7EB_4340_B658_6A5F4844CA35__INCLUDED_)
#define AFX_WIN32THREAD_H__7351FE98_D7EB_4340_B658_6A5F4844CA35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#include "GenericThread.h"

KOMODIA_NAMESPACE_START

class CWin32Thread : public CGenericThread  
{
public:
	//Are we in the thread
	virtual BOOL IsInThread()const;

	//Set the thread priority 
	//(must be set before Start)
	virtual BOOL SetPriority(CGenericThread::ThreadPriority aPriority);

	//Get the thread priority
	virtual CGenericThread::ThreadPriority GetPriority()const;

	//Start the thread
	//pData - Custome user data to pass to the thread
	virtual BOOL Start(LPVOID pData);

	//Terminate the thread
	virtual BOOL Stop();

	//Ctor and Dtor
	//pThreadProc - The thread proc to use
	CWin32Thread(LPGenericThreadProc pThreadProc);
	virtual ~CWin32Thread();
private:
	//No copy Ctor
	CWin32Thread(const CWin32Thread& rThread);

	//Copy operator
	CWin32Thread& operator=(const CWin32Thread& rThread);

	//Our thread proc
	static DWORD WINAPI Win32Thread(LPVOID lpData);

	//The thread handle
	HANDLE m_hThread;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_WIN32THREAD_H__7351FE98_D7EB_4340_B658_6A5F4844CA35__INCLUDED_)
