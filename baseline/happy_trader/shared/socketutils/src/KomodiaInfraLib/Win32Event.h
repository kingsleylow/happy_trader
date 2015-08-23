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

#if !defined(AFX_WIN32EVENT_H__62701C68_0625_43F6_9DF9_56C79D51346B__INCLUDED_)
#define AFX_WIN32EVENT_H__62701C68_0625_43F6_9DF9_56C79D51346B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericEvent.h"

KOMODIA_NAMESPACE_START

class CWin32Event : public CGenericEvent
{
public:	
	//Populate a overlapped strucuture
	void PopulateOverlapped(OVERLAPPED& rOverlapped);

	//Release the object
	virtual void ReleaseObject();

	//Pulse the event 
	//(after all waiting events are released, it resets itself)
	virtual void Pulse();

	//Set the event
	virtual void Set();

	//Reset the event
	virtual void Reset();

	//Wait for the event
	virtual BOOL Wait(unsigned long ulMSTimeout=INFINITE);

	//Ctor and Dtor
	//bInitialState - Is the event started as Set/Reset
	//bManualReset - Does the event resets automatically
	//pEventName - Name of the event (for multiproccess)
	CWin32Event(BOOL bInitialState=FALSE,
				BOOL bManualReset=FALSE,
				const char* pEventName=NULL);
	virtual ~CWin32Event();
protected:
	//Get the handle
	virtual HANDLE GetHandle()const;
private:
	//No copy ctor
	CWin32Event(const CWin32Event& rEvent);

	//No assignment operator
	CWin32Event& operator=(const CWin32Event& rEvent);

	//Our event
	HANDLE m_hEvent;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_WIN32EVENT_H__62701C68_0625_43F6_9DF9_56C79D51346B__INCLUDED_)
