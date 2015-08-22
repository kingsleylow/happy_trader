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
#include "ManagedThread.h"

#include "ErrorHandlerMacros.h"
#include "GenericEvent.h"
#include "OSManager.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CManagedThread_Class "CManagedThread"

CManagedThread::CManagedThread(LPManagedThreadProc pProc) : CErrorHandler(),
															m_pThread(NULL),
															m_pUserProc(pProc),
															m_pEnterEvent(NULL),
															m_pExitEvent(NULL),
															m_dwThreadTimeout(20000),
															m_bDestructor(FALSE)
{
	try
	{
		//Set our name
		SetName(CManagedThread_Class);

		//Allocate the thread
		m_pThread=COSManager::CreateThread(ManagedThread);
	}
	ERROR_HANDLER("CManagedThread")
}

CManagedThread::~CManagedThread()
{
	try
	{
		//Indicate tremination
		m_bDestructor=TRUE;

		//Delete our thread
		Stop();
	}
	ERROR_HANDLER("~CManagedThread")
}

DWORD CManagedThread::ManagedThread(LPVOID lpData)
{
	try
	{
		//Get our class
		CManagedThread* pClass;
		pClass=(CManagedThread*)lpData;

		//Do we exit
		BOOL bExit;
		bExit=FALSE;

		//Set we are alive
		if (pClass->m_pEnterEvent)
			pClass->m_pEnterEvent->Set();
		else
			return FALSE;

		//The value
		DWORD dwValue;

		//Call the user proc
		try
		{
			dwValue=(*pClass->m_pUserProc)(pClass->m_pExitEvent,
										   pClass->m_lpData);
		}
		catch (...)
		{
			dwValue=FALSE;
		}

		//Set the exit event
		pClass->m_pEnterEvent->Set();

		//Exit
		return dwValue;
	}
	ERROR_HANDLER_STATIC_RETURN(CManagedThread_Class,"ManagedThread",FALSE)
}

BOOL CManagedThread::Start(LPVOID lpData)
{
	try
	{
		//Are we running 
		if (m_pEnterEvent ||
			m_pExitEvent)
		{
			//Report it
			ReportError("Start","Already running!");

			//Exit
			return FALSE;
		}

		//Create all
		m_pEnterEvent=COSManager::CreateEvent();
		m_pExitEvent=COSManager::CreateEvent();

		//Save the user data
		m_lpData=lpData;

		//Set it to auto delete
		m_pThread->SetAutoDelete(TRUE);

		//Run the thread
		m_pThread->Start((LPVOID)this);

		//Wait for the event
		if (m_pEnterEvent->Wait(m_dwThreadTimeout))
		{
			//Report it
			ReportError("Start","Timeout waiting for thread!");

			//Delete it all
			delete m_pThread;
			m_pThread=COSManager::CreateThread(ManagedThread);

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

BOOL CManagedThread::Stop()
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

		//Delete it all
		m_pThread=NULL;

		//Do we need to recreate
		if (!m_bDestructor)
			m_pThread=COSManager::CreateThread(ManagedThread);

		//The events
		delete m_pEnterEvent;
		m_pEnterEvent=NULL;

		delete m_pExitEvent;
		m_pExitEvent=NULL;

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("Stop",FALSE)
}

void CManagedThread::SetThreadTimeout(DWORD dwTimeout)
{
	try
	{
		//Save it
		m_dwThreadTimeout=dwTimeout;
	}
	ERROR_HANDLER("SetThreadTimeout")
}

CGenericThread* CManagedThread::GetThread()const
{
	return m_pThread;
}

void CManagedThread::SetPriority(CGenericThread::ThreadPriority aPriority)
{
	try
	{
		//Set the priority
		m_pThread->SetPriority(aPriority);
	}
	ERROR_HANDLER("SetPriority")
}

KOMODIA_NAMESPACE_END
