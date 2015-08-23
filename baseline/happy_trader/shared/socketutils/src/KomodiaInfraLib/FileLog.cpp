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
#include "FileLog.h"

#include "ErrorHandlerMacros.h"
#include "OSManager.h"
#include "GenericCriticalSection.h"

#include <time.h>
#include <assert.h>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

CFileLog::CFileLog() : CErrorHandler::CErrorLog(),
					   m_pFile(NULL),
					   m_pCSection(NULL),
					   m_dwMaxLogSize(5000000),
					   m_bTruncate(FALSE)
{
	//Create the CS
	m_pCSection=COSManager::CreateCriticalSection();
}

CFileLog::~CFileLog()
{
	try
	{
		//Close the file
		fclose(m_pFile);

		//Delete the CS
		delete m_pCSection;
	}
	ERROR_UNKNOWN("~CFileLog")
}

BOOL CFileLog::Initialize(const std::string& rFileName)
{
	//Open's the log file
	try
	{
		//Check if the file is open
		if (m_pFile)
			//close is
			fclose(m_pFile);

		//Which open mode
		BOOL bTruncate;
		bTruncate=FALSE;

		//Do we need to get the file size
		if (m_dwMaxLogSize)
		{
			//Open the file
			HANDLE hFile; 
			hFile=CreateFile(rFileName.c_str(),
							 GENERIC_READ,
							 FILE_SHARE_READ,
							 NULL,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL);
 
			//Do we have it?
			if (hFile!=INVALID_HANDLE_VALUE) 
			{ 
				//Get the file size
				DWORD dwSize;
				dwSize=GetFileSize(hFile,
								   NULL);

				//Close the file
				CloseHandle(hFile);

				//Is it bigger
				if (dwSize>m_dwMaxLogSize)
					bTruncate=TRUE;
			}
		}

		//Now open the file
		if (!bTruncate)
			m_pFile=fopen(rFileName.c_str(),"at");
		else
			m_pFile=fopen(rFileName.c_str(),"wt");

		//Save the file name
		m_sFileName=rFileName;

		//Did we manage to open it?
		if (!m_pFile)
			return FALSE;
		else
			return TRUE;
	}
	ERROR_UNKNOWN_RETURN("ReportError",FALSE)
}

bool CFileLog::WriteLog(const std::string& rData)
{
	try
	{
#ifdef _DEBUG
		m_aLog.WriteLog(rData);
#endif
		//Lock it
		CCriticalAutoRelease aRelease(m_pCSection);
    
		//Write to log
		fprintf(m_pFile,"%s\n",rData.c_str());

		//If error, or auto flush - then flush the data
		if (GetAutoFlush())
			fflush(m_pFile);

		//Do we need to truncate?
		if (m_bTruncate &&
			m_dwMaxLogSize &&
			ftell(m_pFile)>m_dwMaxLogSize)
		{
			//Reopen and close the log
			//Reinitialize if
			Initialize(m_sFileName);

			//Rewrite to the file
			WriteLog(CErrorHandler::FormatData("CFileLog",
											   "WriteLog",
											   "Log truncated!"));
		}

		//Done
		return true;
	}
	ERROR_UNKNOWN_RETURN("WriteLog",false)
}

void CFileLog::WriteError(const std::string& rError)
{
	WriteLog(rError);
}

void CFileLog::WriteMessage(const std::string& rMessage)
{
	WriteLog(rMessage);
}

void CFileLog::SetMaxLogSize(DWORD dwSize)
{
	m_dwMaxLogSize=dwSize;
}

void CFileLog::SetLogTruncate(BOOL bTruncate)
{
	m_bTruncate=bTruncate;
}

KOMODIA_NAMESPACE_END
