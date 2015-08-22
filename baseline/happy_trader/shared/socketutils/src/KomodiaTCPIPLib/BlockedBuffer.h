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

#if !defined(AFX_BLOCKEDBUFFER_H__4E19668B_9B87_4EB0_8E29_2F6A25FA1F5E__INCLUDED_)
#define AFX_BLOCKEDBUFFER_H__4E19668B_9B87_4EB0_8E29_2F6A25FA1F5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../socketutilsdefs.hpp"
#include "SocketBase.h"

#include <deque>

KOMODIA_NAMESPACE_START

class CAsyncSocket;
class CGenericCriticalSection;



class CBlockedBuffer : public CSocketBase
{
public:
	//The data class
	class CBlockedData
	{
	public:
		//Get number of calls
		int GetNumberOfCalls()const
		{
			return ++m_iCalls;
		}

		//Get the data
		char* GetData()const
		{
			return m_pData;
		}

		//Get the data size
		int GetDataSize()const
		{
			return m_iDataSize;
		}

		//ctor and dtor
		CBlockedData(const char* pData,
					 int iDataSize) : m_pData(NULL),
									  m_iDataSize(iDataSize),
									  m_iCalls(1)
		{
			//Do we have data
			if (pData && 
				m_iDataSize)
			{
				//Allocate and copy
				m_pData=new char[m_iDataSize];
				memcpy(m_pData,
					   pData,
					   m_iDataSize);
			}
		}

		CBlockedData(const CBlockedData& rData) : m_pData(NULL),
												  m_iDataSize(rData.m_iDataSize),
												  m_iCalls(rData.m_iCalls)
		{
			//Do we have data
			if (m_iDataSize && rData.m_pData)
			{
				//Allocate and copy
				m_pData=new char[m_iDataSize];
				memcpy(m_pData,
					   rData.m_pData,
					   m_iDataSize);
			}
		}

		virtual ~CBlockedData()
		{
			//Delete the data
			delete [] m_pData;
		}
	private:
		//Our data
		char* m_pData;

		//Data size
		int m_iDataSize;

		//calls count
		mutable int m_iCalls;
	};
public:
	//The event was called
	void SendData();

	//Add a record to the send
	void AddRecord(const CBlockedData& rRecord);

	//Ctor and Dtor
	//iMaxRetries - Number of times to retry sending one block
	//0 - Infinite times
	CBlockedBuffer(CAsyncSocket* pFather,
				   int iMaxRetries=5);
	virtual ~CBlockedBuffer();
private:
	//Our data vector
	typedef std::deque<CBlockedData> BlockedBufferData;
	
private:
	//No copy Ctor
	CBlockedBuffer(const CBlockedBuffer& rBuffer);

	//No assignment operator
	CBlockedBuffer& operator=(const CBlockedBuffer& rBuffer);

	//Our father
	CAsyncSocket* m_pFather;

	//Our CS
	CGenericCriticalSection* m_pCSection;

	//Our data
	BlockedBufferData m_aData;
	

	//Max retries of sending
	int m_iMaxRetries;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_BLOCKEDBUFFER_H__4E19668B_9B87_4EB0_8E29_2F6A25FA1F5E__INCLUDED_)
