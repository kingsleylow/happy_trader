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
#include "CriticalAutoRelease.h"

#include "ErrorHandlerMacros.h"
#include "GenericCriticalSection.h"
#include "CriticalSectionForProtection.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

CCriticalAutoRelease::CCriticalAutoRelease(CGenericCriticalSection* pCS,
										   BOOL bLock) : m_pCSection(pCS),
														 m_ulEntryCount(0)
{
	//Do we need to lock ourselves?
	if (bLock)
		Enter();
}

CCriticalAutoRelease::CCriticalAutoRelease(const CCriticalSectionForProtection* pCS,
										   BOOL bLock) : m_pCSection(NULL),
														 m_ulEntryCount(0)
{
	//Do we have something to copy?
	if (pCS)
		m_pCSection=pCS->GetCriticalSection();

	//Continue
	if (bLock)
		Enter();
}

CCriticalAutoRelease::~CCriticalAutoRelease()
{
	//Check if valid
	if (m_pCSection)
		//How many entries
		for (unsigned long ulCounter=0;ulCounter<m_ulEntryCount;++ulCounter)
			m_pCSection->Exit();
}

void CCriticalAutoRelease::Enter()
{
	if (m_pCSection)
	{
		//Lock the CS
		m_pCSection->Enter();

		//Increate the count
		++m_ulEntryCount;
	}
}

void CCriticalAutoRelease::Exit()
{
	if (m_pCSection && 
		m_ulEntryCount)
	{
		//Decrease the count
		--m_ulEntryCount;

		//And exit
		m_pCSection->Exit();
	}
}

KOMODIA_NAMESPACE_END