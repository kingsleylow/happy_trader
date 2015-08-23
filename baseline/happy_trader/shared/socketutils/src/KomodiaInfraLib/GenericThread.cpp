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
#include "GenericThread.h"

#include "ErrorHandlerMacros.h"
#include "OSManager.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CGenericThread_Class "CGenericThread"

CGenericThread::CGenericThread(LPGenericThreadProc pThreadProc) : CErrorHandler(),
																  m_pThreadProc(pThreadProc),
																  m_dwThreadID(0),
																  m_pData(NULL),
																  m_aThreadStatus(tsError),
																  m_pCSection(NULL),
																  m_bAutoDelete(FALSE),
																  m_bBruteTermination(TRUE)
{
	//Set our name
	SetName(CGenericThread_Class);

	//Create the critical section
	m_pCSection=COSManager::CreateCriticalSection();
}

CGenericThread::~CGenericThread()
{
	try
	{
		delete m_pCSection;
	}
	ERROR_HANDLER("~CGenericThread")
}

DWORD CGenericThread::GetThreadID()const
{
	return m_dwThreadID;
}

void CGenericThread::SetThreadID(DWORD dwThreadID)
{
	m_dwThreadID=dwThreadID;
}

LPVOID CGenericThread::GetData()const
{
	return m_pData;
}

BOOL CGenericThread::Start(LPVOID pData)
{
	//Save the data
	m_pData=pData;

	//Done
	return TRUE;
}

void CGenericThread::SetThreadStatus(ThreadStatus aStatus)
{
	try
	{
		//Enter the CS
		CCriticalAutoRelease aRelease(m_pCSection);

		//Set it
		m_aThreadStatus=aStatus;
	}
	ERROR_HANDLER("SetThreadStatus")
}

CGenericThread::ThreadStatus CGenericThread::GetThreadStatus() const
{
	return m_aThreadStatus;
}

CGenericThread::LPGenericThreadProc CGenericThread::GetThreadProc()const
{
	return m_pThreadProc;
}

void CGenericThread::SetAutoDelete(BOOL bAuto)
{
	m_bAutoDelete=bAuto;
}

BOOL CGenericThread::GetAutoDelete()const
{
	return m_bAutoDelete;
}

void CGenericThread::SetBruteTermination(BOOL bBrute)
{
	m_bBruteTermination=bBrute;
}

BOOL CGenericThread::GetBruteTermination()const
{
	return m_bBruteTermination;
}

KOMODIA_NAMESPACE_END