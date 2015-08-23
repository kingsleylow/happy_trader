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

#if !defined(AFX_RWAUTORELEASE_H__FB076110_4E72_48F9_8B5C_23777362C277__INCLUDED_)
#define AFX_RWAUTORELEASE_H__FB076110_4E72_48F9_8B5C_23777362C277__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KomodiaSettings.h"

KOMODIA_NAMESPACE_START

class CGenericReadWriteLock;

class CRWAutoRelease  
{
public:
	//Lock for reading
	//Returns TRUE for timeout
	BOOL LockRead(unsigned long ulMSTimeout=INFINITE);

	//Unlock for reading
	void UnlockRead();

	//Lock for reading
	BOOL LockWrite();

	//Unlock for writing
	void UnlockWrite();

	//Escalate write priority
	BOOL Escalate(unsigned long ulMSTimeout,
				  unsigned long ulIterations);

	//Remove the escalation flag
	void StopEscalation();

	//Ctor and Dtor
	//bRead - Lock it for reading
	//bLock - Lock it at start
	CRWAutoRelease(CGenericReadWriteLock* pLock,
				   BOOL bRead=TRUE,
				   BOOL bLock=FALSE);
	virtual ~CRWAutoRelease();
private:
	//No copy Ctor
	CRWAutoRelease(const CRWAutoRelease& rRelease);

	//No copy operator
	CRWAutoRelease& operator=(const CRWAutoRelease& rRelease);

	//Our class
	CGenericReadWriteLock* m_pLock;

	//Are we locked for read
	BOOL m_bLockedRead;

	//Are we locked for write
	BOOL m_bLockedWrite;

	//Are we escalated
	BOOL m_bEscalated;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_RWAUTORELEASE_H__FB076110_4E72_48F9_8B5C_23777362C277__INCLUDED_)
