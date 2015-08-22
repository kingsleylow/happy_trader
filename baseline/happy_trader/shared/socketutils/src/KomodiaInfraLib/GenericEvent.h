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

#if !defined(AFX_GENERICEVENT_H__84D8DB3D_D3D4_4B86_8ABC_54D15BB242A6__INCLUDED_)
#define AFX_GENERICEVENT_H__84D8DB3D_D3D4_4B86_8ABC_54D15BB242A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KomodiaSettings.h"
#include "WaitableObject.h"

KOMODIA_NAMESPACE_START

class CGenericEvent : public CWaitableObject
{
public:
	//Pulse the event 
	//(after all waiting events are released, it resets itself)
	virtual void Pulse()=0;

	//Set the event
	virtual void Set()=0;

	//Reset the event
	virtual void Reset()=0;

	//Wait for the event
	//Returns TRUE if timeout occured
	virtual BOOL Wait(unsigned long ulMSTimeout=INFINITE)=0;

	//Ctor and Dtor
	//bInitialState - Is the event started as Set/Reset
	//bManualReset - Does the event resets automatically
	//pEventName - Name of the event (for multiproccess)
	CGenericEvent(BOOL bInitialState=FALSE,
				  BOOL bManualReset=FALSE,
				  const char* pEventName=NULL);
	virtual ~CGenericEvent();
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_GENERICEVENT_H__84D8DB3D_D3D4_4B86_8ABC_54D15BB242A6__INCLUDED_)
