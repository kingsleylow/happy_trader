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

#if !defined(AFX_FILELOG_H__D08640BA_1A6F_46F4_89D3_5F4A4A8D8541__INCLUDED_)
#define AFX_FILELOG_H__D08640BA_1A6F_46F4_89D3_5F4A4A8D8541__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ErrorHandler.h"

#ifdef _DEBUG
#include "TraceLog.h"
#endif

#include <stdio.h>

KOMODIA_NAMESPACE_START

class CGenericCriticalSection;

class CFileLog : public CErrorHandler::CErrorLog
{
public:
	//Write to the log
	bool WriteLog(const std::string& rData);

	//Set the max log size in bytes (default is 5000000,0 is no truncate)
	void SetMaxLogSize(DWORD dwSize);

	//Truncate the log on the fly (if while working log exeeds size)
	void SetLogTruncate(BOOL bTruncate);

	//Initialize the log
	BOOL Initialize(const std::string& rFileName);

	//Ctor and Dtor
	CFileLog();
	virtual ~CFileLog();
protected:
	//Report an error must overide
	virtual void WriteError(const std::string& rError);

	//Report a regular message
	virtual void WriteMessage(const std::string& rMessage);
private:
	//No copy Ctor
	CFileLog(const CFileLog& rLog);

	//No copy operator
	CFileLog& operator=(const CFileLog& rLog);

	//The original file name
	std::string m_sFileName;

	//Our file
	FILE* m_pFile;

	//Our CS
	CGenericCriticalSection* m_pCSection;

	//Max log size
	DWORD m_dwMaxLogSize;

	//Truncate flag
	BOOL m_bTruncate;

	//For debug only, trace log
#ifdef _DEBUG
	CTraceLog m_aLog;
#endif
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_FILELOG_H__D08640BA_1A6F_46F4_89D3_5F4A4A8D8541__INCLUDED_)
