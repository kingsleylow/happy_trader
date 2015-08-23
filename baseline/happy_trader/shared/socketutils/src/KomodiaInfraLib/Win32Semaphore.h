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

#if !defined(AFX_WIN32SEMAPHORE_H__D4CAE2C0_64CC_4329_8253_1A50F7C3A80E__INCLUDED_)
#define AFX_WIN32SEMAPHORE_H__D4CAE2C0_64CC_4329_8253_1A50F7C3A80E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericSemaphore.h"

KOMODIA_NAMESPACE_START

class CWin32Semaphore : public CGenericSemaphore
{
public:
	//Release the object
	virtual void ReleaseObject();

	//Wait for the semaphore
	//Returns TRUE for timeout
	virtual BOOL Aquire(unsigned long ulMSTimeout=INFINITE);

	//Decrease the semaphore count
	virtual long Release(long lAmount=1);

	//Ctor and Dtor
	//lInitialCount - The count the semaphore starts with
	//lMaxCount - Maximum count of the semaphore (can't exceed it)
	//pSemaphoreName - Name of the semaphore (for multiproccess)
	CWin32Semaphore(long lInitialCount,
					long lMaxCount,
					const char* pSemaphoreName=NULL);
	virtual ~CWin32Semaphore();
protected:
	//Get the handle
	virtual HANDLE GetHandle()const;
private:
	//No copy ctor
	CWin32Semaphore(const CWin32Semaphore& rSemaphore);

	//No copy operator
	CWin32Semaphore* operator=(const CWin32Semaphore& rSemaphore);

	//Our semaphore
	HANDLE m_hSemaphore;

	//Don't delete
	BOOL m_bDontDelete;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_WIN32SEMAPHORE_H__D4CAE2C0_64CC_4329_8253_1A50F7C3A80E__INCLUDED_)
