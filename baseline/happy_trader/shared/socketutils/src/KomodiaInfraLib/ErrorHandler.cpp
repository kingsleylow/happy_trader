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
#include "ErrorHandler.h"

#include "ErrorHandlerMacros.h"

#include <assert.h>
#include <time.h>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#pragma warning(disable : 4996)

//----------------------- CErrorLog start -----------------------

CErrorHandler::CErrorLog::CErrorLog() : m_bAutoFlush(TRUE)
{
}

CErrorHandler::CErrorLog::CErrorLog(const CErrorLog& rLog) : m_bAutoFlush(rLog.m_bAutoFlush)
{
}

CErrorHandler::CErrorLog::~CErrorLog()
{
}

CErrorHandler::CErrorLog& CErrorHandler::CErrorLog::operator=(const CErrorLog& rLog)
{
	//Copy the flush status
	m_bAutoFlush=rLog.m_bAutoFlush;

	//Done
	return *this;
}

void CErrorHandler::CErrorLog::SetAutoFlush(BOOL bFlush)
{
	m_bAutoFlush=bFlush;
}

BOOL CErrorHandler::CErrorLog::GetAutoFlush()const
{
	return m_bAutoFlush;
}

//----------------------- CErrorLog end -----------------------

//----------------------- CErrorsRepository start -----------------------

CErrorHandler::CErrorsRepository::ErrorMap& CErrorHandler::CErrorsRepository::GetErrorsMap()
{
	return m_aErrorMap;
}

CErrorHandler::CErrorsRepository& CErrorHandler::CErrorsRepository::GetInstance()
{
	//Our instance
	static CErrorsRepository sInstance;

	//Done
	return sInstance;
}

//----------------------- CErrorsRepository end -----------------------

//Our global log
CErrorHandler::CErrorLog* CErrorHandler::m_pLog=NULL;

#define CErrorHandler_Class "CErrorHandler"

CErrorHandler::CErrorHandler() : m_pLocalLog(NULL)
{
	try
	{
		//Set it
		SetName(CErrorHandler_Class);
	}
	ERROR_HANDLER("CErrorHandler")
}

CErrorHandler::CErrorHandler(const CErrorHandler& rHandler) : m_pLocalLog(NULL)
{
	try
	{
		//Set it
		SetName(CErrorHandler_Class);
	}
	ERROR_HANDLER("CErrorHandler")
}

CErrorHandler::~CErrorHandler()
{
}

void CErrorHandler::SetName(const std::string& rName)const
{
	//Save the class name
	m_sClassName=rName;
}

std::string CErrorHandler::FormatData(const std::string& rClass,
				  					  const std::string& rMethod,
									  const std::string& rMsgOrError,
									  bool bError)
{
	//Our message
	std::string sMsg;

	//Is it an error?
	if (bError)
		sMsg="*** Error *** ";
	else
		sMsg="###  Msg  ### ";

	//Add the data now
	sMsg+="in "+rClass;
	sMsg+=":"+rMethod;
	sMsg+=" - " + rMsgOrError;

	//Done
	return sMsg;
}

void CErrorHandler::WriteError(const std::string& rClass,
				  			   const std::string& rMethod,
							   const std::string& rError)const
{
	//Do we have a log?
	if (!GetLog())
		return;

	//Format the msg
	std::string sError;
	sError=FormatData(rClass,
					  rMethod,
					  rError);

	//Write it
	GetLog()->WriteError(sError);
}

void CErrorHandler::WriteStaticError(const std::string& rClass,
				  					 const std::string& rMethod,
									 const std::string& rError)
{
	//Do we have a log?
	if (!m_pLog)
		return;

	//Format the msg
	std::string sError;
	sError=FormatData(rClass,
					  rMethod,
					  rError);

	//Write it
	m_pLog->WriteError(sError);
}

void CErrorHandler::WriteMessage(const std::string& rClass,
				  				 const std::string& rMethod,
								 const std::string& rMessage)const
{
	//Do we have a log?
	if (!GetLog())
		return;

	//Format the msg
	std::string sMsg;
	sMsg=FormatData(rClass,
				    rMethod,
					rMessage,
					true);

	//Write it
	GetLog()->WriteMessage(sMsg);
}

std::string CErrorHandler::ErrorCodeToString(DWORD dwErrorCode)
{
	try
	{
		//Try to look it in the errors
		if (!CErrorsRepository::GetInstance().GetErrorsMap().empty())
		{
			//Search
			CErrorsRepository::ErrorMap::const_iterator aIterator;
			aIterator=CErrorsRepository::GetInstance().GetErrorsMap().find(dwErrorCode);

			//Do we have it
			if (aIterator!=CErrorsRepository::GetInstance().GetErrorsMap().end())
				return aIterator->second;
		}

		//Get the error string
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					  FORMAT_MESSAGE_FROM_SYSTEM | 
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL,
					  dwErrorCode,
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					  (LPTSTR) &lpMsgBuf,
					  0,
					  NULL);

		//Save it
		std::string sMessage;

		//Do we have the message?
		if (lpMsgBuf)
		{
			//Save it
			sMessage=(char*)lpMsgBuf;
			
			//Release the buffer
			LocalFree(lpMsgBuf);
		}
		else
			//Failed to find
			sMessage="No error description found!";

		//Done
		return sMessage;
	}
	catch (...)
	{
		return "Unknown";
	}
}

void CErrorHandler::ReportError(const std::string& rMethod,
								const std::string& rMessage,
								int iErrorCode)const
{
	if (!GetLog())
		return;

	try
	{
		//Convert the error code
		char aTmp[11];
		sprintf(aTmp,"%d",iErrorCode);

		//Get the string for it
		std::string sError;
		sError=rMessage;
		sError+=", and Socket error: ";
		sError+=aTmp;
		sError+=", ";
		sError+=ErrorCodeToString(iErrorCode);

		//Report to the log
		WriteError(m_sClassName,
			 	   rMethod,
				   sError);
	}
	ERROR_UNKNOWN("ReportError")
}

void CErrorHandler::ReportError(const std::string& rMethod,
								int iErrorCode)const
{
	if (!GetLog())
		return;

	try
	{
		//Convert the error code
		char aTmp[11];
		sprintf(aTmp,"%d",iErrorCode);

		//Get the string for it
		std::string sError;
		sError="Socket error: ";
		sError+=aTmp;
		sError+=", ";
		sError+=ErrorCodeToString(iErrorCode);

		//Report to the log
		WriteError(m_sClassName,
				   rMethod,
				   sError);
	}
	ERROR_UNKNOWN("ReportError")
}

void CErrorHandler::ReportError(const std::string& rMethod, 
								const std::string& rMessage)const
{
	if (!GetLog())
		return;

	try
	{
		//Report to the log
		WriteError(m_sClassName,
				   rMethod,
				   rMessage);
	}
	ERROR_UNKNOWN("ReportError")
}

void CErrorHandler::ReportStaticErrorOS(const std::string& rClass,
										const std::string& rMethod,
										const std::string& rMessage)
{
	if (!m_pLog)
		return;

	try
	{
		//Get the last error
		DWORD dwLastError;
		dwLastError=GetLastError();

		//Format the message
		std::string sMessage;
		sMessage=rMessage;
		sMessage+=", with error code: ";

		//Convert the error code
		char aTmp[11];
		itoa(dwLastError,aTmp,10);

		//Add it again
		sMessage+=aTmp;
		sMessage+=" ";
		sMessage+=ErrorCodeToString(dwLastError);
		
		//Report to the log
		WriteStaticError(rClass,
					     rMethod,
					     sMessage);
	}
	ERROR_UNKNOWN("ReportStaticError")
}

void CErrorHandler::ReportErrorOS(const std::string& rMethod,
								  const std::string& rMessage)const
{
	if (!GetLog())
		return;

	try
	{
		//Get the last error
		DWORD dwLastError;
		dwLastError=GetLastError();

		//Format the message
		std::string sMessage;
		sMessage=rMessage;
		sMessage+=", with error code: ";

		//Convert the error code
		char aTmp[11];
		itoa(dwLastError,aTmp,10);

		//Add it again
		sMessage+=aTmp;
		sMessage+=" ";
		sMessage+=ErrorCodeToString(dwLastError);
		
		//Get the log
		CErrorLog* pLog;
		pLog=GetLog();

		//Report to the log
		WriteError(m_sClassName,
				   rMethod,
				   sMessage);
	}
	ERROR_UNKNOWN("ReportErrorOS")
}

void CErrorHandler::ReportError(const std::string& rMethod,
								const std::string& rMessage,
								const std::string& rAdditionalData)const
{
	if (!GetLog())
		return;

	try
	{
		//Create the new message
		std::string sNewMessage(rMessage);
		sNewMessage+=", Additional data: ";
		sNewMessage+=rAdditionalData;

		//Report to the log
		WriteError(m_sClassName,
			 	   rMethod,
				   sNewMessage);
	}
	ERROR_UNKNOWN("ReportError")
}

void CErrorHandler::ReportError(const std::string& rMethod,
								const std::string& rMessage,
								DWORD dwAdditionalData)const
{
	if (!GetLog())
		return;

	try
	{
		//Convert the number
		char aTmp[11];
		ltoa(dwAdditionalData,aTmp,10);

		//Create the new message
		std::string sNewMessage(rMessage);
		sNewMessage+=", Additional data: ";
		sNewMessage+=aTmp;

		//Report to the log
		WriteError(m_sClassName,
			 	   rMethod,
				   sNewMessage);
	}
	ERROR_UNKNOWN("ReportError")
}

void CErrorHandler::SetLog(CErrorLog *pLog)
{
	//Save the new log
	m_pLog=pLog;
}

void CErrorHandler::ReportError(const std::string& rMethod)const
{
	if (!GetLog())
		return;

	try
	{
		//Get the last error
		DWORD dwLastError;
		dwLastError=GetLastError();

		//Report the error
		WriteError(m_sClassName,
				   rMethod,
				   ErrorCodeToString(dwLastError));
	}
	ERROR_UNKNOWN("ReportError")
}

CErrorHandler::CErrorLog* CErrorHandler::GetLog()const
{
	//Do we use local or global log?
	if (m_pLocalLog)
		return m_pLocalLog;
	else
		return m_pLog;
}

void CErrorHandler::SetLocalLog(CErrorLog *pLog)
{
	m_pLocalLog=pLog;
}

void CErrorHandler::ReportStaticError(const std::string& rClass,
									  const std::string& rMethod)
{
	if (!m_pLog)
		return;

	try
	{
		//Get the last error
		DWORD dwLastError;
		dwLastError=GetLastError();

		//Report the error
		WriteStaticError(rClass,
						  rMethod,
						  ErrorCodeToString(dwLastError));
	}
	ERROR_UNKNOWN("ReportStaticError")
}

void CErrorHandler::ReportStaticError(const std::string& rClass,
									  const std::string& rMethod,
									  const std::string& rMessage)
{
	if (!m_pLog)
		return;

	try
	{
		//Report to the log
		WriteStaticError(rClass,
						 rMethod,
						 rMessage);
	}
	ERROR_UNKNOWN("ReportStaticError")
}

void CErrorHandler::ReportStaticError(const std::string& rClass,
									  const std::string& rMethod,
									  const std::string& rMessage,
									  DWORD dwAdditionalData)
{
	if (!m_pLog)
		return;

	try
	{
		//Convert the number
		char aTmp[11];
		ltoa(dwAdditionalData,aTmp,10);

		//Create the new message
		std::string sNewMessage(rMessage);
		sNewMessage+=", Additional data: ";
		sNewMessage+=aTmp;

		//Report to the log
		WriteStaticError(rClass,
						 rMethod,
						 sNewMessage);
	}
	ERROR_UNKNOWN("ReportStaticError")
}

const std::string& CErrorHandler::GetClassName()const
{
	return m_sClassName;
}

std::string CErrorHandler::GetCurrentDateTime()
{
	try
	{
		//Our string
		std::string sDate;

		//Our tmp buf
		char cTmp[128];

		//Get date
		_strdate(cTmp);
		sDate=cTmp;
		sDate+=' ';

		//Get time
		_strtime(cTmp);
		sDate+=cTmp;

		//Done
		return sDate;
	}
	ERROR_HANDLER_STATIC_RETURN(CErrorHandler_Class,"GetCurrentDateTime","")
}

void CErrorHandler::RegisterError(DWORD dwErrorCode,
								  const std::string& rDescription)
{
	try
	{
		//Add the error
		CErrorsRepository::GetInstance().GetErrorsMap().insert(CErrorsRepository::ErrorMap::value_type(dwErrorCode,rDescription));
	}
	ERROR_HANDLER_STATIC(CErrorHandler_Class,"RegisterError")
}

KOMODIA_NAMESPACE_END