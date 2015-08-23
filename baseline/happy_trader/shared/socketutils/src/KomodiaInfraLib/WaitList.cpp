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
#include "WaitList.h"

#include "ErrorHandlerMacros.h"
#include "WaitableObject.h"
#include "WaitableObjectAutoRelease.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

CWaitList::CWaitList() : m_ulWaitCount(0),
						 m_bAutoRelease(FALSE),
						 m_ppRelease(NULL)
{
}

CWaitList::~CWaitList()
{
	Release();
}

BOOL CWaitList::Wait(BOOL bWaitAll,
					 DWORD& dwSignaledObject,
					 unsigned long ulTimeout)
{
	//Put invalid value in the signaled object
	dwSignaledObject=MAXIMUM_WAIT_OBJECTS;

	//Check we have objects
	if (!m_ulWaitCount)
		return FALSE;
	
	//And wait
	DWORD dwResult;
	dwResult=WaitForMultipleObjects(m_ulWaitCount,
									m_aHandles,
									bWaitAll,
									ulTimeout);

	//check why we exited
	if (dwResult==WAIT_TIMEOUT)
		return TRUE;

	//Check which object
	if (dwResult>=WAIT_OBJECT_0 &&
		dwResult<WAIT_OBJECT_0+m_ulWaitCount)
	{
		//Set the signaled object
		dwSignaledObject=dwResult-WAIT_OBJECT_0;

		//Do we need to build a list ?
		if (m_bAutoRelease)
			if (bWaitAll)
				BuildAutoRelease(0);
			else
				BuildAutoRelease(dwSignaledObject+1);

		//Exit
		return FALSE;
	}

	//Abandon stuff, don't need it
	return FALSE;
}

void CWaitList::AddObject(CWaitableObject* pObject,
						  BOOL bNoRelease)
{
	if (m_ulWaitCount==MAXIMUM_WAIT_OBJECTS)
		return;

	//And put the object
	m_aHandles[m_ulWaitCount]=pObject->GetHandle();
	m_aObjects[m_ulWaitCount]=pObject;
	m_bRelease[m_ulWaitCount]=!bNoRelease;

	//Increase the count
	++m_ulWaitCount;
}

void CWaitList::SetAutoRelease(BOOL bRelease)
{
	m_bAutoRelease=bRelease;
}

void CWaitList::BuildAutoRelease(unsigned long ulPosition)
{
	//Release the pointer first
	Release();

	//How many items
	unsigned long ulItems;

	if (ulPosition)
		//Check if we can use this for the auto release
		if (m_bRelease[ulPosition-1])
			ulItems=1;
		else
			//Exit
			return;
	else
		ulItems=m_ulWaitCount;

	//Build the list
	m_ppRelease=new CWaitableObjectAutoRelease*[ulItems+1];
	memset(m_ppRelease,
		   0,
		   sizeof(CWaitableObjectAutoRelease*)*(ulItems+1));

	//Populate it
	if (ulPosition)
		m_ppRelease[0]=new CWaitableObjectAutoRelease(m_aObjects[ulPosition-1]);
	else
	{
		unsigned long ulRunningCounter;
		ulRunningCounter=0;

		//And build it
		for (unsigned long ulCounter=0;
			 ulCounter<m_ulWaitCount;
			 ++ulCounter)
			//Can we auto release it
			if (m_bRelease[ulCounter])
				//Add it
				m_ppRelease[ulRunningCounter++]=new CWaitableObjectAutoRelease(m_aObjects[ulCounter]);
	}

	//Done
}

void CWaitList::Release()
{
	if (m_ppRelease)
	{
		//Iterate it
		CWaitableObjectAutoRelease** ppBackup;
		ppBackup=m_ppRelease;

		while (*ppBackup)
		{
			//Delete the data
			delete *ppBackup;

			//Increase our position
			++ppBackup;
		}

		//Delete it all
		delete [] m_ppRelease;
		m_ppRelease=NULL;
	}
}

KOMODIA_NAMESPACE_END