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
#include "AsyncSocket.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"

#include "../KomodiaInfraLib/OSManager.h"
#include "../KomodiaInfraLib/GenericCriticalSection.h"
#include "../KomodiaInfraLib/GenericEvent.h"

#include <conio.h>

#include <vector>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#pragma warning(disable : 4996)


// ----------------------------- CAsyncShutdown start ---------------------

CAsyncSocket::CAsyncShutdown::CAsyncShutdown() : CSocketBase()
{
	try
	{
		//Register myself
		SetName(CAsyncShutdown_Class);

		//Register for shutdown
		RegisterShutdown(this);
	}
	ERROR_HANDLER("CAsyncShutdown")
}

CAsyncSocket::CAsyncShutdown::~CAsyncShutdown()
{
}

void CAsyncSocket::CAsyncShutdown::NotifyShutdown()
{
	try
	{
		//Socket shutdown!
		CAsyncSocket::Shutdown();
	}
	ERROR_HANDLER("NotifyShutdown")
}

// ----------------------------- CAsyncShutdown end ---------------------

//Static members
//Our instance
HINSTANCE CAsyncSocket::m_hInstance=0;

//Are we initialized
BOOL CAsyncSocket::m_bInitialized=0;

//Our thread manager
CSocketThreadManagerImp* CAsyncSocket::m_pThreadManager=NULL;

//Are we shutting down?
BOOL CAsyncSocket::m_bShuttingDown=FALSE;

CAsyncSocket::CAsyncSocket() : CSocketBase(),
							   m_bList(FALSE),
							   m_bTimeout(FALSE),
							   m_pLocalThreadManager(NULL),
							   m_hLocalWindowHandle(0),
							   m_bFreeze(FALSE),
							   m_bBlocking(FALSE),
							   m_iMsg(0),
							   m_lEvent(0),
							   m_iSocketID(0),
							   m_pBlockedBuffer(NULL),
							   m_bClosing(FALSE),
							   m_ucEvents(0),
							   m_pSendEvent(NULL),
							   m_bBlockSend(TRUE),
							   m_pThread(NULL),
							   m_bDeleting(FALSE),
							   m_bDetached(FALSE)
{
	try
	{
		//Create the event
		m_pSendEvent=COSManager::CreateEvent();

		//Initialize all data
		Initialize();

		//We are created
		SocketCreated();
	}
	ERROR_HANDLER("CAsyncSocket")
}

CAsyncSocket::~CAsyncSocket()
{
	try
	{
		//Do we have any timers ?
		//Can't have
		if (m_aTimerID.iTimerID)
			GetThreadManager()->RemoveTimeout(m_aTimerID);

		if (m_aDeleteTimerID.iTimerID)
			GetThreadManager()->RemoveTimeout(m_aDeleteTimerID);

		//If we have a thread manager remove ourselves from it
		if (GetThreadManager())
			//Remove from the thread manager
			GetThreadManager()->DecreaseSocketCount(GetWindowHandle(),
													m_bFreeze);

		//Are we a detached socket?
		if (m_bDetached)
			delete GetThreadManager();

		//Delete the blocked buffer
		delete m_pBlockedBuffer;

		//Delete the event
		delete m_pSendEvent;
	}
	ERROR_HANDLER("~CAsyncSocket")
}

void CAsyncSocket::DetachedSocketThread()
{
	try
	{
		//Create a new socket imp
		CSocketThreadManagerImp* pClone;
		pClone=CloneThreadManager(1);

		//Do we have it?
		if (!pClone)
			//Report it
			ReportError("DetachedSocketThread","Failed to create detach manager, will resume as normal!");
		else
		{
			//Set it as local
			if (!SetLocalThreadManager(pClone))
				ReportError("DetachedSocketThread","Failed to set manager!");
			else
				//Set we are detahced
				m_bDetached=TRUE;
		}
	}
	ERROR_HANDLER("DetachedSocketThread")
}

BOOL CAsyncSocket::SetHandlers()
{
	try
	{
		//Do we have a manager ?
		if (!m_pThreadManager)
		{
			//How many threads we want
			int iThreads;
			if (CSocketBase::IsMultiThreaded())
				iThreads=CSocketBase::GetNumberOfThreads();
			else
				iThreads=1;

			//Initialize as multithreaded
			m_pThreadManager=new CSocketThreadManagerImp(m_hInstance);
			if (!m_pThreadManager->Initialize(iThreads))
			{
				//Report it
				ReportStaticError(CAsyncSocket_Class,"SetHandlers","Failed to initialize the thread manager!");

				//Delete it
				delete m_pThreadManager;
				m_pThreadManager=NULL;

				//Exit
				return FALSE;
			}
		}

		//Created !!
		//Success
		return TRUE;
	}
	ERROR_HANDLER_STATIC_RETURN(CAsyncSocket_Class,"CAsyncSocket",FALSE)
}

HINSTANCE CAsyncSocket::GetInstance()
{
	//Returns the instance of the application, must be overided
	return m_hInstance;
}

void CAsyncSocket::AddSocketToList()
{
	try
	{
		//Allocate our window
		AllocateHandle();

		//We are in the list
		m_bList=TRUE;
	}
	ERROR_HANDLER("AddSocketToList")
}

int CAsyncSocket::GetSocketID()const
{
	return m_iSocketID;
}

void CAsyncSocket::SetInstance(HINSTANCE hInst)
{
	m_hInstance=hInst;
}

HWND CAsyncSocket::GetWindowHandle()const
{
	//Check if we are multithreaded ?
	return m_hLocalWindowHandle;
}

void CAsyncSocket::RemoveSocketFromList()
{
	try
	{
		if (m_bList)
		{
			//Remove from thread manager
			GetThreadManager()->RemoveSocket(this);

			//Not in list
			m_bList=FALSE;

			//Deallocate the handle
			DeAllocateHandle();
		}
	}
	ERROR_HANDLER("RemoveSocketFromList")
}

BOOL CAsyncSocket::SetTimeout(int iMs)
{
	try
	{
		//Get the window handle
		HWND hWindowHandle;
		hWindowHandle=GetWindowHandle();

		if (!hWindowHandle)
			return FALSE;

		//Set the timer
		m_bTimeout=SetTimer(hWindowHandle,
							GetAsyncHandle(),
							iMs,
							NULL);

		return m_bTimeout;
	}
	ERROR_HANDLER_RETURN("SetTimeout",FALSE)
}

BOOL CAsyncSocket::KillTimer()
{
	try
	{
		//Get the handle
		HWND hWindowHandle;
		hWindowHandle=GetWindowHandle();

		if (!hWindowHandle || 
			!m_bTimeout)
			return FALSE;

		//No timer in any case
		m_bTimeout=FALSE;

		//Try to kill the timer
		BOOL bResult;
		bResult=::KillTimer(hWindowHandle,
							GetAsyncHandle());

		if (!bResult)
			//Fire an error
			ReportError("KillTimer");

		return bResult;
	}
	ERROR_HANDLER_RETURN("KillTimer",FALSE)
}

void CAsyncSocket::Shutdown()
{
	try
	{
		//Only if initialized
		if (!m_bInitialized)
			return;

		//Indicate we're shutting down
		m_bShuttingDown=TRUE;

		//Delete the thread manager
		if (m_pThreadManager)
		{
			delete m_pThreadManager;
			m_pThreadManager=NULL;
		}

		//Not initialized anymore
		m_bInitialized=FALSE;
	}
	ERROR_HANDLER_STATIC(CAsyncSocket_Class,"Shutdown")
}

BOOL CAsyncSocket::IsTimeout() const
{
	return m_bTimeout;
}

void CAsyncSocket::Initialize()
{
	try
	{
		//Initialize all data
		if (!m_bInitialized && 
			CSocketBase::IsInitialized())
		{
			//Create the CS
			m_pCSection=COSManager::CreateCriticalSection();

			//Create handlers
			if (!SetHandlers())
			{
				//Report it
				ReportStaticError(CAsyncSocket_Class,"CAsyncSocket","Failed to init handlers!");

				//Exit
				return;
			}

			//Create a new socket to do the shutdown
			CAsyncShutdown* pShutdown;
			pShutdown=new CAsyncShutdown;

			//The class registers itself
			m_bInitialized=TRUE;
		}
	}
	ERROR_HANDLER_STATIC(CAsyncSocket_Class,"Initialize")
}

CSocketThreadManagerImp* CAsyncSocket::GetThreadManager()const
{
	if (!m_pLocalThreadManager)
		return m_pThreadManager;
	else
		return m_pLocalThreadManager;
}

void CAsyncSocket::AllocateHandle()
{
	try
	{
		//No thread
		m_pThread=NULL;

		//Allocate the handle
		m_hLocalWindowHandle=GetThreadManager()->GetWindowHandle(this);

		//Get the thread
		m_pThread=GetThreadManager()->GetThreadByHWND(m_hLocalWindowHandle);
	}
	ERROR_HANDLER_RETHROW("AllocateHandle")
}

void CAsyncSocket::DeAllocateHandle()
{
	//No handle
	m_hLocalWindowHandle=0;

	//No thread
	m_pThread=NULL;
}

BOOL CAsyncSocket::DisableAsync()
{
	try
	{
		//Quit if not ok
		if (!CheckAsyncSocketValid())
			return FALSE;

		//Set event to read / write / close / oob
		int iResult;

		iResult=WSAAsyncSelect(GetAsyncHandle(),
							   GetWindowHandle(),
							   0,
							   0);
		if (iResult)
		{
			//Report it
			SetLastError("DisableAsync");

			//Exit
			return FALSE;
		}
		
		return TRUE;
	}
	ERROR_HANDLER_RETURN("DisableAsync",FALSE)
}

BOOL CAsyncSocket::CheckAsyncSocketValid() const
{
	try
	{
		//Check if socket is invalid
		if (GetAsyncHandle()==INVALID_SOCKET)
		{
			//Report it
			ReportError("CheckAsyncSocketValid","Operation made on non existant socket!");

			//Exit
			return FALSE;
		}

		//OK
		return TRUE;
	}
	ERROR_HANDLER_RETURN("CheckAsyncSocketValid",FALSE)
}

void CAsyncSocket::SocketClosing()
{
	try
	{
		//Are we valid
		if (m_bClosing ||
			GetAsyncHandle()==INVALID_SOCKET)
			return;

		//Indicate we are closing
		m_bClosing=TRUE;

		//Signal the event
		m_pSendEvent->Set();

		//Do we have a regular timer
		if (m_bTimeout)
			KillTimer();

		//Do we have a system timer ?
		if (m_aTimerID.iTimerID)
			KillSystemTimer();

		//Kill all the messages
		m_iMsg=0;
		m_lEvent=0;

		//Set it
		if (WSAAsyncSelect(GetAsyncHandle(),
						   GetWindowHandle(),
						   0,
						   0))
			SetLastError("SocketClosing");
	}
	ERROR_HANDLER("SocketClosing")
}

void CAsyncSocket::FreezeThread()
{
	m_bFreeze=TRUE;
}

BOOL CAsyncSocket::ReBlock()
{
	if (m_bBlocking)
		return TRUE;

	try
	{
		//Quit if not ok
		if (!CheckAsyncSocketValid())
			return FALSE;

		if (Block())
			//Set to reblock
			m_bBlocking=TRUE;

		//And quit
		return TRUE;
	}
	ERROR_HANDLER_RETURN("ReBlock",FALSE)
}

int CAsyncSocket::InternalWSAAsyncSelect(unsigned int wMsg, 
										 long lEvent)
{
	try
	{
		//Cache the values
		m_iMsg=wMsg;
		m_lEvent=lEvent;

		//Message pairs
		typedef struct _MsgPair
		{
			unsigned int	uiMsg;
			AsyncEvents		aEvents;
		} MsgPair;

		//Our max events
		static const int iMaxEvents=4;

		//Our events data
		static const MsgPair aMsgPair[iMaxEvents]={{FD_READ,aeReceive},
												   {FD_WRITE,aeSend},
												   {FD_OOB,aeOOB},
												   {FD_CLOSE,aeClose}};

		//Check if the messages are allowed
		for (int iCounter=0;
			 iCounter<iMaxEvents;
			 ++iCounter)
			if ((m_lEvent & aMsgPair[iCounter].uiMsg) &&
				(m_ucEvents & ((unsigned char)aMsgPair[iCounter].aEvents)))
				//Remove it
				m_lEvent^=aMsgPair[iCounter].uiMsg;

		if (m_bBlocking)
			return 0;
		else
			//And call the async select
			return WSAAsyncSelect(GetAsyncHandle(),
								  GetWindowHandle(),
								  wMsg,
								  lEvent);
	}
	ERROR_HANDLER_RETURN("InternalWSAAsyncSelect",GetErrorCode())
}

BOOL CAsyncSocket::ClearEvents()
{
	try
	{
		//Quit if not ok
		if (!CheckAsyncSocketValid())
			return FALSE;

		//Do it
		return !InternalWSAAsyncSelect(0,0);
	}
	ERROR_HANDLER_RETURN("ClearEvents",FALSE)
}

BOOL CAsyncSocket::ReAsync()
{
	if (!m_bBlocking)
		return TRUE;

	try
	{
		//Quit if not ok
		if (!CheckAsyncSocketValid())
			return FALSE;

		//First disable the events
		int iResult;
		iResult=WSAAsyncSelect(GetAsyncHandle(),
							   GetWindowHandle(),
							   m_iMsg,
							   m_lEvent);

		if (iResult)
		{
			//Report it
			SetLastError("ReAsync");

			//Exit
			return FALSE;
		}

		//Set to async
		m_bBlocking=FALSE;

		//And quit
		return TRUE;
	}
	ERROR_HANDLER_RETURN("ReAsync",FALSE)
}

BOOL CAsyncSocket::Block()
{
	try
	{
		//First disable the events
		int iResult;
		iResult=WSAAsyncSelect(GetAsyncHandle(),
							   GetWindowHandle(),
							   0,
							   0);

		if (iResult)
		{
			//Report it
			SetLastError("Block");

			//Exit
			return FALSE;
		}

		unsigned long ulBlocking;
		ulBlocking=0;

		//And return to non-blocking
		iResult=ioctlsocket(GetAsyncHandle(),
							FIONBIO,
							&ulBlocking);

		if (iResult)
		{
			//Report it
			SetLastError("Block");

			//Exit
			return FALSE;
		}

		return TRUE;
	}
	ERROR_HANDLER_RETURN("Block",FALSE)
}

BOOL CAsyncSocket::IsBlocking() const
{
	return m_bBlocking;
}

void CAsyncSocket::AllowBlockedBuffer(BOOL bAllow)
{
	try
	{
		//Do we have a buffer
		if (bAllow && 
			!m_pBlockedBuffer)
			m_pBlockedBuffer=new CBlockedBuffer(this);
		else if (!bAllow && 
				 m_pBlockedBuffer)
		{
			//Delete and reset the blocked buffer
			delete m_pBlockedBuffer;
			m_pBlockedBuffer=NULL;
		}
	}
	ERROR_HANDLER("AllowBlockedBuffer")
}

CBlockedBuffer* CAsyncSocket::GetBlockedBuffer()const
{
	return m_pBlockedBuffer;
}

BOOL CAsyncSocket::SendBlockedBuffer(const CBlockedBuffer::CBlockedData& rData)
{
	//No implemented at this level
	return FALSE;
}

CSocketThreadManagerImp* CAsyncSocket::GetSocketThreadManager()
{
	return m_pThreadManager;
}

BOOL CAsyncSocket::SocketConnected(int iErrorCode,
								   BOOL bNoEvent)
{
	try
	{
		//Did we have an error
		if (iErrorCode)
			if (!bNoEvent)
				return OnSocketConnect(iErrorCode);
			else
				return FALSE;

		//Save the connect value
		BOOL bConnectValue;
		if (!bNoEvent)
			bConnectValue=OnSocketConnect(0);
		else
			bConnectValue=TRUE;

		//Try to set it to async
		if (!SetAsync())
			return FALSE;
		else
			return bConnectValue;
	}
	ERROR_HANDLER_RETURN("SocketConnected",FALSE)
}

BOOL CAsyncSocket::SocketReceive(int iErrorCode,
								 BOOL bNoEvent)
{
	try
	{
		//Do we need to call event?
		if (!bNoEvent)
			return OnSocketReceive(iErrorCode);
		else
			return FALSE;
	}
	ERROR_HANDLER_RETURN("SocketConnected",FALSE)
}

BOOL CAsyncSocket::SocketAccept(int iErrorCode,
							    BOOL bNoEvent)
{
	try
	{
		//Do we need to call event?
		if (!bNoEvent)
			return OnSocketAccept(iErrorCode);
		else
			return FALSE;
	}
	ERROR_HANDLER_RETURN("SocketConnected",FALSE)
}

BOOL CAsyncSocket::SocketClosed(int iErrorCode,
								BOOL bNoEvent)
{
	try
	{
		//Kill the timer
		CAsyncSocket::SocketClosing();

		//Remove from socket list
		RemoveSocketFromList();

		//Call user data
		if (!bNoEvent)
			return OnSocketClose(iErrorCode);
		else
			return TRUE;
	}
	ERROR_HANDLER_RETURN("SocketClosed",FALSE)
}

BOOL CAsyncSocket::SocketWrite(int iErrorCode)
{
	try
	{
		//Notify the event
		m_pSendEvent->Set();

		//Call user data
		return OnSocketWrite(iErrorCode);
	}
	ERROR_HANDLER_RETURN("SocketWrite",FALSE)
}

void CAsyncSocket::SimpleMessageMap()
{
	try
	{
		//If there is no message map, then receive won't work
		MSG msg;

		//Set the message map to zeros
		memset(&msg,
			   0,
			   sizeof(msg));

		while (GetMessage(&msg,
						  NULL,
						  0,
						  0))
		{
			//Dispatch the message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	ERROR_HANDLER_STATIC(CAsyncSocket_Class,"SimpleMessageMap")
}

void CAsyncSocket::SimpleMessageMap(DWORD dwRunTimeMS)
{
	try
	{
		//If there is no message map, then receive won't work
		MSG msg;

		//Set the message map to zeros
		memset(&msg,
			   0,
			   sizeof(msg));

		//Start loop
		DWORD dwLoopStart;
		dwLoopStart=GetTickCount();

		//Until a key was hit
		while (GetTickCount()-dwLoopStart<dwRunTimeMS)
		{
			while (PeekMessage(&msg, 
							   NULL, 
							   0, 
							   0, 
							   PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}

			//Sleep so wont be hanged
			Sleep(1);
		}
    }
	ERROR_HANDLER_STATIC(CAsyncSocket_Class,"SimpleMessageMap")
}

void CAsyncSocket::SimpleMessageMap(LPMapProc pProc,
								    BOOL bStopWhenTRUE,
								    DWORD dwSleep)
{
	try
	{
		//Check if we have the proc
		if (!pProc)
		{
			//Report it
			ReportStaticError(CAsyncSocket_Class,"SimpleMessageMap","Recieved null proc!");

			//Exit
			return;
		}

		//If there is no message map, then receive won't work
		MSG msg;

		//Set the message map to zeros
		memset(&msg,
			   0,
			   sizeof(msg));

		//Stop flag
		BOOL bStop;
		bStop=FALSE;

		//Start the loop
		while (!bStop &&
			   (*pProc)()!=bStopWhenTRUE)
		{
			while (!(bStop=(*pProc)()==bStopWhenTRUE) &&
				   PeekMessage(&msg, 
							   NULL, 
							   0, 
							   0, 
							   PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}

			//Do we need to sleep?
			if (!bStop)
				//No messages
				Sleep(dwSleep);
		}
	}
	ERROR_HANDLER_STATIC(CAsyncSocket_Class,"SimpleMessageMap")
}

char CAsyncSocket::SimpleMessageMapKey(DWORD dwSleep)
{
	try
	{
		//If there is no message map, then receive won't work
		MSG msg;

		//Set the message map to zeros
		memset(&msg,
			   0,
			   sizeof(msg));

		//Until a key was hit
		while (!kbhit())
		{
			while (!kbhit() &&
				   PeekMessage(&msg, 
							   NULL, 
							   0, 
							   0, 
							   PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}

			//Do we need to sleep
			if (!kbhit())
				//Sleep so wont be hanged
				Sleep(dwSleep);
		}
		
		//Return the char
		return getch();
    }
	ERROR_HANDLER_STATIC_RETURN(CAsyncSocket_Class,"SimpleMessageMapKey",0)
}

BOOL CAsyncSocket::SetSystemTimeout(int iMS)
{
	try
	{
		//Do we have a timeout
		if (m_aTimerID.iTimerID)
		{
			//Report it
			ReportError("SetSystemTimeout","Please kill previous timer!");

			//Exit
			return FALSE;
		}

		//Create the timer
		m_aTimerID=GetThreadManager()->RegisterTimeout(iMS,
													   SystemTimerProc,
													   this,
													   TRUE,
													   GetWindowHandle(),
													   &m_aTimerID);

		//Do we have it
		if (!m_aTimerID.iTimerID)
		{
			//Report it
			ReportError("SetSystemTimeout","Failed creating the timer!");

			//Exit
			return FALSE;
		}
		else
			return TRUE;
	}
	ERROR_HANDLER_RETURN("SetSystemTimeout",FALSE)
}

BOOL CAsyncSocket::KillSystemTimer()
{
	try
	{
		//Only if we have one
		if (!m_aTimerID.iTimerID)
			return TRUE;

		return GetThreadManager()->RemoveTimeout(m_aTimerID);
	}
	ERROR_HANDLER_RETURN("KillSystemTimer",FALSE)
}

void CAsyncSocket::SystemTimeout()
{
	//Nothing to do
}

void CAsyncSocket::SystemTimerProc(LPVOID lpData)
{
	try
	{
		//Get the class
		CAsyncSocket* pClass;
		pClass=(CAsyncSocket*)lpData;

		//Kill timer
		memset(&pClass->m_aTimerID,
			   0,
			   sizeof(pClass->m_aTimerID));

		//Invoke handler
		pClass->SystemTimeout();
	}
	ERROR_HANDLER_STATIC(CAsyncSocket_Class,"SystemTimerProc")
}

void CAsyncSocket::DeleteTimerProc(LPVOID lpData)
{
	try
	{
		//Get the class
		CAsyncSocket* pClass;
		pClass=(CAsyncSocket*)lpData;

		//Call the delete
		pClass->DeleteSocketFromThread();
	}
	ERROR_HANDLER_STATIC(CAsyncSocket_Class,"SystemTimerProc")
}

BOOL CAsyncSocket::HasSystemTimer()const
{
	return m_aTimerID.iTimerID!=0;
}

void CAsyncSocket::OnSocketDelete(CGenericEvent* pEvent)
{
	try
	{
		//Delete ourselves
		delete this;

		//Do we have an event?
		if (pEvent)
			pEvent->Set();
	}
	ERROR_HANDLER_STATIC(CAsyncSocket_Class,"OnSocketDelete")
}

void CAsyncSocket::ForceReceiveEvent()
{
	try
	{
		//Are we valid ?
		//And do we have a window
		if (!m_bList)
			OnSocketDelete();
		else
			PostMessage(GetWindowHandle(),
						WM_SOCKET_FORCED,
						(WPARAM)GetAsyncHandle(),
					    0);
	}
	ERROR_HANDLER("ForceReceiveEvent")
}

void CAsyncSocket::DeleteSocketFromThread()
{
	try
	{
		//We are closing
		SocketClosing();

		//Are we valid ?
		//And do we have a window
		if (!m_bList)
			OnSocketDelete();
		else
			PostMessage(GetWindowHandle(),
						WM_SOCKET_DELETE,
						(WPARAM)GetAsyncHandle(),
					    0);
	}
	ERROR_HANDLER("OnSocketDelete")
}

void CAsyncSocket::DeleteSocketFromThread(DWORD dwTimeToWait)
{
	try
	{
		if (!dwTimeToWait)
			DeleteSocketFromThread();
		else
		{
			//We are closing
			SocketClosing();

			//Are we valid ?
			//And do we have a window
			if (!m_bList)
				OnSocketDelete();
			else
			{
				//Create a timer
				m_aDeleteTimerID=GetThreadManager()->RegisterTimeout(dwTimeToWait,
																     DeleteTimerProc,
																     (LPVOID)this,
																     TRUE,
																     GetWindowHandle(),
																     &m_aDeleteTimerID);

				//Do we have the timer ?
				if (!m_aDeleteTimerID.iTimerID)
				{
					//Report it
					ReportError("DeleteSocketFromThread","Failed to create timeout!");

					//Run regular delete
					DeleteSocketFromThread();
				}
			}
		}
	}
	ERROR_HANDLER("OnSocketDelete")
}

void CAsyncSocket::DeleteSocketFromThreadWait()
{
	try
	{
		//We are closing
		SocketClosing();

		//Are we valid ?
		//And do we have a window
		if (!m_bList)
			OnSocketDelete();
		else
		{
			//Create the event
			CGenericEvent* pEvent;
			pEvent=COSManager::CreateEvent();

			//Post the message
			PostMessage(GetWindowHandle(),
						WM_SOCKET_DELETE,
						(WPARAM)GetAsyncHandle(),
					    (LPARAM)pEvent);

			//Wait for the event
			if (pEvent->Wait(15000))
				//Report it
				ReportStaticError(CAsyncSocket_Class,"DeleteSocketFromThreadWait","Timeout waiting for event!");
			else
				delete pEvent;
		}
	}
	ERROR_HANDLER("OnSocketDelete")
}

void CAsyncSocket::SocketCreated()
{
	try
	{
		//No timer
		memset(&m_aTimerID,
			   0,
			   sizeof(m_aTimerID));

		//No delete timer
		memset(&m_aDeleteTimerID,
			   0,
			   sizeof(m_aDeleteTimerID));

		//Set values for new socket
		m_iSocketID=0;
		m_bBlocking=FALSE;
		m_bFreeze=FALSE;
		m_lEvent=0;
		m_hLocalWindowHandle=0;
		m_bList=FALSE;
		m_bClosing=FALSE;
	}
	ERROR_HANDLER("SocketCreated")
}

BOOL CAsyncSocket::IsClosing()const
{
	return m_bClosing;
}

void CAsyncSocket::DisableEvents(unsigned char ucEvents)
{
	try
	{
		//Save the events
		m_ucEvents=ucEvents;
	}
	ERROR_HANDLER("DisableEvents")
}

BOOL CAsyncSocket::HasTimeout()const
{
	return m_bTimeout;
}

CSocketThreadManagerImp::TimerID CAsyncSocket::RegisterTimeout(int iMS,
															   CSocketThreadManagerImp::LPTimeoutProc pProc,
															   LPVOID pData,
															   BOOL bClearTimeout,
															   CSocketThreadManagerImp::TimerID* pTimer)
{
	//Delegate the call
	return GetThreadManager()->RegisterTimeout(iMS,
											   pProc,
											   pData,
											   bClearTimeout,
											   GetWindowHandle(),
											   pTimer);
}

BOOL CAsyncSocket::SetLocalThreadManager(CSocketThreadManagerImp* pManager)
{
	try
	{
		//We can do it only if we are not created
		if (GetAsyncHandle()!=INVALID_SOCKET)
		{
			//Report it
			ReportError("SetLocalThreadManager","Can't switch after socket is created!");

			//Exit
			return FALSE;
		}

		//Is it a legal manager
		if (!pManager ||
			!pManager->IsInitialized())
		{
			//Report it
			ReportError("SetLocalThreadManager","Manager is either null or not initialized!");

			//Exit
			return FALSE;
		}

		//Save the new manager
		m_pLocalThreadManager=pManager;

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("SetLocalThreadManager",FALSE)
}

CSocketThreadManagerImp* CAsyncSocket::CloneThreadManager(unsigned long ulThreads)
{
	try
	{
		//Do we have a thread manager?
		if (!m_pThreadManager ||
			!m_pThreadManager->IsInitialized())
		{
			//Report it
			ReportStaticError(CAsyncSocket_Class,"SetLocalThreadManager","Manager is either null or not initialized!");

			//Exit
			return NULL;
		}

		//Which thread count
		unsigned long ulThreadCount;
		if (!ulThreads)
			ulThreadCount=m_pThreadManager->GetNumberOfThreads();
		else
			ulThreadCount=ulThreads;

		//Clone it
		CSocketThreadManagerImp* pManager;
		pManager=new CSocketThreadManagerImp(m_hInstance);
		if (!pManager->Initialize(ulThreads))
		{
			//Report it
			ReportStaticError(CAsyncSocket_Class,"CloneThreadManager","Failed to initialize the thread manager!");

			//Delete it
			delete pManager;

			//Exit
			return NULL;
		}
		else
			return pManager;
	}
	ERROR_HANDLER_STATIC_RETURN(CAsyncSocket_Class,"CloneThreadManager",NULL)
}

BOOL CAsyncSocket::IsBlockSend()const
{
	return m_bBlockSend;
}

BOOL CAsyncSocket::IsInThread()const
{
	try
	{
		//Do we have the thread?
		if (m_pThread)
			return m_pThread->IsInThread();
		else
			return FALSE;
	}
	ERROR_HANDLER_RETURN("IsInThread",FALSE)
}

BOOL CAsyncSocket::WaitForBlockEvent()const
{
	//Wait for the event
	if (m_pSendEvent->Wait(5000))
		return TRUE;
	else
		return IsClosing();
}

void CAsyncSocket::ResetEvent()
{
	m_pSendEvent->Reset();
}

void CAsyncSocket::DisableBlockingSend()
{
	m_bBlockSend=FALSE;
}

BOOL CAsyncSocket::IsDeleting()const
{
	return m_bDeleting;
}

void CAsyncSocket::SetDeleting()
{
	m_bDeleting=TRUE;
}

KOMODIA_NAMESPACE_END