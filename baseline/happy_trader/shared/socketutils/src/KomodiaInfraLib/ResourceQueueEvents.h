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

#ifndef _ResourceQueueEvents_H_
#define _ResourceQueueEvents_H_

#include "ResourceQueue.h"
#include "ThreadPool.h"

KOMODIA_NAMESPACE_START

template<class T>
class CResourceQueueEvents : public CResourceQueue<T>
{
public:
	//Our class
	typedef CResourceQueueEvents<T> ResourceClass;

	//The member to submit the data
	typedef void (*ResourceProc)(ResourceClass* pResourcePool,
								 LPVOID lpData);
	typedef ResourceProc LPResourceProc;
public:
	//Request a resource
	virtual BOOL RequestResource(T& rResource,
								 unsigned long ulTimeout=INFINITE);

	//Ctor and Dtor
	CResourceQueueEvents(LPResourceProc pSpawnProc,
						 LPVOID lpData,
						 unsigned long ulMaxItems=0,
						 unsigned long ulThreads=5,
						 CResourceQueue<T>::LPDeleteProc pCleanUPProc=NULL);
	virtual ~CResourceQueueEvents();
private:
	//No copy Ctor
	CResourceQueueEvents(const CResourceQueueEvents& rEvents);

	//No copy operator
	CResourceQueueEvents& operator=(const CResourceQueueEvents& rEvents);

	//Add a request
	void AddRequest();

	//Our submit thread
	static void SubmitThread(LPVOID lpData);

	//Our spawn proc
	LPResourceProc m_pSpawnProc;

	//Our user data
	LPVOID m_lpUserData;

	//Our thread pool
	CThreadPool* m_pPool;
};

#define CResourceQueueEvents_Class "CResourceQueueEvents"

template<class T>
CResourceQueueEvents<T>::CResourceQueueEvents(LPResourceProc pSpawnProc,
											  LPVOID lpData,
											  unsigned long ulMaxItems,
											  unsigned long ulThreads,
											  CResourceQueue<T>::LPDeleteProc pCleanUPProc) : CResourceQueue<T>(pCleanUPProc,
																												ulMaxItems),
																							  m_pSpawnProc(pSpawnProc),
																							  m_lpUserData(lpData),
																							  m_pPool(NULL)
{
	try
	{
		//Set our name
		SetName(CResourceQueueEvents_Class);

		//Do we have the spawn proc
		if (!m_pSpawnProc)
		{
			//Report it
			ReportError("CResourceQueueEvents","No spawn proc!");
			
			//Throw the error
			throw std::string("No spawn proc!");
		}

		//Create the pool
		m_pPool=new CThreadPool(ulThreads);
	}
	ERROR_HANDLER_RETHROW("CResourceQueueEvents")
}

template<class T>
CResourceQueueEvents<T>::~CResourceQueueEvents()
{
	try
	{
		//Delete the pool
		delete m_pPool;
	}
	ERROR_HANDLER("~CResourceQueueEvents")
}

template<class T>
BOOL CResourceQueueEvents<T>::RequestResource(T& rResource,
										 	  unsigned long ulTimeout)
{
	try
	{
		//Add the request
		AddRequest();

		//Delegate call
		return CResourceQueue<T>::RequestResource(rResource,
											      ulTimeout);
	}
	ERROR_HANDLER_RETURN("RequestResource",FALSE)
}

template<class T>
void CResourceQueueEvents<T>::AddRequest()
{
	try
	{
		//Submit a job for the user
		m_pPool->SubmitJob(SubmitThread,
						   (LPVOID)this);
	}
	ERROR_HANDLER_RETHROW("AddRequest")
}

template<class T>
void CResourceQueueEvents<T>::SubmitThread(LPVOID lpData)
{
	try
	{
		//Get the data
		CResourceQueueEvents<T>* pClass;
		pClass=(CResourceQueueEvents<T>*)lpData;

		//Call the user proc
		(*pClass->m_pSpawnProc)(pClass,
								pClass->m_lpUserData);
	}
	ERROR_HANDLER_STATIC(CResourceQueueEvents_Class,"SubmitThread")
}

KOMODIA_NAMESPACE_END

#endif