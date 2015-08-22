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

#if !defined(AFX_ASYNCSOCKET_H__980B05BD_A250_40D0_AD92_4F31AEC90E74__INCLUDED_)
#define AFX_ASYNCSOCKET_H__980B05BD_A250_40D0_AD92_4F31AEC90E74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)

#include "../socketutilsdefs.hpp"
#include "SocketThreadManagerImp.h"

#include "SocketBase.h"
#include "BlockedBuffer.h"

//Message handlers
#define WM_BASE				WM_USER
#define WM_SOCKET_GENERAL	WM_BASE+1
#define WM_SOCKET_ACCEPT	WM_BASE+2
#define WM_SOCKET_CONNECT	WM_BASE+3
#define WM_SOCKET_TIMEOUT	WM_BASE+4
#define WM_SOCKET_DELETE	WM_BASE+5
#define WM_SOCKET_FORCED	WM_BASE+6

//Definitions for no messaging
#define NO_OnSocketTimeout	virtual BOOL OnSocketTimeout() {return TRUE;}
#define NO_OnSocketConnect	virtual BOOL OnSocketConnect(int iErrorCode) {return TRUE;}
#define NO_OnSocketAccept	virtual BOOL OnSocketAccept(int iErrorCode) {return TRUE;}
#define NO_OnSocketClose	virtual BOOL OnSocketClose(int iErrorCode) {return TRUE;}
#define NO_OnSocketOOB		virtual BOOL OnSocketOOB(int iErrorCode) {return TRUE;}
#define NO_OnSocketWrite	virtual BOOL OnSocketWrite(int iErrorCode) {return TRUE;}
#define NO_OnSocketReceive	virtual BOOL OnSocketReceive(int iErrorCode) {return TRUE;}

//Window class name
#define CAsyncSocket_Class "CAsyncSocketClass"
#define CAsyncShutdown_Class "CAsyncShutdown"

KOMODIA_NAMESPACE_START

class CGenericCriticalSection;
class CGenericEvent;

class SOCKETUTILS_EXP CAsyncSocket : virtual protected CSocketBase
{
	friend CBlockedBuffer;
	friend CSocketThreadManagerImp;
public:
	//Events
	typedef enum _AsyncEvents
	{
		aeReceive=1,
		aeSend=2,
		aeOOB=4,
		aeClose=8
	} AsyncEvents;

	//Our map proc
	typedef BOOL (*MapProc)();
	typedef MapProc LPMapProc;
public:
	//Set socket as detached, which means it will have it's own thread
	void DetachedSocketThread();

	//Disable blocking send
	void DisableBlockingSend();

	//Clone a thread manager (only if one is already created)
	//ulThreads - Number of threads to create, if 0 then like in the thread manager
	static CSocketThreadManagerImp* CloneThreadManager(unsigned long ulThreads);

	//Set a local thread manager
	BOOL SetLocalThreadManager(CSocketThreadManagerImp* pManager);

	//Get our thread manager (global or local)
	CSocketThreadManagerImp* GetThreadManager()const;

	//Register a regular timeout, but put it on the thread as this socket
	//If socket is not created the any thread
	//iMS - Timeout interval
	//pProc - Proc to run
	//pData - Data to give the proc
	//bClearTimeout - Autodelete the timeout
	CSocketThreadManager::TimerID RegisterTimeout(int iMS,
												  CSocketThreadManagerImp::LPTimeoutProc pProc,
												  LPVOID pData,
												  BOOL bClearTimeout=FALSE,
												  CSocketThreadManagerImp::TimerID* pTimer=NULL);

	//Do we have a timeout
	BOOL HasTimeout()const;

	//Disable events
	void DisableEvents(unsigned char ucEvents);

	//Are we deleting? (system shutdown)
	BOOL IsDeleting()const;

	//Set we are deleting
	void SetDeleting();

	//Are we closing ?
	BOOL IsClosing()const;

	//Force a receive event
	void ForceReceiveEvent();

	//Delete an async socket in his own thread
	virtual void DeleteSocketFromThread();

	//Delete an async socket in his own thread
	virtual void DeleteSocketFromThread(DWORD dwTimeToWait);

	//Delete an async socket in his own thread (but exists only after socket is deleted)
	virtual void DeleteSocketFromThreadWait();

	//Implement a message map
	static void SimpleMessageMap();

	//Implement a timed message map
	static void SimpleMessageMap(DWORD dwRunTimeMS);

	//Implement a message map
	//Stop it when function has a result
	//pProc - The proc to run
	//bStopWhenTRUE - Stop when the proc is TRUE
	//dwSleep - How much to sleep between idle calls
	static void SimpleMessageMap(LPMapProc pProc,
								 BOOL bStopWhenTRUE,
								 DWORD dwSleep);

	//Message map that exists when a key is pressed
	static char SimpleMessageMapKey(DWORD dwSleep);

	//Enable/disable blocked buffer 
	//(for when sends has to wait)
	void AllowBlockedBuffer(BOOL bAllow);

	//Go back to async mode
	virtual BOOL ReAsync();

	//Transfer the socket back to blocking mode!
	virtual BOOL ReBlock();

	//Notify that don't select this thread 
	//after socket destruction
	void FreezeThread();

	//Disable the async notification
	BOOL DisableAsync();

	//Initialize all the handlers
	//Can be called by user, but will be invoked automatically
	static void Initialize();

	//Indicate a system shutdown
	//Can be called by user, but will be invoked automatically
	static void Shutdown();

	//Disable the timeout
	BOOL KillTimer();

	//Create a timeout
	//Another call to SetTimeout will reset the old timer
	//and create a new one
	BOOL SetTimeout(int iMs);

	//Set the instace of our app
	//Required for Win9X
	static void SetInstance(HINSTANCE hInst);

	//Ctor and Dtor
	CAsyncSocket();
	virtual ~CAsyncSocket();
protected:
	//Messaging methods

	//Error codes are
	//SOCKET_ERROR	- Internal library error
	//0				- Success
	//Error code	- System error code
	//				  You can use ErrorCodeToString in CErrorHandler
	//				  to convert

	//When a timeout occured (implemented at library level)
	//User should return 0, if proccessed the message
	virtual BOOL OnSocketTimeout()=0;

	//When a socket got a connect event (after calling connect)
	virtual BOOL OnSocketConnect(int iErrorCode)=0;

	//When a socket can accept an incoming connection
	virtual BOOL OnSocketAccept(int iErrorCode)=0;

	//When a socket was closed
	virtual BOOL OnSocketClose(int iErrorCode)=0;

	//When an OOB data came (however uses this feature
	//shouldn't be doing it)
	virtual BOOL OnSocketOOB(int iErrorCode)=0;

	//When a send failed because it would block
	//this event signals the send can be reattempted
	virtual BOOL OnSocketWrite(int iErrorCode)=0;

	//When socket has data in queue
	virtual BOOL OnSocketReceive(int iErrorCode)=0;

	//Are we in blocking mode
	BOOL IsBlocking()const;

	//Save the last blocking status
	int InternalWSAAsyncSelect(unsigned int wMsg,
							   long lEvent);

	//Close the timeout if needed
	void SocketClosing();

	//Get the ID of the socket
	int GetSocketID()const;

	//Get the handle of the window
	HWND GetWindowHandle()const;

	//Get the socket handle
	virtual SOCKET GetAsyncHandle()const=0;

	//Go to async regular mode
	virtual BOOL SetAsync()=0;

	//Remove the socket from the list
	void RemoveSocketFromList();

	//Add the socket to the list
	void AddSocketToList();

	//Do we have a timeout
	BOOL IsTimeout()const;

	//Just block
	BOOL Block();

	//Send data from the send buffer
	virtual BOOL SendBlockedBuffer(const CBlockedBuffer::CBlockedData& rData);

	//Get the blocked buffer
	CBlockedBuffer* GetBlockedBuffer()const;

	//Get the socket thread manager
	static CSocketThreadManagerImp* GetSocketThreadManager();

	//Socket is connected
	//Not for user to override
	virtual BOOL SocketConnected(int iErrorCode,
								 BOOL bNoEvent=FALSE);

	//Socket it closed
	//Not for user to override
	virtual BOOL SocketClosed(int iErrorCode,
							  BOOL bNoEvent=FALSE);

	//When we have data to write
	//Not for user to override
	virtual BOOL SocketWrite(int iErrorCode);

	//When we have data to receive
	//Not for user to override
	virtual BOOL SocketReceive(int iErrorCode,
							   BOOL bNoEvent=FALSE);

	//When we want to accept
	//Not for user to override
	virtual BOOL SocketAccept(int iErrorCode,
							  BOOL bNoEvent=FALSE);

	//When we have a system timeout
	//Not for user to override
	virtual void SystemTimeout();

	//Set a system timer
	BOOL SetSystemTimeout(int iMS);

	//Kill the system timer
	BOOL KillSystemTimer();

	//Do we have a system timer
	BOOL HasSystemTimer()const;

	//Delete an async socket in his own thread
	//Not for user to override
	virtual void OnSocketDelete(CGenericEvent* pEvent=NULL);

	//We have a new socket
	//Not for user to override
	virtual void SocketCreated();

	//Clear the events
	virtual BOOL ClearEvents();

	//Do we block on send
	BOOL IsBlockSend()const;

	//Are we in the same messaging thread?
	BOOL IsInThread()const;

	//Wait for block event (TRUE for timeout)
	BOOL WaitForBlockEvent()const;

	//Reset the event
	void ResetEvent();
private:
	//No copy Ctor
	CAsyncSocket(const CAsyncSocket& rSocket);

	//No assignment operator
	CAsyncSocket& operator=(const CAsyncSocket& rSocket);

	//Check if the socket is valid
	BOOL CheckAsyncSocketValid()const;

	//Remove from thread info
	void DeAllocateHandle();

	//Allocate ourself a window
	void AllocateHandle();

	//Get the instance of our APP
	static HINSTANCE GetInstance();

	//Create our handlers
	static BOOL SetHandlers();
	
	//Our proc
	static void SystemTimerProc(LPVOID lpData);

	//Our delete proc
	static void DeleteTimerProc(LPVOID lpData);

	//Instance of our window
	static HINSTANCE m_hInstance;

	//Are we initialized
	static BOOL m_bInitialized;

	//ID of our socket
	int m_iSocketID;

	//Are we in the list
	BOOL m_bList;

	//Timeout indicator
	BOOL m_bTimeout;

	//Our window's handle
	HWND m_hLocalWindowHandle;

	//Are we shutting down
	static BOOL m_bShuttingDown;

	//Should we freeze this thread ?
	BOOL m_bFreeze;

	//Are we blocking
	BOOL m_bBlocking;

	//The last async message setting
	unsigned int m_iMsg;

	//And the event
	long m_lEvent;

	//Our thread manager (global)
	static CSocketThreadManagerImp* m_pThreadManager;

	//Our local thread manager (to allow custom thread mangement)
	CSocketThreadManagerImp* m_pLocalThreadManager;

	//Our blocked buffer
	CBlockedBuffer* m_pBlockedBuffer;

	//Our timer
	CSocketThreadManagerImp::TimerID m_aTimerID;

	//Our delete timer
	CSocketThreadManagerImp::TimerID m_aDeleteTimerID;

	//Are we gonna close
	BOOL m_bClosing;

	//Events to disable
	unsigned char m_ucEvents;

	//Our event for the send
	CGenericEvent* m_pSendEvent;
	
	//Do we block on sends?
	BOOL m_bBlockSend;

	//Our thread we live in
	const CGenericThread* m_pThread;

	//Are we deleting
	BOOL m_bDeleting;

	//Are we a detached socket
	BOOL m_bDetached;
private:
	//Our shutdown class (all of this to avoid father to know his sons)
	class CAsyncShutdown : protected CSocketBase
	{
	public:
		CAsyncShutdown();
		virtual ~CAsyncShutdown();
	protected:
		//Shutdown notifier
		virtual void NotifyShutdown();
	};
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_ASYNCSOCKET_H__980B05BD_A250_40D0_AD92_4F31AEC90E74__INCLUDED_)
