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

#if !defined(AFX_GENERICSEMAPHORE_H__ABEC2DE4_97AC_4EAE_BB8D_561D8ECC5C31__INCLUDED_)
#define AFX_GENERICSEMAPHORE_H__ABEC2DE4_97AC_4EAE_BB8D_561D8ECC5C31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KomodiaSettings.h"
#include "WaitableObject.h"

KOMODIA_NAMESPACE_START

class CGenericSemaphore : public CWaitableObject
{
public:
	//Wait for the semaphore
	//Returns TRUE for timeout
	virtual BOOL Aquire(unsigned long ulMSTimeout=INFINITE)=0;

	//Decrease the semaphore count
	virtual long Release(long lAmount=1)=0;

	//Ctor and Dtor
	//lInitialCount - The count the semaphore starts with
	//lMaxCount - Maximum count of the semaphore (can't exceed it)
	//pSemaphoreName - Name of the semaphore (for multiproccess)
	CGenericSemaphore(long lInitialCount,
					  long lMaxCount,
					  const char* pSemaphoreName=NULL);
	virtual ~CGenericSemaphore();
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_GENERICSEMAPHORE_H__ABEC2DE4_97AC_4EAE_BB8D_561D8ECC5C31__INCLUDED_)
