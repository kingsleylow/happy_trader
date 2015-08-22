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

#if !defined(AFX_WIN32CRITICALSECTION_H__23A0B451_A607_4C00_9AF0_504CCB20B8A3__INCLUDED_)
#define AFX_WIN32CRITICALSECTION_H__23A0B451_A607_4C00_9AF0_504CCB20B8A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#include "GenericCriticalSection.h"

KOMODIA_NAMESPACE_START

class CWin32CriticalSection : public CGenericCriticalSection  
{
public:
	//Enter the critical section
	virtual void Enter();

	//Exit the critical section
	virtual void Exit();

	//Ctor and Dtor
	CWin32CriticalSection();
	virtual ~CWin32CriticalSection();
private:
	//No copy ctor
	CWin32CriticalSection(const CWin32CriticalSection& rCS);

	//No assignment operator
	CWin32CriticalSection& operator=(const CWin32CriticalSection& rCS);
	
	//Our critical section
	CRITICAL_SECTION m_pCSection;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_WIN32CRITICALSECTION_H__23A0B451_A607_4C00_9AF0_504CCB20B8A3__INCLUDED_)
