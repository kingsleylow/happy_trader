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

#if !defined(AFX_SOCKETTHREADMANAGER_H__74FE76EA_CB87_4F58_8F81_AA23A971A332__INCLUDED_)
#define AFX_SOCKETTHREADMANAGER_H__74FE76EA_CB87_4F58_8F81_AA23A971A332__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include "../socketutilsdefs.hpp"
#include "../KomodiaInfraLib/ErrorHandler.h"

#include <map>

//Our socket window class
#define SOCKET_WINDOW_NAME "Socket notification sink"

//Reasonable amount of threads
#define MAX_THREADS 200

KOMODIA_NAMESPACE_START

class CGenericCriticalSection;
class CGenericThread;
class CGenericEvent;

class SOCKETUTILS_EXP CSocketThreadManager : public CErrorHandler  
{
public:
	//Our running procs
	typedef void (*TimeoutProc)(LPVOID pParam);
	typedef TimeoutProc LPTimeoutProc;

	//Time struct to keep track of timeouts
	typedef struct SOCKETUTILS_EXP _TimerID
	{
		int		iTimerID;
		int		iIndex;
		int		iMS;

		//Default clearing Ctor
		_TimerID() : iTimerID(0),
					 iIndex(0),
					 iMS(0)
		{
		}
	} TimerID;
public:
	//Are we initialized
	BOOL IsInitialized()const;

	//Get the number of threads?
	unsigned long GetNumberOfThreads()const;

	//Get the name of the allocated class
	const std::string& GetClassName()const;

	//Refresh an existing timeout
	BOOL ReSetTimeout(const TimerID& rTimerID);

	//Remove a timer
	//aTimerID - The timer to be deleted
	//bInvokeDeleteProc - To invoke the delete proc - if exists?
	BOOL RemoveTimeout(TimerID& aTimerID,
					   BOOL bInvokeDeleteProc=FALSE);

	//Register a timeout
	//iMS - Timeout interval
	//pProc - Proc to run
	//pData - Data to give the proc
	//bClearTimeout - Autodelete the timeout
	//pDeleteProc - Optional proc to delete the parameters incase the timeout was not activated
	//				If timeout was activated user has to delete the data himself!
	TimerID RegisterTimeout(int iMS,
							LPTimeoutProc pProc,
							LPVOID pData,
							BOOL bClearTimeout=FALSE,
							HWND hHandle=NULL,
							TimerID* pTimer=NULL,
							LPTimeoutProc pDeleteProc=NULL);

	//Less socket in the system
	void DecreaseSocketCount(HWND hWindowHandle,
							 BOOL bFreeze=FALSE);

	//Less socket in the system
	void DecreaseTimeoutCount(HWND hWindowHandle,
							  BOOL bFreeze=FALSE);

	//Get the most available window handle
	HWND GetWindowHandle(BOOL bTimeout=FALSE);

	//Get the thread (according to the window handle, for IsInThread lookups)
	const CGenericThread* GetThreadByHWND(HWND hWindowHandle)const;

	//Initialize the thread manager
	BOOL Initialize(unsigned long ulThreadCount,
					LPCSTR lpClassName);

	//Uninitialize the thread manager (will be called by destructor anyway)
	BOOL Uninitialize();

	//Ctor and Dtor
	CSocketThreadManager(HINSTANCE hInstance);
	virtual ~CSocketThreadManager();
protected:
	//Way to extend data
	typedef struct _ThreadDataExtend
	{
		LPVOID						lpData;
		_ThreadDataExtend*			pNext;

		//Ctor
		_ThreadDataExtend() : lpData(NULL),
							  pNext(NULL)
		{
		}
	} ThreadDataExtend;

	//Timeout data
	typedef struct _TimeoutData
	{
		LPTimeoutProc	pTimeoutProc;
		LPVOID			pData;
		BOOL			bClearTimeout;
		TimerID*		pTimer;
		LPTimeoutProc	pDeleteProc;
	} TimeoutData;

	//Maps by IP
	typedef std::map<unsigned int,TimeoutData> TOMap;

	//Our thread data
	typedef struct _ThreadData
	{
		HWND						hWindowHandle;
		int							iSocketCount;
		int							iTimeoutCount;
		BOOL						bFreeze;
		CGenericThread*				pThread;
		HINSTANCE					hInstance;
		CGenericEvent*				pEvent;
		TOMap						pMap;
		std::string					sClassName;
		CGenericCriticalSection*	pCSection;
		CSocketThreadManager*		pClass;
		ThreadDataExtend			aExtension;
	} ThreadData;
protected:
	//Dispatch the message
	virtual void ParseDispatchMessage(const MSG& rMsg,
									  ThreadData* pData);

	//Overide to create the extended data
	virtual void CreateExtendedData(ThreadDataExtend& rData);

	//Overide when extended data is deleted
	virtual void DeleteExtendedData(ThreadDataExtend& rData);

	//Get data by window handle
	ThreadData* GetDataByHWND(HWND hHandle)const;
private:
	//Static Ctor
	CSocketThreadManager(BOOL bStatic);
private:
	//No copy Ctor
	CSocketThreadManager(const CSocketThreadManager& rManager);

	//No assignment operator
	CSocketThreadManager& operator=(const CSocketThreadManager& rManager);

	//Register the wnd class
	BOOL RegisterClass(LPCSTR lpClassName);

	//Unregister the wnd class
	BOOL UnregisterClass();

	//Delete the thread structure
	void CleanThreads(BOOL bError);

	//Get the socket array position by the window handle
	int GetIndexByHWND(HWND hHandle)const;
	
	//Get the freeiest thread
	int GetMostAvailableThread()const;

	//Our thread function
	static DWORD SocketThread(LPVOID lpParameter);

	//Spawn the threads
	BOOL SpawnThreads(LPCSTR lpClassName);

	//Our thread count
	unsigned long m_ulThreadCount;

	//Our windows struct
	ThreadData* m_pThreadData;

	//Our instance
	HINSTANCE m_hInstance;

	//Our critical section
	CGenericCriticalSection* m_pCSection;

	//Have we registered our window ?
	static BOOL m_bRegisteredWindow;

	//Our static instance to delete the window
	static CSocketThreadManager m_sManager;
	
	//Are we a static class
	BOOL m_bStaticClass;

	//Our class name
	std::string m_sClassName;
	
	//Are we initialized
	BOOL m_bInitialized;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_SOCKETTHREADMANAGER_H__74FE76EA_CB87_4F58_8F81_AA23A971A332__INCLUDED_)
