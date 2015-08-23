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
#include "SocketThreadManager.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"
#include "../KomodiaInfraLib/OSManager.h"

#include "../KomodiaInfraLib/GenericCriticalSection.h"
#include "../KomodiaInfraLib/GenericThread.h"
#include "../KomodiaInfraLib/GenericEvent.h"
#include "../KomodiaInfraLib/WaitList.h"

#include <objbase.h>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CSocketThreadManager_Class "CSocketThreadManager"

//Timeout for threads to load
#define THREADS_TIMEOUT 20000

//Our window registration status
BOOL CSocketThreadManager::m_bRegisteredWindow=FALSE;

//Our global manager
CSocketThreadManager CSocketThreadManager::m_sManager(TRUE);

CSocketThreadManager::CSocketThreadManager(HINSTANCE hInstance) : CErrorHandler(),
																  m_ulThreadCount(0),
																  m_hInstance(hInstance),
																  m_pThreadData(NULL),
																  m_pCSection(NULL),
																  m_bStaticClass(FALSE),
																  m_bInitialized(FALSE)
{
	try
	{
		//Set the class name
		SetName(CSocketThreadManager_Class);

		//Create the critical section
		m_pCSection=COSManager::CreateCriticalSection();
	}
	ERROR_HANDLER("CSocketThreadManager")
}

CSocketThreadManager::CSocketThreadManager(BOOL bStatic) : CErrorHandler(),
														   m_bStaticClass(bStatic)
{
	try
	{
		//Set the class name
		SetName(CSocketThreadManager_Class);
	}
	ERROR_HANDLER("CSocketThreadManager")
}

CSocketThreadManager::~CSocketThreadManager()
{
	try
	{
		//Are we initialized
		if (m_bInitialized)
			Uninitialize();

		//Delete the CS
		delete m_pCSection;
	}
	ERROR_HANDLER("~CSocketThreadManager")
}

BOOL CSocketThreadManager::Initialize(unsigned long ulThreadCount,
									  LPCSTR lpClassName)
{
	try
	{
		//Are we initialized?
		if (m_bInitialized)
		{
			//Report it
			ReportError("Initialize","Already initialized!");

			//Exit
			return FALSE;
		}

		//Check the number of threads
		if (ulThreadCount>MAX_THREADS)
		{
			//Report it
			ReportError("Initialize","Too many threads to spawn!");

			//Exit
			return FALSE;
		}

		//Check the number of threads
		if (!ulThreadCount)
		{
			//Report it
			ReportError("Initialize","Can't have zero threads!");

			//Exit
			return FALSE;
		}

		//Save the count
		m_ulThreadCount=ulThreadCount;

		//Try to spawn the threads
		return SpawnThreads(lpClassName);
	}
	ERROR_HANDLER_RETURN("Initialize",FALSE)
}

BOOL CSocketThreadManager::Uninitialize()
{
	try
	{
		//Are we initialized?
		if (!m_bInitialized)
			return TRUE;

		//Are we a static class
		if (m_bStaticClass)
			//Deallocate the class
			UnregisterClass();
		else
			//Delete the thread data
			CleanThreads(FALSE);

		//Unitialized
		m_bInitialized=FALSE;

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("Uninitialize",FALSE)
}

BOOL CSocketThreadManager::SpawnThreads(LPCSTR lpClassName)
{
	try
	{
		//Add to this class our thread ID, and a random number
		if (!lpClassName)
		{
			//Calculate our string
			int iStringLength;
			iStringLength=strlen(CSocketThreadManager_Class);

			//Add some size to it
			iStringLength+=50;

			//Allocate the string
			std::auto_ptr<char> pString(new char[iStringLength]);

			//Get our thread ID
			DWORD dwThreadID;
			dwThreadID=GetCurrentThreadId();

			//Get the tick of the system
			DWORD dwProcessID;
			dwProcessID=GetTickCount();
			
			//Create the string
			sprintf(pString.get(),"%s_%lu_%lu",CSocketThreadManager_Class,
											   GetCurrentThreadId(),
											   GetTickCount());

			//And register the window
			//Check if we need to register the class
			if (!m_bRegisteredWindow)
				if (!RegisterClass(pString.get()))
				{
					//Report it
					ReportError("SpawnThreads","Failed to register window!");

					//And quit
					return FALSE;
				}
		
			//Save the class name
			m_sClassName=pString.get();
		}
		else
			m_sClassName=lpClassName;

		//Start creating threads
		//Allocate the thread structure
		m_pThreadData=new ThreadData[m_ulThreadCount];

		//Initialize the data
		for (int iCount=0;iCount<m_ulThreadCount;++iCount)
		{
			m_pThreadData[iCount].bFreeze=FALSE;
			m_pThreadData[iCount].pEvent=NULL;
			m_pThreadData[iCount].hInstance=NULL;
			m_pThreadData[iCount].hWindowHandle=NULL;
			m_pThreadData[iCount].iSocketCount=0;
			m_pThreadData[iCount].iTimeoutCount=0;
			m_pThreadData[iCount].pThread=NULL;
			m_pThreadData[iCount].pCSection=COSManager::CreateCriticalSection();
			m_pThreadData[iCount].pClass=this;
		}

		//Did we have an error?
		BOOL bError;
		bError=FALSE;

		//Our waiting list
		CWaitList aList;

		//Start spawning
		for (int iCounter=0;
			 iCounter<m_ulThreadCount;
			 ++iCounter)
		{
			//Copy the class name
			m_pThreadData[iCounter].sClassName=m_sClassName;

			//Create an event
			m_pThreadData[iCounter].pEvent=COSManager::CreateEvent();

			//Add it to the list
			aList.AddObject(m_pThreadData[iCounter].pEvent,
							TRUE);

			//Set our instance
			m_pThreadData[iCounter].hInstance=m_hInstance;

			//Create extended data
			CreateExtendedData(m_pThreadData[iCounter].aExtension);

			//And create it
			m_pThreadData[iCounter].pThread=COSManager::CreateThread(SocketThread);
			
			//Check the thread has been created
			if (!m_pThreadData[iCounter].pThread->GetThreadID())
			{
				//Report the error
				ReportError("SpawnThreads","Failed to create thread!");

				//We have an error
				bError=TRUE;

				//Exit the loop
				break;
			}
			else
				//Start the thread
				m_pThreadData[iCounter].pThread->Start((LPVOID)&m_pThreadData[iCounter]);
		}

		//Tmp position
		DWORD dwTmp;

		if (aList.Wait(TRUE,
					   dwTmp,
					   THREADS_TIMEOUT))
		{
			//Report the error
			ReportError("SpawnThreads","Timeout waiting for threads!");

			//Close the threads
			CleanThreads(TRUE);

			//Exit
			return FALSE;
		}
		else if (bError)
		{
			//Close the threads
			CleanThreads(FALSE);

			//Exit
			return FALSE;
		}
		else
		{
			//We are OK
			m_bInitialized=TRUE;

			//Done
			return TRUE;
		}
	}
	ERROR_HANDLER_RETURN("SpawnThreads",FALSE)
}

DWORD CSocketThreadManager::SocketThread(LPVOID lpParameter)
{
	try
	{
		//Get the address of our data
		ThreadData* pData;
		pData=(ThreadData*)lpParameter;

		//Initialize the random seed
		srand(GetTickCount()+(unsigned long)pData->pEvent+(unsigned long)pData->hWindowHandle);
		
		//Create the window
		pData->hWindowHandle=CreateWindowEx(0,
											pData->sClassName.c_str(),
											SOCKET_WINDOW_NAME,
											WS_OVERLAPPED,
											0,
											0,
											0,
											0,
											0,
											NULL,
											pData->hInstance,
											NULL);

		//Alert we are done
		pData->pEvent->Set();

		//Check we have this window 
		if (pData->hWindowHandle)
		{
			//Run a message map
			MSG msg;

			while (GetMessage(&msg,
							  NULL,
							  0,
							  0))
			{
				//Translate and dispatch
				TranslateMessage(&msg);

				//Check if it's a timer
				if (msg.message==WM_TIMER)
				{
					//Lock the CS
					CCriticalAutoRelease aRelease(pData->pCSection);

					//Check is it our timer
					TOMap::iterator aTheIterator;
					aTheIterator=pData->pMap.find(msg.wParam);
			
					//Check if we have it
					if (aTheIterator!=pData->pMap.end())
					{
						//Found it
						TimeoutData aData;
						aData=aTheIterator->second;

						//Do we need to delete it
						if (aData.bClearTimeout)
						{
							//Delete it
							KillTimer(pData->hWindowHandle,
									  msg.wParam);

							//Do we have the data to reset ?
							if (aData.pTimer)
								memset(aData.pTimer,
									   0,
									   sizeof(TimerID));

							//Erase the timer
							pData->pMap.erase(aTheIterator);

							//Decrease the count
							--pData->iTimeoutCount;
						}

						//Exit the CS
						aRelease.Exit();

						//Protect it
						try
						{
							//Dispatch the data
							(*(aData.pTimeoutProc))(aData.pData);
						}
						ERROR_HANDLER_STATIC(CSocketThreadManager_Class,"SocketThread - Proc")
					}
					else
					{
						//Release the data
						aRelease.Exit();

						//Dispatch the message
						pData->pClass->ParseDispatchMessage(msg,
															pData);
					}
				}
				else
					//Regular message
					pData->pClass->ParseDispatchMessage(msg,
														pData);
			}
		}
		else
			//Report the error
			ReportStaticError(CSocketThreadManager_Class,"SocketThread","Failed to create window!");

		//Set we are OK
		pData->pEvent->Set();

		//Exit
		return FALSE;
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketThreadManager_Class,"SocketThread",TRUE)
}

int CSocketThreadManager::GetMostAvailableThread()const
{
	try
	{
		//Start position
		int iStartPosition;
		iStartPosition=1;

		int iIndex;
		iIndex=0;

		//Enter the CS
		CCriticalAutoRelease aRelease(m_pCSection);

		//Check for freezing
		while (iIndex<m_ulThreadCount &&
			   m_pThreadData[iIndex].bFreeze)
		{
			//Release the freeze
			m_pThreadData[iIndex].bFreeze=FALSE;

			++iIndex;
			++iStartPosition;
		}

		//Check we aren't in the end
		if (iIndex==m_ulThreadCount)
			return GetMostAvailableThread();
		
		//Start searching the threads
		for (int iCounter=iStartPosition;iCounter<m_ulThreadCount;++iCounter)
			//Check is it larger
			if (m_pThreadData[iCounter].iSocketCount+m_pThreadData[iCounter].iTimeoutCount<
				m_pThreadData[iIndex].iSocketCount+m_pThreadData[iIndex].iTimeoutCount && 
				m_pThreadData[iCounter].pThread->GetThreadID())
				//Check if is freezed
				if (m_pThreadData[iCounter].bFreeze)
					//Release the freeze
					m_pThreadData[iCounter].bFreeze=FALSE;
				else
					//Set the new index
					iIndex=iCounter;

		//Return the value
		return iIndex+1;
	}
	ERROR_HANDLER_RETURN("GetMostAvailableThread",0)
}

HWND CSocketThreadManager::GetWindowHandle(BOOL bTimeout)
{	
	try
	{
		//Shared resource
		CCriticalAutoRelease aRelease(m_pCSection);
	
		//Get the freeiest index
		int iIndex;
		iIndex=GetMostAvailableThread();

		//Check it's valid
		if (!iIndex)
			//Quit
			return 0;
	
		//Check what to increase
		if (bTimeout)
			++m_pThreadData[iIndex-1].iTimeoutCount;
		else
			//Increase the socket count
			++m_pThreadData[iIndex-1].iSocketCount;

		//Done
		return m_pThreadData[iIndex-1].hWindowHandle;
	}
	ERROR_HANDLER_RETURN("GetWindowHandle",0)
}

const CGenericThread* CSocketThreadManager::GetThreadByHWND(HWND hWindowHandle)const
{
	try
	{
		//First find the window handle
		int iIndex;
		iIndex=GetIndexByHWND(hWindowHandle);

		//Check it's valid
		if (!iIndex)
			return NULL;

		//Enter the critical section
		CCriticalAutoRelease aRelease(m_pCSection);

		//Return the thread
		return m_pThreadData[iIndex-1].pThread;
	}
	ERROR_HANDLER_RETURN("GetThreadByHWND",NULL)
}

void CSocketThreadManager::DecreaseSocketCount(HWND hWindowHandle,
											   BOOL bFreeze)
{
	try
	{
		//First find the window handle
		int iIndex;
		iIndex=GetIndexByHWND(hWindowHandle);

		//Check it's valid
		if (!iIndex)
			return;

		//Enter the critical section
		CCriticalAutoRelease aRelease(m_pCSection);

		//Decrement the socket count
		if (m_pThreadData[iIndex-1].iSocketCount>0)
		{
			--m_pThreadData[iIndex-1].iSocketCount;
			m_pThreadData[iIndex-1].bFreeze=bFreeze;
		}
	}
	ERROR_HANDLER("DecreaseSocketCount")
}

void CSocketThreadManager::DecreaseTimeoutCount(HWND hWindowHandle,
												BOOL bFreeze)
{
	try
	{
		//First find the window handle
		int iIndex;
		iIndex=GetIndexByHWND(hWindowHandle);

		//Check it's valid
		if (!iIndex)
			return;

		//Enter the critical section
		CCriticalAutoRelease aRelease(m_pCSection);

		//Decrement the socket count
		if (m_pThreadData[iIndex-1].iTimeoutCount>0)
		{
			--m_pThreadData[iIndex-1].iTimeoutCount;
			m_pThreadData[iIndex-1].bFreeze=bFreeze;
		}
	}
	ERROR_HANDLER("DecreaseTimeoutCount")
}

CSocketThreadManager::ThreadData* CSocketThreadManager::GetDataByHWND(HWND hHandle)const
{
	try
	{
		//Get the index
		int iIndex;
		iIndex=GetIndexByHWND(hHandle);

		//Do we have it?
		if (iIndex)
			return &m_pThreadData[iIndex-1];
		else
			return NULL;
	}
	ERROR_HANDLER_RETURN("GetDataByHWND",NULL)
}

int CSocketThreadManager::GetIndexByHWND(HWND hHandle)const
{
	try
	{
		for (int iCounter=0;iCounter<m_ulThreadCount;++iCounter)
			if (m_pThreadData[iCounter].hWindowHandle==hHandle)
				//Return it
				return iCounter+1;

		//Nothing
		return 0;
	}
	ERROR_HANDLER_RETURN("GetIndexByHWND",0)
}

void CSocketThreadManager::CleanThreads(BOOL bError)
{
	try
	{
		//Our wait list
		CWaitList aList;

		//Start deleting
		if (m_pThreadData)
		{
			//Our counter
			int iCounter;

			//Delete the threads
			for (iCounter=0;
				 iCounter<m_ulThreadCount;
				 ++iCounter)
				//Post a stop message
				if (m_pThreadData[iCounter].pThread->GetThreadID())
				{
					//Send the message
					PostThreadMessage(m_pThreadData[iCounter].pThread->GetThreadID(),
									  WM_QUIT,
									  0,
									  0);

					//Are we nice or not ?
					if (bError)
						//Delete the window
						DestroyWindow(m_pThreadData[iCounter].hWindowHandle);
					else if (m_pThreadData[iCounter].pEvent)
						aList.AddObject(m_pThreadData[iCounter].pEvent,TRUE);
				}

			//Error or not ?
			if (bError)
				//Sleep to allow the threads to quit
				Sleep(1000);
			else
			{
				//Tmp object
				DWORD dwTmp;

				//Wait for threads
				if (aList.Wait(TRUE,
							   dwTmp,
							   THREADS_TIMEOUT))
					ReportError("CleanThreads","Timeout waiting for threads!");
			}

			//Delete the thread classes
			for (iCounter=0;
				 iCounter<m_ulThreadCount;
				 ++iCounter)
			{
				//Delete extension data
				DeleteExtendedData(m_pThreadData[iCounter].aExtension);

				//And delete the thread
				delete m_pThreadData[iCounter].pThread;

				//Delete the CS
				delete m_pThreadData[iCounter].pCSection;

				//Delete the event
				delete m_pThreadData[iCounter].pEvent;

				//Are we nice or not ?
				if (!bError)
					//Delete the window
					DestroyWindow(m_pThreadData[iCounter].hWindowHandle);
			}

			//Delete the structure
			delete [] m_pThreadData;
		}
	}
	ERROR_HANDLER("CleanThreads")
}

CSocketThreadManager::TimerID CSocketThreadManager::RegisterTimeout(int iMS, 
																	LPTimeoutProc pProc, 
																	LPVOID pData,
																	BOOL bClearTimeout,
																	HWND hHandle,
																	TimerID* pTimer,
																	LPTimeoutProc pDeleteProc)
{
	TimerID aTimerID;
	aTimerID.iIndex=0;

	try
	{
		//Do we have a timer ?
		if (pTimer)
			memset(pTimer,0,sizeof(TimerID));

		int iIndex;

		//Create the data structure (to save time on the CS)
		TimeoutData aData;
		aData.pTimeoutProc=pProc;
		aData.pData=pData;
		aData.bClearTimeout=bClearTimeout;
		aData.pTimer=pTimer;
		aData.pDeleteProc=pDeleteProc;

		//Enter the CS
		CCriticalAutoRelease aRelease(m_pCSection);

		//Do we have a handle
		if (hHandle)
		{
			//Get it
			iIndex=GetIndexByHWND(hHandle);

			//Do we have it
			if (!iIndex)
			{
				//Report it
				ReportError("RegisterTimeout","Failed to find handle!");

				//Exit
				return aTimerID;
			}
			else
				--iIndex;			
		}
		else
		{
			//Get the handler
			iIndex=GetMostAvailableThread();

			//Check it's legal
			if (!iIndex)
			{
				//Report the error
				ReportError("RegisterTimeout","Failed to find thread!");

				//Quit
				return aTimerID;
			}

			//Reset the index
			--iIndex;
		}

		//Our timer ID
		unsigned int iTimerID;
		iTimerID=0;

		//Timer counter
		static int sTimerCounter=1000000;

		//Get the timerID
		int iSetTimerID;
		iSetTimerID=sTimerCounter++;

		//Lock the thread data
		CCriticalAutoRelease aRelease2(m_pThreadData[iIndex].pCSection);

		//Increase the timeout count
		++m_pThreadData[iIndex].iTimeoutCount;

		//Register the data
		m_pThreadData[iIndex].pMap.insert(TOMap::value_type(iSetTimerID,aData));

		//Exit the CS
		aRelease2.Exit();
		aRelease.Exit();

		//Register the timeout
		iTimerID=SetTimer(m_pThreadData[iIndex].hWindowHandle,
						  iSetTimerID,
						  iMS,
						  NULL);

		//Check we have the timer
		if (!iTimerID)
		{
			{
				//Remove it
				//Lock the thread data
				CCriticalAutoRelease aRelease2(m_pThreadData[iIndex].pCSection);

				//Increase the timeout count
				--m_pThreadData[iIndex].iTimeoutCount;

				//Register the data
				m_pThreadData[iIndex].pMap.erase(iSetTimerID);
			}

			//Write the error
			ReportError("RegisterTimeout","Failed to create the timer!");

			//Quit
			return aTimerID;
		}

		//Create the data
		aTimerID.iTimerID=iSetTimerID;
		aTimerID.iIndex=iIndex+1;
		aTimerID.iMS=iMS;

		//Do we have a timer ?
		if (pTimer)
			memcpy(pTimer,&aTimerID,sizeof(aTimerID));

		//Done
		return aTimerID;
	}
	ERROR_HANDLER_RETURN("RegisterTimeout",aTimerID)
}

BOOL CSocketThreadManager::RemoveTimeout(TimerID& aTimerID,
										 BOOL bInvokeDeleteProc)
{
	//Check it's a valid timeout
	if (!aTimerID.iIndex)
	{
		//Write the error
		ReportError("RemoveTimeout","Received invalid TimerID");

		//Quit
		return FALSE;
	}

	try
	{
		//First unregister the timer
		if (!KillTimer(m_pThreadData[aTimerID.iIndex-1].hWindowHandle,
					   aTimerID.iTimerID))
			//Report it
			ReportErrorOS("RemoveTimeout","Failed to delete timer!");

		{
			//Lock the timeout manager
			CCriticalAutoRelease aRelease(m_pThreadData[aTimerID.iIndex-1].pCSection);

			//Do we need to invoke the delete proc?
			if (!bInvokeDeleteProc)
				//No remove from the map
				m_pThreadData[aTimerID.iIndex-1].pMap.erase(aTimerID.iTimerID);
			else
			{
				//We need to look for the data
				TOMap::iterator aIterator;
				aIterator=m_pThreadData[aTimerID.iIndex-1].pMap.find(aTimerID.iTimerID);

				//Do we have it?
				if (aIterator!=m_pThreadData[aTimerID.iIndex-1].pMap.end())
				{
					//Do we have the delete proc?
					if (aIterator->second.pDeleteProc)
						//Invoke it
						(*aIterator->second.pDeleteProc)(aIterator->second.pData);

					//Remove the entry
					m_pThreadData[aTimerID.iIndex-1].pMap.erase(aIterator);
				}
				else
					//Report it
					ReportError("RemoveTimeout","Failed to find data!");
			}
					
			//Decrease the count
			--m_pThreadData[aTimerID.iIndex-1].iTimeoutCount;

			//Set the old timer ID
			aTimerID.iIndex=0;
			aTimerID.iMS=0;
			aTimerID.iTimerID=0;
		}

		//Done 
		return TRUE;
	}
	ERROR_HANDLER_RETURN("RemoveTimeout",FALSE)
}

BOOL CSocketThreadManager::UnregisterClass()
{
	if (!m_bRegisteredWindow)
		return TRUE;

	try
	{
		//Unregister the class
		m_bRegisteredWindow=!::UnregisterClass(m_sClassName.c_str(),
											   m_hInstance);

		//Return the result
		return !m_bRegisteredWindow;
	}
	ERROR_HANDLER_RETURN("UnregisterClass",FALSE)
}

BOOL CSocketThreadManager::RegisterClass(LPCSTR lpClassName)
{
	//Is the window registered
	if (m_bRegisteredWindow)
		return TRUE;

	try
	{
		WNDCLASS wc;

		/* Fill in window class structure with parameters that describe the       */
		/* main window.                                                           */

		wc.style = 0;										  /* Class style(s).                    */
		wc.lpfnWndProc = (WNDPROC)DefWindowProc;       /* Function to retrieve messages for  */
											/* windows of this class.             */
		wc.cbClsExtra = 0;                  /* No per-class extra data.           */
		wc.cbWndExtra = 0;                  /* No per-window extra data.          */
		wc.hIcon = NULL;				    /* Icon name from .RC        */
		wc.hInstance = m_hInstance;          /* Application that owns the class.   */
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName =  NULL;   /* Name of menu resource in .RC file. */
		wc.lpszClassName = lpClassName ; /* Name used in call to CreateWindow. */

		/* Register the window class and return success/failure code. */
		m_bRegisteredWindow=::RegisterClass(&wc)!=0;

		//And return the value
		return m_bRegisteredWindow;
	}
	ERROR_HANDLER_RETURN("RegisterClass",FALSE)
}

BOOL CSocketThreadManager::ReSetTimeout(const TimerID &rTimerID)
{
	//Check if anyone is smartass
	if (!rTimerID.iTimerID)
	{
		//Report it
		ReportError("ReSetTimeout","Someone send an empty TimerID!");

		//And exit
		return FALSE;
	}

	try
	{
		//Reset the timer
		int iResult;
		iResult=SetTimer(m_pThreadData[rTimerID.iIndex-1].hWindowHandle,
						 rTimerID.iTimerID,
						 rTimerID.iMS,
						 NULL);

		//Check the result
		if (!iResult)
			ReportError("ReSetTimeout","Failed to reset the timeout!");

		//Done
		return iResult!=0;
	}
	ERROR_HANDLER_RETURN("ReSetTimeout",FALSE)
}

const std::string& CSocketThreadManager::GetClassName()const
{
	return m_sClassName;
}

BOOL CSocketThreadManager::IsInitialized()const
{
	return m_bInitialized;
}

unsigned long CSocketThreadManager::GetNumberOfThreads()const
{
	return m_ulThreadCount;
}

void CSocketThreadManager::ParseDispatchMessage(const MSG& rMsg,
												ThreadData* pData)
{
	//Just dispatch it
	DispatchMessage(&rMsg);
}

void CSocketThreadManager::CreateExtendedData(ThreadDataExtend& rData)
{
}

void CSocketThreadManager::DeleteExtendedData(ThreadDataExtend& rData)
{
}

KOMODIA_NAMESPACE_END