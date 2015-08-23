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

#ifndef _ResourceQueue_H_
#define _ResourceQueue_H_

#include "ErrorHandler.h"
#include "OSManager.h"
#include "GenericCriticalSection.h"
#include "GenericSemaphore.h"
#include "ErrorHandlerMacros.h"

#include <deque>
#include <algorithm>

KOMODIA_NAMESPACE_START

template<class T>
class CResourceQueue : public CErrorHandler
{
public:
	//Run all function
	typedef void (*DeleteProc)(T& rItem);
	typedef DeleteProc LPDeleteProc;
public:
	//Get data in queue
	unsigned long GetDataCount()const;

	//Request a resource
	virtual BOOL RequestResource(T& rResource,
								 unsigned long ulTimeout=INFINITE);

	//Add a resource
	virtual void AddResource(T& rResource);

	//Ctor and Dtor
	CResourceQueue(LPDeleteProc pCleanUPProc=NULL,
				   unsigned long ulMaxItems=0);
	virtual ~CResourceQueue();
private:
	//Our queue of items
	typedef std::deque<T> ItemsQueue;
private:
	//No copy Ctor
	CResourceQueue(const CResourceQueue& rQueue);

	//No copy operator
	CResourceQueue& operator=(const CResourceQueue& rQueue);

	//Our items
	ItemsQueue m_aItems;

	//Our CS
	CGenericCriticalSection* m_pCSection;

	//Our semaphore
	CGenericSemaphore* m_pSemaphore;

	//Our delete proc
	LPDeleteProc m_pProc;

	//Max items
	unsigned long m_ulMaxItems;
};

#define CResourceQueue_Class "CResourceQueue"

template<class T>
CResourceQueue<T>::CResourceQueue(LPDeleteProc pCleanUPProc,
								  unsigned long ulMaxItems) : CErrorHandler(),
															  m_pCSection(NULL),
															  m_pSemaphore(NULL),
															  m_pProc(pCleanUPProc),
															  m_ulMaxItems(ulMaxItems)
{
	try
	{
		//Set our name
		SetName(CResourceQueue_Class);

		//Create the CS
		m_pCSection=COSManager::CreateCriticalSection();

		//Create the semaphore
		m_pSemaphore=COSManager::CreateSemaphore(0,
												 INFINITE/3);
	}
	ERROR_HANDLER("CResourceQueue")
}

template<class T>
CResourceQueue<T>::~CResourceQueue()
{
	try
	{
		//Delete the CS
		delete m_pCSection;

		//Delete the semaphore
		delete m_pSemaphore;

		//Do we have a proc?
		if (m_pProc)
			//Run it
			std::for_each(m_aItems.begin(),
						  m_aItems.end(),
						  m_pProc);
	}
	ERROR_HANDLER("~CResourceQueue")
}

template<class T>
BOOL CResourceQueue<T>::RequestResource(T& rResource,
										unsigned long ulTimeout)
{
	try
	{
		//Try to aquire the semaphore
		if (m_pSemaphore->Aquire(ulTimeout))
			//Timeout
			return FALSE;

		//Lock the CS
		CCriticalAutoRelease aRelease(m_pCSection);

		//Take the item
		rResource=m_aItems.front();
		m_aItems.pop_front();

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("RequestResource",FALSE)
}

template<class T>
void CResourceQueue<T>::AddResource(T& rResource)
{
	try
	{
		{
			while (1)
			{
				//Lock the CS
				CCriticalAutoRelease aRelease(m_pCSection);

				//Can we add
				if (!m_ulMaxItems ||
					m_aItems.size()<m_ulMaxItems)
				{
					//Add the item
					m_aItems.push_back(rResource);

					//Exit
					break;
				}
				else
					Sleep(1);
			}
		}

		//Indicate we have a spare resource
		m_pSemaphore->Release();
	}
	ERROR_HANDLER_RETHROW("AddResource")
}

template<class T>
unsigned long CResourceQueue<T>::GetDataCount()const
{
	try
	{
		//Lock the CS
		CCriticalAutoRelease aRelease(m_pCSection);

		//Get the size
		return m_aItems.size();
	}
	ERROR_HANDLER_RETURN("GetDataCount",0)
}

KOMODIA_NAMESPACE_END

#endif
