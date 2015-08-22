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

#include "stdafx.h"
#include "Win32Event.h"

#include <string>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

CWin32Event::CWin32Event(BOOL bInitialState,
						 BOOL bManualReset,
						 const char* pEventName) : CGenericEvent(bInitialState,
																 bManualReset,
																 pEventName),
												   m_hEvent(NULL)
{
	//Create the event
	m_hEvent=CreateEvent(NULL,
						 bManualReset,
						 bInitialState,
						 pEventName);

	//Do we have the event
	if (!m_hEvent)
		throw std::string("Failed to create event!");

	//Check if we are the owners
	if (pEventName && 
		GetLastError()!=ERROR_ALREADY_EXISTS)
		SetFirst();
}

CWin32Event::~CWin32Event()
{
	if (m_hEvent)
		CloseHandle(m_hEvent);
}

void CWin32Event::Set()
{
	if (m_hEvent)
		SetEvent(m_hEvent);
}

BOOL CWin32Event::Wait(unsigned long ulMSTimeout)
{
	DWORD dwWaitResult;

	//Wait for the mutex
	dwWaitResult=WaitForSingleObject(m_hEvent,
								     ulMSTimeout);

	//Check why did we quit
	return dwWaitResult==WAIT_TIMEOUT;
}

HANDLE CWin32Event::GetHandle()const
{
	return m_hEvent;
}

void CWin32Event::Reset()
{
	if (m_hEvent)
		ResetEvent(m_hEvent);
}

void CWin32Event::Pulse()
{
	if (m_hEvent)
		PulseEvent(m_hEvent);
}

void CWin32Event::ReleaseObject()
{
	Reset();
}

void CWin32Event::PopulateOverlapped(OVERLAPPED& rOverlapped)
{
	rOverlapped.hEvent=m_hEvent;
}

KOMODIA_NAMESPACE_END
