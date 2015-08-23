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

#ifndef _Array_ptr_H_
#define _Array_ptr_H_

#include "../KomodiaInfraLib/KomodiaSettings.h"

KOMODIA_NAMESPACE_START

template<class PointerType>
class CArray_ptr
{
public:
	//operators
	PointerType& operator*()
	{
		return *Get();
	}

	PointerType* operator->()const
	{
		return Get();
	}

	PointerType* Get()const
	{
		return m_pData;
	}

	PointerType* Release()const
	{
		//We are not the owners
		m_bOwners=FALSE;

		//Done with it
		return Get();
	}
	
	//Reset the current data
	void Reset()
	{
		//Release the data we have
		if (m_bOwners)
			delete [] m_pData;

		//We don't have it anymore
		m_pData=NULL;
	}

	//Assignment operator
	void operator=(const PointerType* pData)
	{
		//Reset the data
		Reset();

		//Take the new one
		m_pData=pData;

		//We are the owners
		m_bOwners=TRUE;

		//Release the ownership of the source PTR
		pData->ReleaseOwnership();
	}

	//ctor and dtor
	CArray_ptr(PointerType* pData) : m_pData(pData),
									 m_bOwners(TRUE)
	{
	}

	virtual ~CArray_ptr()
	{
		Reset();
	}
private:
	//Release ownership
	void ReleaseOwnership()const
	{
		m_bOwners=FALSE;
	}

	//Our data
	PointerType* m_pData;

	//Are we the owners
	mutable BOOL m_bOwners;
};

KOMODIA_NAMESPACE_END

#endif