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
#include "Win32Thread.h"

#include "ErrorHandlerMacros.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CWin32Thread_Class "CWin32Thread"

CWin32Thread::CWin32Thread(LPGenericThreadProc pThreadProc) : CGenericThread(pThreadProc),
															  m_hThread(0)
{
	try
	{
		//Set our name
		SetName(CWin32Thread_Class);

		//Create the thread
		if (GetThreadProc())
		{
			//Our thread ID
			DWORD dwThreadID;

			//Create the thread in suspend mode
			m_hThread=CreateThread(NULL,
   								   0,
								   Win32Thread,
								   this,
								   CREATE_SUSPENDED,
								   &dwThreadID);

			//Check if created
			if (m_hThread)
			{
				//Set the thread ID
				SetThreadID(dwThreadID);

				//Set the status
				SetThreadStatus(tsSuspended);
			}
			else
				//An error
				throw std::string("Failed to create thread!");
		}
		else
			//Throw the error
			throw std::string("No thread proc!");
	}
	ERROR_HANDLER_RETHROW("CWin32Thread")
}

CWin32Thread::~CWin32Thread()
{
	try
	{
		//Stop the thread
		Stop();
	}
	ERROR_HANDLER("~CWin32Thread")
}

BOOL CWin32Thread::Start(LPVOID pData)
{
	try
	{
		if (GetThreadStatus()==tsStopped)
		{
			//Recreate the thread
			//Our thread ID
			DWORD dwThreadID;

			//Create the thread in suspend mode
			m_hThread=CreateThread(NULL,
   								   0,
								   Win32Thread,
								   this,
								   CREATE_SUSPENDED,
								   &dwThreadID);

			//Check if created
			if (m_hThread)
			{
				//Set the thread ID
				SetThreadID(dwThreadID);

				//Set the status
				SetThreadStatus(tsSuspended);
			}
			else
				//Can't run
				return FALSE;
		}
		else if (GetThreadStatus()!=tsSuspended)
			return FALSE;

		//Start the thread
		CGenericThread::Start(pData);

		//Resume the thread
		if (m_hThread)
			if (ResumeThread(m_hThread)!=-1)
				//We are running
				return TRUE;

		return FALSE;
	}
	ERROR_HANDLER_RETURN("Start",FALSE)
}

BOOL CWin32Thread::Stop()
{
	try
	{
		//Only if suspened or running
		//Do we have the thread ?
		if (m_hThread)
		{
			//What status are we
			if (GetThreadStatus()==tsRunning &&
				GetBruteTermination())
				//First try to close it
				if (!TerminateThread(m_hThread,THREAD_DO_NOTHING_EXIT_VALUE))
					return FALSE;

			if (GetThreadStatus()==tsSuspended ||
				GetThreadStatus()==tsRunning)
				if (CloseHandle(m_hThread))
				{
					//Close the handle
					m_hThread=NULL;

					//Stopped
					SetThreadStatus(tsStopped);

					//Exit
					return TRUE;
				}
				else
					return FALSE;
			else
			{
				//Just close the handle
				if (!CloseHandle(m_hThread))
					m_hThread=NULL;

				//Exit
				return FALSE;
			}
		}
		else
			return FALSE;
	}
	ERROR_HANDLER_RETURN("Stop",FALSE)
}

DWORD WINAPI CWin32Thread::Win32Thread(LPVOID lpData)
{
	try
	{
		//We have ourselves
		CWin32Thread* pClass;
		pClass=(CWin32Thread*)lpData;
		
		//Indicate we started running
		pClass->SetThreadStatus(tsRunning);

		//Get our data
		LPVOID pData;
		pData=pClass->GetData();

		//Our threadproc
		LPGenericThreadProc lpProc;
		lpProc=pClass->GetThreadProc();

		//Do we have autodelete (must be set before running thread)
		BOOL bAutoDelete;
		bAutoDelete=pClass->GetAutoDelete();

		//Get the proc return value
		DWORD dwReturnValue;

		//Protect it
		try
		{
			dwReturnValue=(*lpProc)(pData);
		}
		catch(...)
		{
		}

		//If we have auto delete then just delete it
		if (bAutoDelete)
		{
			//First close the socket
			//We don't need to be like this
			pClass->SetBruteTermination(FALSE);

			//Now we can delete ourselves
			delete pClass;
		}
		//Check if we need to set the status
		else if (dwReturnValue!=THREAD_DO_NOTHING_EXIT_VALUE)
			//Indicate we stopped running
			pClass->SetThreadStatus(tsStopped);

		return dwReturnValue;
	}
	ERROR_HANDLER_STATIC_RETURN(CWin32Thread_Class,"Win32Thread",THREAD_DO_NOTHING_EXIT_VALUE)
}

BOOL CWin32Thread::SetPriority(CGenericThread::ThreadPriority aPriority)
{
	try
	{
		static const int iThreadPriority[]={THREAD_PRIORITY_IDLE,
											THREAD_PRIORITY_LOWEST,
											THREAD_PRIORITY_BELOW_NORMAL,
											THREAD_PRIORITY_NORMAL,
											THREAD_PRIORITY_ABOVE_NORMAL,
											THREAD_PRIORITY_HIGHEST,
											THREAD_PRIORITY_TIME_CRITICAL};

		//Do we have a thread
		if (GetThreadStatus()==tsStopped)
		{
			//Recreate the thread
			//Our thread ID
			DWORD dwThreadID;

			//Create the thread in suspend mode
			m_hThread=CreateThread(NULL,
   								   0,
								   Win32Thread,
								   this,
								   CREATE_SUSPENDED,
								   &dwThreadID);

			//Check if created
			if (m_hThread)
			{
				//Set the thread ID
				SetThreadID(dwThreadID);

				//Set the status
				SetThreadStatus(tsSuspended);
			}
			else
				//Can't run
				return FALSE;
		}

		//Now we can set the priority
		return SetThreadPriority(m_hThread,
								 iThreadPriority[aPriority]);
	}
	ERROR_HANDLER_RETURN("SetPriority",FALSE)
}

CGenericThread::ThreadPriority CWin32Thread::GetPriority()const
{
	try
	{
		//Check if we have the thread
		if (!m_hThread)
			return CGenericThread::tpNormal;

		int iPriority;

		//Get the priority
		iPriority=GetThreadPriority(m_hThread);

		//And return the value
		switch (iPriority)
		{
		case THREAD_PRIORITY_ABOVE_NORMAL:
			return CGenericThread::tpAboveNormal;
		case THREAD_PRIORITY_BELOW_NORMAL:
			return CGenericThread::tpBelowNormal;
		case THREAD_PRIORITY_HIGHEST:
			return CGenericThread::tpHighest;
		case THREAD_PRIORITY_IDLE:
			return CGenericThread::tpIdle;
		case THREAD_PRIORITY_LOWEST:
			return CGenericThread::tpLowest;
		case THREAD_PRIORITY_NORMAL:
			return CGenericThread::tpNormal;
		case THREAD_PRIORITY_TIME_CRITICAL:
			return CGenericThread::tpTimeCritical;
		default:
			return CGenericThread::tpNormal;
		}
	}
	ERROR_HANDLER_RETURN("GetPriority",CGenericThread::tpNormal)
}

BOOL CWin32Thread::IsInThread()const
{
	return GetCurrentThreadId()==GetThreadID();
}

KOMODIA_NAMESPACE_END
