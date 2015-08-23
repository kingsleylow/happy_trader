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

#if !defined(AFX_GENERICREADWRITELOCK_H__BCA15B95_3F3D_48BA_8310_01DB58AB415F__INCLUDED_)
#define AFX_GENERICREADWRITELOCK_H__BCA15B95_3F3D_48BA_8310_01DB58AB415F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KomodiaSettings.h"
#include "RWAutoRelease.h"

KOMODIA_NAMESPACE_START

class CGenericReadWriteLock  
{
public:
	//Lock for reading
	//Returns TRUE for timeout
	virtual BOOL LockRead(unsigned long ulMSTimeout=INFINITE)=0;

	//Unlock for reading
	virtual void UnlockRead()=0;

	//Lock for reading
	virtual BOOL LockWrite()=0;

	//Unlock for writing
	virtual void UnlockWrite()=0;

	//Escalate write priority
	virtual BOOL Escalate(unsigned long ulMSTimeout,
						  unsigned long ulIterations)=0;

	//Remove the escalation flag
	virtual void StopEscalation()=0;

	//Ctor and Dtor
	//iMaximumReaders - Maximum readers in the same time
	CGenericReadWriteLock(int iMaximumReaders);
	virtual ~CGenericReadWriteLock();
protected:
	//Get the number of readers
	int GetMaximumReaders()const;
private:
	//Number of readers
	int m_iMaximumReaders;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_GENERICREADWRITELOCK_H__BCA15B95_3F3D_48BA_8310_01DB58AB415F__INCLUDED_)
