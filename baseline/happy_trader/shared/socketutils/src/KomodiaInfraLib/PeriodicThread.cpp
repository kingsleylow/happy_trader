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
#include "PeriodicThread.h"

#include "ErrorHandlerMacros.h"
#include "GenericEvent.h"
#include "OSManager.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CPeriodicThread_Class "CPeriodicThread"

CPeriodicThread::CPeriodicThread(PeriodicThreadProc pProc) : CErrorHandler(),
															 m_pThread(NULL),
															 m_pUserProc(pProc),
															 m_pEnterEvent(NULL),
															 m_pExitEvent(NULL),
															 m_dwInterval(0),
															 m_dwThreadTimeout(20000),
															 m_bDestructor(FALSE),
															 m_bAbort(FALSE),
															 m_pUserProcNoInit(NULL)
{
	try
	{
		//Set our name
		SetName(CPeriodicThread_Class);

		//Allocate the thread
		m_pThread=COSManager::CreateThread(PeriodicThread);
	}
	ERROR_HANDLER("CPeriodicThread")
}

CPeriodicThread::CPeriodicThread(LPPeriodicThreadProcNoInit pProc) : CErrorHandler(),
														  			 m_pThread(NULL),
																	 m_pUserProc(NULL),
																	 m_pEnterEvent(NULL),
																	 m_pExitEvent(NULL),
																	 m_dwInterval(0),
																	 m_dwThreadTimeout(20000),
																	 m_bDestructor(FALSE),
																	 m_bAbort(FALSE),
																	 m_pUserProcNoInit(pProc)
{
	try
	{
		//Set our name
		SetName(CPeriodicThread_Class);

		//Allocate the thread
		m_pThread=COSManager::CreateThread(PeriodicThread);
	}
	ERROR_HANDLER("CPeriodicThread")
}

CPeriodicThread::~CPeriodicThread()
{
	try
	{
		//Indicate tremination
		m_bDestructor=TRUE;

		//Delete our thread
		Stop();
	}
	ERROR_HANDLER("~CPeriodicThread")
}

DWORD CPeriodicThread::PeriodicThread(LPVOID lpData)
{
	try
	{
		//Get our class
		CPeriodicThread* pClass;
		pClass=(CPeriodicThread*)lpData;

		//Do we exit
		BOOL bExit;
		bExit=FALSE;

		if (pClass->m_pUserProc)
		{
			//Run user proc
			try
			{
				if (!(*pClass->m_pUserProc)(tsStart,
											pClass->m_lpData))
					//We are aborting
					bExit=TRUE;
			}
			catch (...)
			{
				bExit=TRUE;
			}

			//Do we exit
			if (bExit)
				pClass->m_bAbort=TRUE;
		}

		//Set we are alive
		if (pClass->m_pEnterEvent)
			pClass->m_pEnterEvent->Set();
		else
			return FALSE;

		//Do we exit
		if (bExit)
			return FALSE;

		//Is it the first run ?
		BOOL bFirstRun;
		bFirstRun=TRUE;

		//Run
		while (1)
		{
			//Do we have a valid interval ?
			if (!pClass->m_dwInterval)
			{
				//Report it
				ReportStaticError(CPeriodicThread_Class,"PeriodicThread","Zero interval!");

				//Exit
				break;
			}

			//Is it first run ?
			if (!bFirstRun ||
				(bFirstRun &&
				 pClass->m_bWaitOneInterval))
				//Wait for the timeout
				if (!pClass->m_pExitEvent->Wait(pClass->m_dwInterval))
					break;

			//Not first run
			bFirstRun=FALSE;

			//Run user proc
			try
			{
				if (pClass->m_pUserProc)
					if (!(*pClass->m_pUserProc)(tsBody,
												pClass->m_lpData))
						//We are aborting
						bExit=TRUE;
					else
						;
				else if (!(*pClass->m_pUserProcNoInit)(pClass->m_lpData))
					//We are aborting
					bExit=TRUE;
			}
			catch (...)
			{
				bExit=TRUE;
			}

			//Do we exit
			if (bExit)
			{
				//Abort it
				pClass->m_bAbort=TRUE;

				//Exit
				return FALSE;
			}
		}

		//Exit value
		BOOL bValue;
		bValue=TRUE;

		if (pClass->m_pUserProc)
		{
			//Call user proc again
			try
			{
				bValue=(*pClass->m_pUserProc)(tsEnd,
											  pClass->m_lpData);
			}
			catch (...)
			{
				bValue=FALSE;
			}
		}

		//Set the exit event
		pClass->m_pEnterEvent->Set();

		//Exit
		return bValue;
	}
	ERROR_HANDLER_STATIC_RETURN(CPeriodicThread_Class,"PeriodicThread",FALSE)
}

BOOL CPeriodicThread::Start(DWORD dwInterval,
							LPVOID lpData,
							BOOL bWaitOneInterval)
{
	try
	{
		//Is the interval legal
		if (!dwInterval)
		{
			//Report it
			ReportError("Start","Invalid interval!");

			//Exit
			return FALSE;
		}

		//Are we running 
		if (m_dwInterval ||
			m_pEnterEvent ||
			m_pExitEvent)
		{
			//Report it
			ReportError("Start","Already running!");

			//Exit
			return FALSE;
		}

		//We are not aborting
		m_bAbort=FALSE;

		//Create all
		m_pEnterEvent=COSManager::CreateEvent();
		m_pExitEvent=COSManager::CreateEvent();

		//Save the user data
		m_lpData=lpData;

		//Save the interval
		m_dwInterval=dwInterval;
		m_bWaitOneInterval=bWaitOneInterval;

		//Set it to auto delete
		m_pThread->SetAutoDelete(TRUE);

		//Run the thread
		m_pThread->Start((LPVOID)this);

		//Wait for the event
		if (m_pEnterEvent->Wait(m_dwThreadTimeout) ||
			m_bAbort)
		{
			//Report it
			if (m_bAbort)
				ReportError("Start","User aborted!");
			else
				ReportError("Start","Timeout waiting for thread!");

			//Delete it all
			delete m_pThread;
			m_pThread=COSManager::CreateThread(PeriodicThread);

			//The events
			delete m_pEnterEvent;
			m_pEnterEvent=NULL;

			delete m_pExitEvent;
			m_pExitEvent=NULL;

			//Exit
			return FALSE;
		}
		else
			return TRUE;
	}
	ERROR_HANDLER_RETURN("Start",FALSE)
}

BOOL CPeriodicThread::Stop()
{
	try
	{
		if (m_bDestructor)
			if (!m_pEnterEvent ||
				!m_pExitEvent)
			{
				//Just delete the thread
				delete m_pThread;

				//Exit
				return TRUE;
			}
			else
				;
		else
			//Are we running 
			if (!m_pEnterEvent ||
				!m_pExitEvent)
			{
				//Report it
				ReportError("Stop","Not running!");

				//Exit
				return FALSE;
			}

		//Are we aborted
		if (!m_bAbort)
		{
			//The thread ID
			DWORD dwThreadID;
			dwThreadID=m_pThread->GetThreadID();

			//Call the thread
			m_pExitEvent->Set();

			//Wait for the thread
			if (GetCurrentThreadId()!=dwThreadID &&
				m_pEnterEvent->Wait(m_dwThreadTimeout))
			{
				//Report it
				ReportError("Stop","Timeout waiting for thread!");

				//Delete it by force
				delete m_pThread;
			}
		}

		//Delete it all
		m_pThread=NULL;

		//Do we need to recreate
		if (!m_bDestructor)
			m_pThread=COSManager::CreateThread(PeriodicThread);

		//The events
		delete m_pEnterEvent;
		m_pEnterEvent=NULL;

		delete m_pExitEvent;
		m_pExitEvent=NULL;

		//Set the interval
		m_dwInterval=0;

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("Stop",FALSE)
}

void CPeriodicThread::SetThreadTimeout(DWORD dwTimeout)
{
	try
	{
		//Save it
		m_dwThreadTimeout=dwTimeout;
	}
	ERROR_HANDLER("SetThreadTimeout")
}

CGenericThread* CPeriodicThread::GetThread()const
{
	return m_pThread;
}

void CPeriodicThread::SetPriority(CGenericThread::ThreadPriority aPriority)
{
	try
	{
		//Set the priority
		m_pThread->SetPriority(aPriority);
	}
	ERROR_HANDLER("SetPriority")
}

KOMODIA_NAMESPACE_END
