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

#if !defined(AFX_SOCKETTHREADMANAGERIMP_H__09027047_1304_48DF_9547_33344AE3D88C__INCLUDED_)
#define AFX_SOCKETTHREADMANAGERIMP_H__09027047_1304_48DF_9547_33344AE3D88C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SocketThreadManager.h"

#include <map>

KOMODIA_NAMESPACE_START

class CAsyncSocket;

class SOCKETUTILS_EXP CSocketThreadManagerImp : public CSocketThreadManager
{
public:
	//Get the most available window handle, and associate a socket with it
	HWND GetWindowHandle(CAsyncSocket* pSocket);

	//Remove socket from list
	void RemoveSocket(CAsyncSocket* pSocket);

	//Initialize the thread manager
	BOOL Initialize(unsigned long ulThreadCount);

	//Ctor and Dtor
	CSocketThreadManagerImp(HINSTANCE hInstance);
	virtual ~CSocketThreadManagerImp();
protected:
	//Dispatch the message
	virtual void ParseDispatchMessage(const MSG& rMsg,
									  ThreadData* pData);

	//Overide to create the extended data
	virtual void CreateExtendedData(ThreadDataExtend& rData);

	//Overide when extended data is deleted
	virtual void DeleteExtendedData(ThreadDataExtend& rData);
private:
	//Our map of sockets
	typedef std::map<int,CAsyncSocket*> SocketMap;

	//Our data extension
	typedef struct _ExtensionData
	{
		CGenericCriticalSection*	pCSection;
		SocketMap					aSocketsMap;
	} ExtensionData;
private:
	//Static Ctor
	CSocketThreadManagerImp();
private:
	//No copy Ctor
	CSocketThreadManagerImp(const CSocketThreadManagerImp& rManager);

	//No assigment opreator
	CSocketThreadManagerImp& operator=(const CSocketThreadManagerImp& rManager);

	//Add socket to the list
	void AddSocketToList(CAsyncSocket* pSocket,
						 HWND hHandle);

	//Register our window
	static BOOL RegisterWindow(HINSTANCE hInstance);

	//Check if it's a socket's message
	static BOOL IsSocketMessage(unsigned int uiMsg);

	//Do we have a window handle
	static BOOL m_bWindow;

	//Our instance
	static HINSTANCE m_hInstance;

	//Our static version
	static CSocketThreadManagerImp m_sCleanUp;

	//Local instance
	HINSTANCE m_hLocalInstance;

	//Are we static
	BOOL m_bStatic;
private:
	//Our window proc
	static LRESULT CALLBACK SocketMessageHandler(HWND hwnd,      // handle to window
				    			  			     UINT uMsg,      // message identifier
										         WPARAM wParam,  // first message parameter
										         LPARAM lParam);
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_SOCKETTHREADMANAGERIMP_H__09027047_1304_48DF_9547_33344AE3D88C__INCLUDED_)
