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

#if !defined(AFX_INTERFACES_H__CDBDAE95_6A20_4784_9DD5_83A7270AF82F__INCLUDED_)
#define AFX_INTERFACES_H__CDBDAE95_6A20_4784_9DD5_83A7270AF82F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SocketBase.h"

KOMODIA_NAMESPACE_START

class SOCKETUTILS_EXP CInterfaces : public CSocketBase  
{
public:
	//Information about the interface
	BOOL IsMulticast()const;
	BOOL IsPPP()const;
	BOOL IsLoopback()const;
	BOOL IsBroadcast()const;
	BOOL IsRunning()const;

	//Get the broadcast address
	long GetBroadcast()const;

	//Get the netmask
	long GetMask()const;

	//Move to the next interface
	BOOL MoveNext();

	//Get the interface address
	long GetAddress()const;

	//Retreive the list of all the interfaces
	BOOL GetInterfaces();

	//Ctor and Dtor
	CInterfaces(int iMaxInterfaces=20);
	virtual ~CInterfaces();
private:
	//No copy ctor
	CInterfaces(const CInterfaces& rInterfaces);

	//No copy operator
	CInterfaces& operator=(const CInterfaces& rInterfaces);

	//Get the flags
	long GetFlags()const;

	//Our interface list
	INTERFACE_INFO* m_pInfo;

	//Number of interfaces
	int m_iMaxInterfaces;

	//How many structures we have
	int m_iStructures;

	//Our position
	int m_iPosition;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_INTERFACES_H__CDBDAE95_6A20_4784_9DD5_83A7270AF82F__INCLUDED_)
