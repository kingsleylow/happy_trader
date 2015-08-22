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

#if !defined(AFX_KOMODIATHREADPOOL_H__D3F7BADC_92DA_4C70_A2BF_090924E69D81__INCLUDED_)
#define AFX_KOMODIATHREADPOOL_H__D3F7BADC_92DA_4C70_A2BF_090924E69D81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include "ErrorHandler.h"
#include "GenericThread.h"

#include <deque>

KOMODIA_NAMESPACE_START

class CGenericSemaphore;
class CGenericCriticalSection;
class CGenericEvent;

//Our max thread count
#define MAX_POOL_THREADS 200

//Our default jobs
#define THREAD_POOL_DEFAULT_JOBS 20

class CThreadPool : public CErrorHandler
{
public:
	//Proc we are expecting
	typedef void (*ThreadPoolProc)(LPVOID pParam);
	typedef ThreadPoolProc LPThreadPoolProc;

	//Another type we can receive
	typedef DWORD (*ThreadDWORDPoolProc)(LPVOID pParam);
	typedef ThreadDWORDPoolProc LPThreadDWORDPoolProc;

	//Proc with data (passed from the initialize)
	typedef void (*ThreadPoolDataProc)(LPVOID pParam,
									   LPVOID pThreadData);
	typedef ThreadPoolDataProc LPThreadPoolDataProc;

	//A proc to initialize the thread pool
	typedef BOOL (*InitializePoolProc)(LPVOID& pParam,
									   BOOL bLoading);
	typedef InitializePoolProc LPInitializePoolProc;
public:
	//Get the total number of jobs that ran
	int GetJobsRan()const;

	//Clear the data
	void Clear();

	//Do we allow extra data to block, or to be dropped ?
	void SetExtraDataDrop(BOOL bDrop);

	//Set the sleep interval
	void SetSleepInterval(DWORD dwSleepInterval);

	//Is the pool initialized ?
	BOOL IsInitialized()const;

	//Get the number of running threads
	int GetRunningThreads()const;

	//Get the number of threads
	DWORD GetMaxThreads()const;

	//Did we finish running
	BOOL IsFinished()const;

	//How many jobs we have
	int GetWaitingJobs()const;

	//Add a job
	BOOL SubmitJob(LPThreadPoolProc pJobProc,
				   LPVOID lpData);
	BOOL SubmitJob(LPThreadDWORDPoolProc pJobProc,
				   LPVOID lpData);
	BOOL SubmitJob(LPThreadPoolDataProc pJobProc,
				   LPVOID lpData);

	//Ctor and Dtor
	//ulNumberOfThreads - Number of active threads
	//ulMaxJobsPending - Size of the back queue
	//pInitializeProc - Initialize proc
	//aPriority - Threads priority
	CThreadPool(unsigned long ulNumberOfThreads,
				unsigned long ulMaxJobsPending=THREAD_POOL_DEFAULT_JOBS,
				LPInitializePoolProc pInitializeProc=NULL,
				CGenericThread::ThreadPriority aPriority=CGenericThread::tpNormal,
				BOOL bInitializeCOM=FALSE);
	virtual ~CThreadPool();
private:
	//Data for the job
	typedef struct _JobData
	{
		LPThreadPoolProc		pProc;
		LPThreadDWORDPoolProc	pDWORDProc;
		LPThreadPoolDataProc	pDataProc;
		LPVOID					pParam;
	} JobData;

	//Our job queue
	typedef std::deque<JobData> JobVector;

	//Thread data
	typedef struct _ThreadData
	{
		CGenericEvent*			pEvent;
		CGenericEvent*			pExitEvent;
		CThreadPool*			pClass;
		LPInitializePoolProc	pInitializeProc;
		BOOL					bCOM;
	} ThreadData;
private:
	//No copy Ctor
	CThreadPool(const CThreadPool& rPool);

	//No assignment operator
	CThreadPool& operator=(const CThreadPool& rPool);

	//Submit the job localy
	BOOL SubmitJob(LPThreadPoolProc pJobProc,
				   LPThreadDWORDPoolProc pDWORDJobProc,
				   LPThreadPoolDataProc pDataProc,
				   LPVOID lpData);

	//We have another or less thread running
	void ReSetThreadCount(int iIncrement);

	//delete all the threads
	void ClearThreads();

	//Get a job
	void GetJob(JobData& aData);

	//Get the semaphore
	CGenericSemaphore* GetSemaphore()const;

	//Create the threads
	BOOL SpawnThreads(LPInitializePoolProc pInitializeProc,
					  CGenericThread::ThreadPriority aPriority,
					  BOOL bCOM);

	//Our thread proc
	static DWORD PoolThread(LPVOID pParam);

	//Number of thread
	unsigned long m_ulThreadCount;

	//Number of jobs
	unsigned long m_ulJobsCount;

	//Our CS
	CGenericCriticalSection* m_pCSection;
	CGenericCriticalSection* m_pCSectionCounter;

	//Our array of threads
	CGenericThread** m_ppThreads;

	//Our semaphore
	CGenericSemaphore* m_pSemaphore;

	//Our vector
	JobVector m_aJobList;

	//Thread data
	ThreadData** m_ppThreadData;

	//Our running thread count
	int m_iRunningThreads;

	//Max jobs pending
	int m_iMaxJobsPending;

	//Is the pool initialized
	BOOL m_bInitialized;
	
	//Sleep interval for the semaphore
	DWORD m_dwSleepInterval;

	//Do we drop extra data ?
	BOOL m_bDrop;

	//Total number of jobs ran so far
	int m_iTotalJobsRan;

	//Did we initialize COM?
	BOOL m_bInitializeCOM;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_KOMODIATHREADPOOL_H__D3F7BADC_92DA_4C70_A2BF_090924E69D81__INCLUDED_)
