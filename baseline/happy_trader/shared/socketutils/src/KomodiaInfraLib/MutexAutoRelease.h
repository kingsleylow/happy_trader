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

#if !defined(AFX_MUTEXAUTORELEASE_H__BA1CC9CD_BAEB_40CE_AA38_AEAC7224C904__INCLUDED_)
#define AFX_MUTEXAUTORELEASE_H__BA1CC9CD_BAEB_40CE_AA38_AEAC7224C904__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KomodiaSettings.h"

KOMODIA_NAMESPACE_START

class CGenericMutex;

class CMutexAutoRelease  
{
public:
	//Wait for the mutex
	//Returns TRUE for timeout
	BOOL Aquire(unsigned long ulMSTimeout=INFINITE);

	//Release the mutex
	BOOL Release();

	//Ctor and Dtor
	//Ctor will automatically try to lock the mutex
	//(unless bLock is FALSE)
	CMutexAutoRelease(CGenericMutex* pMutex,
					  BOOL bLock=TRUE,
					  unsigned long ulMSTimeout=INFINITE);
	virtual ~CMutexAutoRelease();
private:
	//No copy Ctor
	CMutexAutoRelease(const CMutexAutoRelease& rRelease);

	//No assignment operator
	CMutexAutoRelease& operator=(const CMutexAutoRelease& rMutex);

	//Our mutex
	CGenericMutex* m_pMutex;

	//Our count
	unsigned long m_ulEntryCount;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_MUTEXAUTORELEASE_H__BA1CC9CD_BAEB_40CE_AA38_AEAC7224C904__INCLUDED_)
