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

#if !defined(AFX_ERRORHANDLER_H__B886D5A4_FD2D_4CB8_8459_7311CBE31B77__INCLUDED_)
#define AFX_ERRORHANDLER_H__B886D5A4_FD2D_4CB8_8459_7311CBE31B77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../socketutilsdefs.hpp"
#include "KomodiaSettings.h"

#include <string>
#include <map>

KOMODIA_NAMESPACE_START

class SOCKETUTILS_EXP CErrorHandler  
{
public:
	//The external log
	class CErrorLog
	{
		friend class CErrorHandler;
	public:
		//Do we auto flush?
		//If so, will try to flush medium after each write
		void SetAutoFlush(BOOL bFlush);
		BOOL GetAutoFlush()const;

		//Assignment operator
		CErrorLog& operator=(const CErrorLog& rLog);

		//Ctor and Dtor
		CErrorLog();
		CErrorLog(const CErrorLog& rLog);
		virtual ~CErrorLog();
	protected:
		//Report an error must overide
		virtual void WriteError(const std::string& rError)=0;

		//Report a regular message
		virtual void WriteMessage(const std::string& rMessage)=0;
	private:
		//Do we auto flush
		BOOL m_bAutoFlush;
	};
public:
	//Get a formated time and date
	static std::string GetCurrentDateTime();

	//Convert an error code to string
	static std::string ErrorCodeToString(DWORD dwErrorCode);

	//Register an error
	static void RegisterError(DWORD dwErrorCode,
							  const std::string& rDescription);

	//Set the local log (will be used in current class only)
	void SetLocalLog(CErrorLog* pLog);

	//Set the global log
	static void SetLog(CErrorLog* pLog);

	//Report an error
	void WriteError(const std::string& rClass,
				  	const std::string& rMethod,
					const std::string& rError)const;

	//Report an error (to static log)
	static void WriteStaticError(const std::string& rClass,
				  				 const std::string& rMethod,
								 const std::string& rError);

	//Report a regular message
	void WriteMessage(const std::string& rClass,
				  	  const std::string& rMethod,
					  const std::string& rMessage)const;

	//Format our message
	static std::string FormatData(const std::string& rClass,
				  				  const std::string& rMethod,
								  const std::string& rMsgOrError,
								  bool bError=true);

	//Ctor and Dtor
	CErrorHandler();
	CErrorHandler(const CErrorHandler& rHandler);
	virtual ~CErrorHandler();
protected:
	//Get the name of the current class
	const std::string& GetClassName()const;

	//Various routines to report errors to save the need to use of the macro
	//Report an unknown error (use GetLastError)
	static void ReportStaticError(const std::string& rClass,
								  const std::string& rMethod);

	//Report an error
	static void ReportStaticError(const std::string& rClass,
								  const std::string& rMethod,
								  const std::string& rMessage);

	//Report an error
	static void ReportStaticError(const std::string& rClass,
								  const std::string& rMethod,
								  const std::string& rMessage,
								  DWORD dwAdditionalData);

	//Report an error
	static void ReportStaticErrorOS(const std::string& rClass,
									const std::string& rMethod,
									const std::string& rMessage);

	//Report an unknown error (use GetLastError)
	virtual void ReportError(const std::string& rMethod)const;

	//Report an error
	virtual void ReportError(const std::string& rMethod,
							 const std::string& rMessage)const;

	//Report an error
	virtual void ReportError(const std::string& rMethod,
							 const std::string& rMessage,
							 DWORD dwAdditionalData)const;

	//Report an error
	virtual void ReportError(const std::string& rMethod,
							 const std::string& rMessage,
							 const std::string& rAdditionalData)const;

	//Report an error, with socket error
	virtual void ReportError(const std::string& rMethod,
							 int iErrorCode)const;

	//Report an error, with socket error
	virtual void ReportError(const std::string& rMethod,
							 const std::string& rMessage,
							 int iErrorCode)const;

	//Report an error (Take the get last error)
	virtual void ReportErrorOS(const std::string& rMethod,
							   const std::string& rMessage)const;

	//Set the name of the current class
	void SetName(const std::string& rName)const;
private:
	//Get the correct log (global or local)
	CErrorLog* GetLog()const;

	//Our log
	static CErrorLog* m_pLog;

	//If we have a local log
	CErrorLog* m_pLocalLog;

	//Our class name
	mutable std::string m_sClassName;
private:
	//This class is to avoid the static init order problem
	class CErrorsRepository
	{
	public:
		//Our map of errors
		typedef std::map<DWORD,std::string> ErrorMap;
	public:
		//Get the map
		ErrorMap& GetErrorsMap();

		//Get our instance
		static CErrorsRepository& GetInstance();
	private:
		//Our error map
		 ErrorMap m_aErrorMap;
	};
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_ERRORHANDLER_H__B886D5A4_FD2D_4CB8_8459_7311CBE31B77__INCLUDED_)
