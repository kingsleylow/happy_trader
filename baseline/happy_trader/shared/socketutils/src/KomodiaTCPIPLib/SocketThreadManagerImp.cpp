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
#include "SocketThreadManagerImp.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"
#include "../KomodiaInfraLib/OSManager.h"
#include "../KomodiaInfraLib/GenericCriticalSection.h"

#include "AsyncSocket.h"

#include <vector>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

//Do we have a window?
BOOL CSocketThreadManagerImp::m_bWindow=FALSE;

//Our static delete
CSocketThreadManagerImp CSocketThreadManagerImp::m_sCleanUp;

//Our instance
HINSTANCE CSocketThreadManagerImp::m_hInstance=0;

#define CSocketThreadManagerImp_Class "CSocketThreadManagerImp"

CSocketThreadManagerImp::CSocketThreadManagerImp() : CSocketThreadManager((HINSTANCE)0),
													 m_bStatic(TRUE),
													 m_hLocalInstance(0)
{
	try
	{
		//Set our name
		SetName(CSocketThreadManagerImp_Class);
	}
	ERROR_HANDLER("CSocketThreadManagerImp")
}

CSocketThreadManagerImp::CSocketThreadManagerImp(HINSTANCE hInstance) : CSocketThreadManager(hInstance),
																		m_hLocalInstance(hInstance),
																		m_bStatic(FALSE)
{
	try
	{
		//Set our name
		SetName(CSocketThreadManagerImp_Class);
	}
	ERROR_HANDLER("CSocketThreadManagerImp")
}

CSocketThreadManagerImp::~CSocketThreadManagerImp()
{
	try
	{
		//Are we static version?
		if (m_bStatic)
			//Remove the window
			if (m_bWindow)
			{
				//Unregister it
				::UnregisterClass(CSocketThreadManagerImp_Class,
								  m_hInstance);

				//No window
				m_bWindow=FALSE;
			}
			else
				;
		else
			//Uninitialize 
			Uninitialize();
	}
	ERROR_HANDLER("~CSocketThreadManagerImp")
}

void CSocketThreadManagerImp::ParseDispatchMessage(const MSG& rMsg,
												   ThreadData* pData)
{
	//Get the extension data
	ExtensionData* pExtension;
	pExtension=(ExtensionData*)pData->aExtension.lpData;

	//Our new msg
	MSG aNewMsg(rMsg);

	//Do we have param?
	if (rMsg.wParam &&
		IsSocketMessage(aNewMsg.message))
	{
		//Lock the CS
		CCriticalAutoRelease aRelease(pExtension->pCSection);

		//Try to get the socket
		SocketMap::iterator aIterator;
		aIterator=pExtension->aSocketsMap.find((int)rMsg.wParam);

		//Do we have it?
		if (aIterator!=pExtension->aSocketsMap.end())
			//Modify the message
			aNewMsg.wParam=(WPARAM)aIterator->second;
		else
			aNewMsg.wParam=0;
	}

	//Dispatch it
	CSocketThreadManager::ParseDispatchMessage(aNewMsg,
											   pData);
}

void CSocketThreadManagerImp::CreateExtendedData(ThreadDataExtend& rData)
{
	try
	{
		//Create the struct
		ExtensionData* pData;
		pData=new ExtensionData;

		//Populate it
		pData->pCSection=COSManager::CreateCriticalSection();
		
		//Put it in the data
		rData.lpData=(LPVOID)pData;
	}
	ERROR_HANDLER("CreateExtendedData")
}

void CSocketThreadManagerImp::DeleteExtendedData(ThreadDataExtend& rData)
{
	try
	{
		//Take the data
		ExtensionData* pData;
		pData=(ExtensionData*)rData.lpData;

		//Is it legal?
		if (pData)
		{
			//Our delete vector
			typedef std::vector<CAsyncSocket*> DeleteVector;

			//Our vector
			DeleteVector aDeleteVector;

			{
				//Lock the CS
				CCriticalAutoRelease aRelease(pData->pCSection);

				//Iterate to delete all the data
				SocketMap::iterator aIterator;
				aIterator=pData->aSocketsMap.begin();
				while (aIterator!=pData->aSocketsMap.end())
				{
					//Add the socket
					aDeleteVector.push_back(aIterator->second);

					//Delete it
					aIterator=pData->aSocketsMap.erase(aIterator);
				}
			}

			//Delete the sockets
			for (int iCount=0;
				 iCount<aDeleteVector.size();
				 ++iCount)
				try
				{
					//Set to delete
					aDeleteVector[iCount]->SetDeleting();

					//Delete the socket
					delete aDeleteVector[iCount];
				}
				ERROR_HANDLER("DeleteExtendedData - Socket delete")

			//Delete the CS
			delete pData->pCSection;

			//Delete it
			delete pData;

			//Remove from data
			rData.lpData=NULL;
		}
	}
	ERROR_HANDLER("DeleteExtendedData")
}

BOOL CSocketThreadManagerImp::Initialize(unsigned long ulThreadCount)
{
	try
	{
		//First create the window class
		if (!m_bWindow)
		{
			//Save the instance
			m_hInstance=m_hLocalInstance;

			if (!RegisterWindow(m_hInstance))
			{
				//Report it
				ReportErrorOS("SetHandlers","Error registering the window, please check API error!");

				//Exit
				return FALSE;
			}
			else
				//Window is OK
				m_bWindow=TRUE;
		}

		//Initialize our father
		return CSocketThreadManager::Initialize(ulThreadCount,
												CSocketThreadManagerImp_Class);
	}
	ERROR_HANDLER_RETURN("Initialize",FALSE)
}

BOOL CSocketThreadManagerImp::RegisterWindow(HINSTANCE hInstance)
{
	try
	{
		//Register a window class
		WNDCLASS wc;
		wc.style = 0;										/* Class style(s).                    */
		wc.lpfnWndProc = (WNDPROC)SocketMessageHandler;		/* Function to retrieve messages for  */
		wc.cbClsExtra = 0;									/* No per-class extra data.           */
		wc.cbWndExtra = 0;									/* No per-window extra data.          */
		wc.hIcon = NULL;									/* Icon name from .RC        */
		wc.hInstance = hInstance;							/* Application that owns the class.   */
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName =  NULL;							/* Name of menu resource in .RC file. */
		wc.lpszClassName = CSocketThreadManagerImp_Class ;	/* Name used in call to CreateWindow. */

		/* Register the window class and return success/failure code. */
		return ::RegisterClass(&wc);
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketThreadManagerImp_Class,"RegisterWindow",FALSE)
}

HWND CSocketThreadManagerImp::GetWindowHandle(CAsyncSocket* pSocket)
{
	try
	{
		//Allocate a handle
		HWND hHandle;
		hHandle=CSocketThreadManager::GetWindowHandle();

		//Do we have it?
		if (hHandle)
			//Add the socket
			AddSocketToList(pSocket,
							hHandle);

		//Done
		return hHandle;
	}
	ERROR_HANDLER_RETURN("GetWindowHandle",NULL)
}

void CSocketThreadManagerImp::AddSocketToList(CAsyncSocket* pSocket,
											  HWND hHandle)
{
	try
	{
		//Find the thread data
		ThreadData* pData;
		pData=GetDataByHWND(hHandle);

		//Do we have it
		if (!pData)
			//Report it
			ReportError("AddSocketToList","Failed to find thread data!");
		else
		{
			//Get our extension
			ExtensionData* pExtension;
			pExtension=(ExtensionData*)pData->aExtension.lpData;

			//Lock the CS
			CCriticalAutoRelease aRelease(pExtension->pCSection);

			//Insert to the map
			pExtension->aSocketsMap.insert(SocketMap::value_type(pSocket->GetAsyncHandle(),
																 pSocket));
		}
	}
	ERROR_HANDLER("AddSocketToList")
}

void CSocketThreadManagerImp::RemoveSocket(CAsyncSocket* pSocket)
{
	try
	{
		//Get the handle
		HWND hHandle;
		hHandle=pSocket->GetWindowHandle();

		//Do we have it
		if (!hHandle)
		{
			//Report it
			ReportError("RemoveSocket","Received socket without a handle!");

			//Exit
			return;
		}

		//Find the thread data
		ThreadData* pData;
		pData=GetDataByHWND(hHandle);

		//Do we have it
		if (!pData)
			//Report it
			ReportError("RemoveSocket","Failed to find thread data!");
		else
		{
			//Get our extension
			ExtensionData* pExtension;
			pExtension=(ExtensionData*)pData->aExtension.lpData;

			{
				//Lock the CS
				CCriticalAutoRelease aRelease(pExtension->pCSection);

				//Remove from the map
				pExtension->aSocketsMap.erase(pSocket->GetAsyncHandle());
			}

			//Decrease socket count
			DecreaseSocketCount(hHandle,
								TRUE);
		}
	}
	ERROR_HANDLER("RemoveSocket")
}

BOOL CSocketThreadManagerImp::IsSocketMessage(unsigned int uiMsg)
{
	return uiMsg==WM_SOCKET_DELETE ||
		   uiMsg==WM_SOCKET_FORCED ||
		   uiMsg==WM_SOCKET_GENERAL ||
		   uiMsg==WM_SOCKET_CONNECT ||
		   uiMsg==WM_SOCKET_ACCEPT ||
		   uiMsg==WM_TIMER;
}

LRESULT CALLBACK CSocketThreadManagerImp::SocketMessageHandler(HWND hwnd,      // handle to window
							  								   UINT uMsg,      // message identifier
															   WPARAM wParam,  // first message parameter
															   LPARAM lParam)   // second message parameter													
{
	try
	{
		//first get the socket
		CAsyncSocket* pSocket;

		//Is it a socket message?
		if (IsSocketMessage(uMsg))
			pSocket=(CAsyncSocket*)(wParam);
		else
			pSocket=NULL;

		//Are we closing ?
		BOOL bIgnore;
		if (pSocket &&
			pSocket->IsClosing() &&
			uMsg!=WM_SOCKET_DELETE)
			bIgnore=TRUE;
		else if (pSocket &&
				 pSocket->m_hLocalWindowHandle!=hwnd)
		{
			//Report it
			ReportStaticError(CAsyncSocket_Class,"SocketMessageHandler","Received loose message!");

			//Ignore this message
			bIgnore=TRUE;
		}
		else
			bIgnore=FALSE;

		//Protect the call
		try
		{
			if (pSocket &&
				!bIgnore)
				//Socket exists
				switch (uMsg)
				{
				case WM_SOCKET_FORCED:
					return pSocket->OnSocketReceive(0);
					break;
				case WM_SOCKET_GENERAL:
					if (WSAGETSELECTEVENT(lParam)==FD_READ)
						return pSocket->SocketReceive(WSAGETSELECTERROR(lParam));
					else if (WSAGETSELECTEVENT(lParam)==FD_WRITE)
						return pSocket->SocketWrite(WSAGETSELECTERROR(lParam));
					else if (WSAGETSELECTEVENT(lParam)==FD_OOB)
						return pSocket->OnSocketOOB(WSAGETSELECTERROR(lParam));
					else if (WSAGETSELECTEVENT(lParam)==FD_CLOSE)
						return pSocket->SocketClosed(WSAGETSELECTERROR(lParam));
					break;
				case WM_SOCKET_CONNECT:
					if (WSAGETSELECTEVENT(lParam)==FD_CONNECT)
						return pSocket->SocketConnected(WSAGETSELECTERROR(lParam));
					break;
				case WM_SOCKET_ACCEPT:
					if (WSAGETSELECTEVENT(lParam)==FD_ACCEPT)
						return pSocket->SocketAccept(WSAGETSELECTERROR(lParam));
					break;
				case WM_TIMER:
					//Do we have a timer ?
					if (pSocket->HasTimeout())
						//Inform the socket
						return pSocket->OnSocketTimeout();
					else
						return FALSE;
				case WM_SOCKET_DELETE:
					//Delete the socket
					pSocket->OnSocketDelete((CGenericEvent*)lParam);

					//Exit
					return TRUE;
				default:                       /* Passes it on if unproccessed    */
					return (int)(DefWindowProc(hwnd, 
											   uMsg, 
											   wParam, 
											   lParam));
				}
			else
				return (int)(DefWindowProc(hwnd, 
										   uMsg, 
										   wParam, 
										   lParam));
		}
		ERROR_HANDLER_STATIC(CSocketThreadManagerImp_Class,"SocketMessageHandler - Proc")

		//Exit
		return TRUE;
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketThreadManagerImp_Class,"SocketMessageHandler",TRUE)
}

KOMODIA_NAMESPACE_END
