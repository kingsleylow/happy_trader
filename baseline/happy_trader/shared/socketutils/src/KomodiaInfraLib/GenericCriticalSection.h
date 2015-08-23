/*
 *
 *
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
 *
 * Contact info:
 * Site: http://www.komodia.com
 * Email: barak@komodia.com
 */

#if !defined(AFX_GENERICCRITICALSECTION_H__2094C4F2_5011_4957_A375_A5AEEFEC6301__INCLUDED_)
#define AFX_GENERICCRITICALSECTION_H__2094C4F2_5011_4957_A375_A5AEEFEC6301__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KomodiaSettings.h"
#include "CriticalAutoRelease.h"

KOMODIA_NAMESPACE_START

class CGenericCriticalSection  
{
public:
	//Enter the critical section
	virtual void Enter()=0;

	//Exit the critical section
	virtual void Exit()=0;

	//Ctor and Dtor
	CGenericCriticalSection();
	virtual ~CGenericCriticalSection();
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_GENERICCRITICALSECTION_H__2094C4F2_5011_4957_A375_A5AEEFEC6301__INCLUDED_)
