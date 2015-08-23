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

#if !defined(AFX_GENERICTHREAD_H__6DE35F78_E78B_4B37_BF46_A75D1FC3A1F8__INCLUDED_)
#define AFX_GENERICTHREAD_H__6DE35F78_E78B_4B37_BF46_A75D1FC3A1F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ErrorHandler.h"
#include "GenericCriticalSection.h"
#include "KomodiaSettings.h"

KOMODIA_NAMESPACE_START

#define THREAD_DO_NOTHING_EXIT_VALUE 0xffffffff

class CGenericThread : public CErrorHandler
{
public:
	//Our thread proc
	typedef DWORD (*GenericThreadProc)(LPVOID pParam);
	typedef GenericThreadProc LPGenericThreadProc;

	//Thread status
	typedef enum _ThreadStatus
	{
		tsSuspended,
		tsRunning,
		tsStopped,
		tsError
	} ThreadStatus;

	//Thread priority
	typedef enum _ThreadPriority
	{
		tpIdle=0,
		tpLowest,
		tpBelowNormal,
		tpNormal,
		tpAboveNormal,
		tpHighest,
		tpTimeCritical
	} ThreadPriority;
public:
	//Are we in the thread
	virtual BOOL IsInThread()const=0;

	//Set brute termination status
	//Affects when can't stop thread, will be brutely terminated
	//Default is TRUE
	void SetBruteTermination(BOOL bBrute);

	//Set autodelete status
	//Affects when thread stops, it will be automaticall deleted
	//Default is FALSE
	void SetAutoDelete(BOOL bAuto);

	//Set the thread priority 
	//(must be set before Start)
	virtual BOOL SetPriority(ThreadPriority aPriority)=0;

	//Get the thread priority
	virtual ThreadPriority GetPriority()const=0;

	//Start the thread
	//pData - Custome user data to pass to the thread
	virtual BOOL Start(LPVOID pData);

	//Terminate the thread
	virtual BOOL Stop()=0;

	//Get the thread ID
	virtual DWORD GetThreadID()const;
	
	//Get our thread status
	ThreadStatus GetThreadStatus()const;

	//Ctor and Dtor
	//pThreadProc - The thread proc to use
	CGenericThread(LPGenericThreadProc pThreadProc);
	virtual ~CGenericThread();
protected:
	//Get brute termination
	BOOL GetBruteTermination()const;

	//Should we auto delete
	BOOL GetAutoDelete()const;

	//Get the thread proc
	LPGenericThreadProc GetThreadProc()const;
	
	//Set our thread status
	void SetThreadStatus(ThreadStatus aStatus);

	//Get the saved data
	virtual LPVOID GetData()const;

	//Set the thread ID
	void SetThreadID(DWORD dwThreadID);
private:
	//No copy ctor
	CGenericThread(const CGenericThread& rThread);

	//No assignment operator
	CGenericThread& operator=(const CGenericThread& rThread);

	//Our thread status
	ThreadStatus m_aThreadStatus;

	//Our critical section
	CGenericCriticalSection* m_pCSection;

	//Auto delete ?
	BOOL m_bAutoDelete;
	
	//Do we allow brute termination ?
	BOOL m_bBruteTermination;

	//Our thread proc
	LPGenericThreadProc m_pThreadProc;

	//The thread ID
	DWORD m_dwThreadID;

	//Our data
	LPVOID m_pData;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_GENERICTHREAD_H__6DE35F78_E78B_4B37_BF46_A75D1FC3A1F8__INCLUDED_)
