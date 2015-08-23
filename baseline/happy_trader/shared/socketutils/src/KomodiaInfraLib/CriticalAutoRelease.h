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

#if !defined(AFX_CRITICALAUTORELEASE_H__F03329D2_DEF1_4EB1_9FCE_431466BE98A9__INCLUDED_)
#define AFX_CRITICALAUTORELEASE_H__F03329D2_DEF1_4EB1_9FCE_431466BE98A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KomodiaSettings.h"

KOMODIA_NAMESPACE_START

class CGenericCriticalSection;
class CCriticalSectionForProtection;

class CCriticalAutoRelease  
{
public:
	//Enter (will try to lock the CS, can be called multiple times)
	void Enter();

	//Exit (will try to release the CS, can be called multiple times)
	void Exit();

	//Ctor and Dtor
	//Ctor will automatically try to lock the CS
	//(unless bLock is FALSE)
	CCriticalAutoRelease(CGenericCriticalSection* pCS,
						 BOOL bLock=TRUE);
	CCriticalAutoRelease(const CCriticalSectionForProtection* pCS,
						 BOOL bLock=FALSE);
	virtual ~CCriticalAutoRelease();
private:
	//No copy Ctor
	CCriticalAutoRelease(const CCriticalAutoRelease& rRelease);

	//No copy operator
	CCriticalAutoRelease& operator=(const CCriticalAutoRelease& rRelease);

	//Our CS
	CGenericCriticalSection* m_pCSection;

	//Our renetry count
	unsigned long m_ulEntryCount;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_CRITICALAUTORELEASE_H__F03329D2_DEF1_4EB1_9FCE_431466BE98A9__INCLUDED_)
