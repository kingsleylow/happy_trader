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

#if !defined(AFX_WIN32READWRITELOCK_H__C86B09CB_919E_498F_871A_1919E476469A__INCLUDED_)
#define AFX_WIN32READWRITELOCK_H__C86B09CB_919E_498F_871A_1919E476469A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericReadWriteLock.h"

KOMODIA_NAMESPACE_START

class CGenericSemaphore;
class CGenericMutex;

class CWin32ReadWriteLock : public CGenericReadWriteLock  
{
public:
	//Lock for reading
	//Returns TRUE for timeout
	virtual BOOL LockRead(unsigned long ulMSTimeout=INFINITE);

	//Unlock for reading
	virtual void UnlockRead();

	//Lock for reading
	virtual BOOL LockWrite();

	//Unlock for reading
	virtual void UnlockWrite();

	//Escalate write priority
	virtual BOOL Escalate(unsigned long ulMSTimeout,
						  unsigned long ulIterations);

	//Remove the escalation flag
	virtual void StopEscalation();

	//Ctor and Dtor
	CWin32ReadWriteLock(int iMaximumReaders);
	virtual ~CWin32ReadWriteLock();
private:
	//No copy ctor
	CWin32ReadWriteLock(const CWin32ReadWriteLock& rLock);

	//No copy operator
	CWin32ReadWriteLock& operator=(const CWin32ReadWriteLock& rLock);

	//Our semaphore
	CGenericSemaphore* m_pSemaphore;

	//Our mutex
	CGenericMutex* m_pMutex;

	//Do we have a writer
	BOOL m_bWriter;

	//Do we have escalation
	BOOL m_bEscalation;

	//Number of readers
	int m_iReaders;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_WIN32READWRITELOCK_H__C86B09CB_919E_498F_871A_1919E476469A__INCLUDED_)
