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
#include "BlockedBuffer.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"
#include "../KomodiaInfraLib/OSManager.h"
#include "../KomodiaInfraLib/GenericCriticalSection.h"

#include "AsyncSocket.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CBlockedBuffer_Class "CBlockedBuffer"

CBlockedBuffer::CBlockedBuffer(CAsyncSocket* pFather,
							   int iMaxRetries) : CSocketBase(),
												  m_pFather(pFather),
												  m_pCSection(NULL),
												  m_iMaxRetries(iMaxRetries)
{
	try
	{
		//Set our name
		SetName(CBlockedBuffer_Class);

		//Create the CS
		m_pCSection=COSManager::CreateCriticalSection();
	}
	ERROR_HANDLER("CBlockedBuffer")
}

CBlockedBuffer::~CBlockedBuffer()
{
	try
	{
		//Delete the CS
		delete m_pCSection;
	}
	ERROR_HANDLER("~CBlockedBuffer")
}

void CBlockedBuffer::SendData()
{
	try
	{
		//Lock
		CCriticalAutoRelease aRelease(m_pCSection);

		//Do we have data ?
		if (!m_aData.empty())
			//Try to send it
			if (m_pFather->SendBlockedBuffer(*m_aData.begin()))
				//We can remove it
				m_aData.pop_front();
			else if (m_iMaxRetries &&
					 m_aData.begin()->GetNumberOfCalls()==m_iMaxRetries)
			{
				//We can't send it, remove it and indicate an error
				m_aData.pop_front();

				//Report it
				ReportError("SendData","Failed sending data for more then 5 times!");
			}
	}
	ERROR_HANDLER("SendData")
}

void CBlockedBuffer::AddRecord(const CBlockedData& rRecord)
{
	try
	{
		//Lock the vector and add the data
		CCriticalAutoRelease aRelease(m_pCSection);

		//Add it
		m_aData.push_back(rRecord);
	}
	ERROR_HANDLER("AddRecord")
}

KOMODIA_NAMESPACE_END
