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

#if !defined(AFX_WAITLIST_H__1947A0FC_5D3F_466B_9674_6054AFFDE46E__INCLUDED_)
#define AFX_WAITLIST_H__1947A0FC_5D3F_466B_9674_6054AFFDE46E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KomodiaSettings.h"

KOMODIA_NAMESPACE_START

class CWaitableObject;
class CWaitableObjectAutoRelease;

class CWaitList  
{
public:
	//Release all waiting data
	void Release();

	//Do we want to auto release the data
	void SetAutoRelease(BOOL bRelease);

	//Wait on an object
	//bWaitAll - False, wait for any object
	//			 TRUE, wait to all objects
	//dwSignaledObject - Object signaled (bWaitAll is FALSE)
	//ulTimeout - Timeout to wait (MS)
	//Returns TRUE if timeout
	BOOL Wait(BOOL bWaitAll,
			  DWORD& dwSignaledObject,
			  unsigned long ulTimeout=INFINITE);

	//Add an object
	void AddObject(CWaitableObject* pObject,
				   BOOL bNoRelease=FALSE);

	//Ctor and Dtor
	CWaitList();
	virtual ~CWaitList();
private:
	//No copy ctor
	CWaitList(const CWaitList& rList);

	//No assignment operator
	CWaitList& operator=(const CWaitList& rList);

	//Build the auto release pointers
	void BuildAutoRelease(unsigned long ulPosition);

	//The wait array
	HANDLE m_aHandles[MAXIMUM_WAIT_OBJECTS];

	//To release or not
	BOOL m_bRelease[MAXIMUM_WAIT_OBJECTS];

	//The actual data
	CWaitableObject* m_aObjects[MAXIMUM_WAIT_OBJECTS];

	//Our count
	unsigned long m_ulWaitCount;

	//Auto release flag
	BOOL m_bAutoRelease;

	//Our release pointer
	CWaitableObjectAutoRelease** m_ppRelease;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_WAITLIST_H__1947A0FC_5D3F_466B_9674_6054AFFDE46E__INCLUDED_)
