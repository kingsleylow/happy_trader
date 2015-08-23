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
#include "ThreadPool.h"

#include "ErrorHandlerMacros.h"
#include "GenericSemaphore.h"
#include "GenericCriticalSection.h"
#include "GenericEvent.h"
#include "WaitList.h"
#include "OSManager.h"

#include <objbase.h>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CThreadPool_Class "CThreadPool"

//Timeout for threads
#define THREAD_TIMEOUT 10000

CThreadPool::CThreadPool(unsigned long ulNumberOfThreads,
						 unsigned long ulMaxJobsPending,
						 LPInitializePoolProc pInitializeProc,
						 CGenericThread::ThreadPriority aPriority,
						 BOOL bInitializeCOM) : CErrorHandler(),
												m_ulThreadCount(ulNumberOfThreads),
												m_pSemaphore(NULL),
												m_ppThreads(NULL),
												m_pCSection(NULL),
												m_ulJobsCount(0),
												m_ppThreadData(NULL),
												m_iMaxJobsPending(ulMaxJobsPending),
												m_pCSectionCounter(NULL),
												m_iRunningThreads(0),
												m_bInitialized(FALSE),
												m_dwSleepInterval(1),
												m_bDrop(FALSE),
												m_iTotalJobsRan(0),
												m_bInitializeCOM(bInitializeCOM)
{
	try
	{
		//Set our name
		SetName(CThreadPool_Class);

		//Check number of threads are legal
		if (!ulNumberOfThreads)
		{
			//Report it
			ReportError("CThreadPool","Received zero threads!");

			//Throw and error
			throw std::string("Received zero threads!");
		}

		//Create the semaphore
		m_pSemaphore=COSManager::CreateSemaphore(0,
												 m_iMaxJobsPending);

		//Create the CS
		m_pCSection=COSManager::CreateCriticalSection();
		m_pCSectionCounter=COSManager::CreateCriticalSection();

		//Initialize the threads
		m_bInitialized=SpawnThreads(pInitializeProc,
									aPriority,
									m_bInitializeCOM);
	}
	ERROR_HANDLER_RETHROW("CThreadPool")
}

CThreadPool::~CThreadPool()
{
	try
	{
		//Clear the threads
		ClearThreads();

		//Delete the semaphore
		delete m_pSemaphore;
		
		//Delete the CS
		delete m_pCSection;
		delete m_pCSectionCounter;
	}
	ERROR_HANDLER("~CThreadPool")
}

BOOL CThreadPool::SpawnThreads(LPInitializePoolProc pInitializeProc,
							   CGenericThread::ThreadPriority aPriority,
							   BOOL bCOM)
{
	try
	{
		//Create the thread list
		m_ppThreads=new CGenericThread*[m_ulThreadCount];

		//Create the thread data
		m_ppThreadData=new ThreadData*[m_ulThreadCount];

		//Clear the data
		memset(m_ppThreads,
			   0,
			   sizeof(CGenericThread*)*m_ulThreadCount);
		memset(m_ppThreadData,
			   0,
			   sizeof(ThreadData*)*m_ulThreadCount);

		//Our waiting list
		CWaitList aWaitingList;

		//Create each thread
		for (int iCount=0;
			 iCount<m_ulThreadCount;
			 ++iCount)
		{
			m_ppThreadData[iCount]=new ThreadData;

			//Create the event
			m_ppThreadData[iCount]->pEvent=COSManager::CreateEvent();
			m_ppThreadData[iCount]->pExitEvent=COSManager::CreateEvent();
			m_ppThreadData[iCount]->bCOM=bCOM;

			//Add it to the waiting list
			aWaitingList.AddObject(m_ppThreadData[iCount]->pEvent,TRUE);

			//Set our class
			m_ppThreadData[iCount]->pClass=this;

			//The initialize proc
			m_ppThreadData[iCount]->pInitializeProc=pInitializeProc;

			//Create the thread
			m_ppThreads[iCount]=COSManager::CreateThread(PoolThread);

			//Set the priority
			m_ppThreads[iCount]->SetPriority(aPriority);

			//Start it
			m_ppThreads[iCount]->Start((LPVOID)m_ppThreadData[iCount]);
		}

		//Did we have an error
		BOOL bError;
		bError=FALSE;

		//The timeout
		int iTimeout;

		//Which timeout
		if (m_ulThreadCount>20)
			iTimeout=THREAD_TIMEOUT+(m_ulThreadCount-20)*100;
		else
			iTimeout=THREAD_TIMEOUT;

		//Wait for all the handles to finish
		DWORD dwTmp;
		if (aWaitingList.Wait(TRUE,
							  dwTmp,
							  THREAD_TIMEOUT))
		{
			//Report the error
			ReportError("SpawnThreads","Timeout waiting for threads!");

			//Indicate it
			bError=TRUE;
		}

		//Done
		return !bError;
	}
	ERROR_HANDLER_RETURN("SpawnThreads",FALSE)
}

BOOL CThreadPool::SubmitJob(LPThreadPoolProc pJobProc, 
							LPVOID lpData)
{
	try
	{
		return SubmitJob(pJobProc,
						 NULL,
						 NULL,
						 lpData);
	}
	ERROR_HANDLER_RETURN("SubmitJob",FALSE)
}

BOOL CThreadPool::SubmitJob(LPThreadDWORDPoolProc pJobProc,
							LPVOID lpData)
{
	try
	{
		return SubmitJob(NULL,
						 pJobProc,
						 NULL,
						 lpData);
	}
	ERROR_HANDLER_RETURN("SubmitJob",FALSE)
}

BOOL CThreadPool::SubmitJob(LPThreadPoolDataProc pJobProc,
							LPVOID lpData)
{
	try
	{
		return SubmitJob(NULL,
						 NULL,
						 pJobProc,
						 lpData);
	}
	ERROR_HANDLER_RETURN("SubmitJob",FALSE)
}

BOOL CThreadPool::SubmitJob(LPThreadPoolProc pJobProc,
						    LPThreadDWORDPoolProc pDWORDJobProc,
							LPThreadPoolDataProc pDataProc,
						    LPVOID lpData)
{
	try
	{
		//Did we release
		BOOL bRelease;
		bRelease=FALSE;

		//Job data, not to do inside CS
		JobData aData;
		aData.pProc=pJobProc;
		aData.pDWORDProc=pDWORDJobProc;
		aData.pDataProc=pDataProc;
		aData.pParam=lpData;

		//Lock it
		CCriticalAutoRelease aRelease(m_pCSection);

		//Are we full ?
		if (m_ulJobsCount>=m_iMaxJobsPending &&
			m_bDrop)
			//Quit
			return FALSE;

		//Add it
		m_aJobList.push_back(aData);

		//Increase the number of jobs we have
		++m_ulJobsCount;

		//Exit the CS
		aRelease.Exit();

		//And release a semaphore (if we have a big back log it will stick it)
		while (m_pSemaphore->Release()==-1)
			Sleep(m_dwSleepInterval);

		//Everything is OK
		return TRUE;
	}
	ERROR_HANDLER_RETURN("SubmitJob",FALSE)
}

DWORD CThreadPool::PoolThread(LPVOID pParam)
{
	try
	{
		//This is the thread class
		ThreadData* pData;
		pData=(ThreadData*)pParam;

		//Initialize the random seed
		srand(GetTickCount()+((unsigned long)pData->pEvent)*2);

		//Error for initialize proc
		BOOL bError;
		bError=FALSE;

		//Our data
		LPVOID lpData;
		lpData=NULL;

		//Call the initialize proc (if exists)
		if (pData->pInitializeProc)
		{
			try
			{
				bError=!(*pData->pInitializeProc)(lpData,TRUE);
			}
			catch (...)
			{
				bError=TRUE;
			}
		}

		//Do we need to initialize COM
		if (pData->bCOM)
		{
			//Initialize it
			HRESULT hr;
			if (FAILED(hr=CoInitialize(NULL)))
			{
				//Report it
				ReportStaticError(CThreadPool_Class,"PoolThread","Failed to initialize COM!",hr);

				//Set the error
				bError=TRUE;
			}
		}

		//Set the event
		if (pData->pEvent)
			if (!bError)
				pData->pEvent->Set();
			else
			{
				//Report it
				ReportStaticError(CThreadPool_Class,"PoolThread","Initialize proc failed!");

				//Exit
				return FALSE;
			}
		else
		{
			//Report it
			ReportStaticError(CThreadPool_Class,"PoolThread","Received null event!");

			//Exit
			return FALSE;
		}

		//Our wait list
		CWaitList aList;
		aList.AddObject(pData->pClass->GetSemaphore(),
						TRUE);
		aList.AddObject(pData->pExitEvent);

		//Endless loop
		while (1)
		{
			DWORD dwObject;

			//Wait for the events
			aList.Wait(FALSE,
					   dwObject);

			//What do we have
			if (dwObject)
				break;

			//Increase the count
			pData->pClass->ReSetThreadCount(1);

			//Take a job
			JobData aJobData;
			pData->pClass->GetJob(aJobData);

			//Need to protect running thread
			try
			{
				//Check it's a valid proc
				if (aJobData.pProc)
					//Run it
					(*aJobData.pProc)(aJobData.pParam);
				else if (aJobData.pDWORDProc)
					//Run it
					(*aJobData.pDWORDProc)(aJobData.pParam);
				else if (aJobData.pDataProc)
					(*aJobData.pDataProc)(aJobData.pParam,lpData);
			}
			ERROR_HANDLER_STATIC(CThreadPool_Class,"PoolThread - Proc")

			//Release the count
			pData->pClass->ReSetThreadCount(-1);
		}
		
		if (pData->bCOM)
			//Destroy COM
			CoUninitialize();

		//Call the unload
		if (pData->pInitializeProc)
		{
			try
			{
				bError=!(*pData->pInitializeProc)(lpData,FALSE);
			}
			catch (...)
			{
				bError=TRUE;
			}
		}

		//We are OK to exit
		pData->pEvent->Set();

		//Done
		return TRUE;
	}
	ERROR_HANDLER_STATIC_RETURN(CThreadPool_Class,"PoolThread",FALSE)
}

CGenericSemaphore* CThreadPool::GetSemaphore() const
{
	return m_pSemaphore;
}

void CThreadPool::GetJob(JobData& aData)
{
	try
	{
		//Lock the CS
		CCriticalAutoRelease aRelease(m_pCSection);

		//Check we can have if
		if (!m_ulJobsCount)
		{
			//Can exists the CS
			aRelease.Exit();

			//Set all to null
			aData.pProc=NULL;
			aData.pDWORDProc=NULL;
			aData.pDataProc=NULL;
			aData.pParam=NULL;

			//And return
			return;
		}

		//Get the data
		aData=m_aJobList.front();

		//We can remove it now
		m_aJobList.pop_front();

		//Decrease the job count
		--m_ulJobsCount;
	}
	ERROR_HANDLER("GetJob")
}

void CThreadPool::ClearThreads()
{
	try
	{
		//Do we have brute delete
		if (m_bInitialized)
		{
			//The timeout
			int iTimeout;

			//Which timeout
			if (m_ulThreadCount>20)
				iTimeout=THREAD_TIMEOUT+(m_ulThreadCount-20)*100;
			else
				iTimeout=THREAD_TIMEOUT;

			//Our waiting list
			CWaitList aList;

			//Wait for all the threads to exit
			for (int iCount=0;
				 iCount<m_ulThreadCount;
				 iCount++)
			{
				//Add the exit event
				aList.AddObject(m_ppThreadData[iCount]->pEvent,TRUE);

				//Signal the exit
				m_ppThreadData[iCount]->pExitEvent->Set();
			}
			
			//Wait for them
			DWORD dwTmp;
			if (aList.Wait(TRUE,
						   dwTmp,
						   iTimeout))
				//Report the error
				ReportError("ClearThreads","Timeout waiting for threads!");
		}

		//Clear all
		for (int iCount=0;
			 iCount<m_ulThreadCount;
			 iCount++)
		{
			//Delete the thread
			delete m_ppThreads[iCount];

			//Delete the events
			delete m_ppThreadData[iCount]->pEvent;
			delete m_ppThreadData[iCount]->pExitEvent;

			//Delete the data
			delete m_ppThreadData[iCount];
		}

		//Delete the array
		delete [] m_ppThreads;
		delete [] m_ppThreadData;

		//Clear them
		m_ppThreads=NULL;
		m_ppThreadData=NULL;
	}
	ERROR_HANDLER("ClearThreads")
}

int CThreadPool::GetWaitingJobs()const
{
	try
	{
		//Lock the CS
		CCriticalAutoRelease aRelease(m_pCSection);

		//Save the number of jobs
		int iJobs;
		iJobs=m_ulJobsCount;

		//And return it
		return iJobs;
	}
	ERROR_HANDLER_RETURN("GetWaitingJobs",0)
}

void CThreadPool::ReSetThreadCount(int iIncrement)
{
	try
	{
		//Lock the counter
		CCriticalAutoRelease aRelease(m_pCSectionCounter);

		//Set the count
		m_iRunningThreads+=iIncrement;

		//Is it negative?
		if (iIncrement<0)
			m_iTotalJobsRan-=iIncrement;
	}
	ERROR_HANDLER("ReSetThreadCount")
}

BOOL CThreadPool::IsFinished()const
{
	return !m_ulJobsCount && 
		   !m_iRunningThreads;
}

DWORD CThreadPool::GetMaxThreads()const
{
	return m_ulThreadCount;
}

BOOL CThreadPool::IsInitialized()const
{
	return m_bInitialized;
}

void CThreadPool::SetSleepInterval(DWORD dwSleepInterval)
{
	m_dwSleepInterval=dwSleepInterval;
}

void CThreadPool::SetExtraDataDrop(BOOL bDrop)
{
	m_bDrop=bDrop;
}

int CThreadPool::GetRunningThreads()const
{
	return m_iRunningThreads;
}

void CThreadPool::Clear()
{
	try
	{
		//Lock the CS
		CCriticalAutoRelease aRelease(m_pCSection);

		//Check we can have if
		if (!m_ulJobsCount)
			return;

		//Delete the data
		m_aJobList.clear();
		m_ulJobsCount=0;
	}
	ERROR_HANDLER("Clear")
}

int CThreadPool::GetJobsRan()const
{
	return m_iTotalJobsRan;
}

KOMODIA_NAMESPACE_END