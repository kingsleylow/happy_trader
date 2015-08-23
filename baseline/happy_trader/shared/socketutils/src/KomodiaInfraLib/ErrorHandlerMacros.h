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

#ifndef _ErrorHandlerMacros_H_
#define _ErrorHandlerMacros_H_

//Report a message
#define DbgPrint(METHOD,FORMAT,DATA)	\
	{	\
		char	aTmpBuff[MAX_PATH];	\
		sprintf(aTmpBuff,"Class: %s Method %s ",GetClassName().c_str(),METHOD,FORMAT,DATA); \
		WriteMessage(aTmpBuff);	\
	}

//Report an error
#define DbgError(METHOD,FORMAT,DATA)	\
	{	\
		char	aTmpBuff[MAX_PATH];	\
		sprintf(aTmpBuff,"Class: %s Method %s ",GetClassName().c_str(),METHOD,FORMAT,DATA); \
		WriteError(aTmpBuff);	\
	}

//Report a static message
#define DbgPrintS(CLS,METHOD,FORMAT,DATA)	\
	{	\
		char	aTmpBuff[MAX_PATH];	\
		sprintf(aTmpBuff,"Class: %s Method %s ",CLS,METHOD,FORMAT,DATA); \
		WriteMessage(aTmpBuff);	\
	}

//Report a static error
#define DbgErrorS(CLS,METHOD,FORMAT,DATA)	\
	{	\
		char	aTmpBuff[MAX_PATH];	\
		sprintf(aTmpBuff,"Class: %s Method %s ",CLS,METHOD,FORMAT,DATA); \
		WriteError(aTmpBuff);	\
	}

#ifndef ERROR_HANDLER
#define ERROR_HANDLER(METHOD_NAME)	\
	catch (...)\
	{\
		/*Get the last error*/\
		ReportError(METHOD_NAME);\
	}
#endif

#ifndef ERROR_HANDLER_RETURN
#define ERROR_HANDLER_RETURN(METHOD_NAME,RETURN_VALUE)	\
	catch (...)\
	{\
		/*Get the last error*/\
		ReportError(METHOD_NAME);\
		return RETURN_VALUE;\
	}
#endif

//Manage static handlers
#ifndef ERROR_HANDLER_STATIC
#define ERROR_HANDLER_STATIC(CLASS_NAME,METHOD_NAME)	\
	catch (...)\
	{\
		/*Get the last error*/\
		CErrorHandler::ReportStaticError(CLASS_NAME,METHOD_NAME);\
	}
#endif

#ifndef ERROR_HANDLER_STATIC_RETURN
#define ERROR_HANDLER_STATIC_RETURN(CLASS_NAME,METHOD_NAME,RETURN_VALUE)	\
	catch (...)\
	{\
		/*Get the last error*/\
		CErrorHandler::ReportStaticError(CLASS_NAME,METHOD_NAME);\
		return RETURN_VALUE;\
	}
#endif

//Unknown error
#ifndef ERROR_UNKNOWN
#define ERROR_UNKNOWN(METHOD_NAME) \
	catch (...)\
	{\
		/*Can't do anything to avoid circular catch*/\
		assert(0);\
	}
#endif

//Unknown error
#ifndef ERROR_UNKNOWN_RETURN
#define ERROR_UNKNOWN_RETURN(METHOD_NAME,RETURN_VALUE) \
	catch (...)\
	{\
		/*Can't do anything to avoid circular catch*/\
		assert(0);\
		return RETURN_VALUE;\
	}
#endif

//Rethrow macros
#ifndef ERROR_HANDLER_RETHROW
#define ERROR_HANDLER_RETHROW(METHOD_NAME)	\
	catch (...)\
	{\
		/*Get the last error*/\
		ReportError(METHOD_NAME);\
		throw;\
	}
#endif

//Manage static handlers
#ifndef ERROR_HANDLER_STATIC_RETHROW
#define ERROR_HANDLER_STATIC_RETHROW(CLASS_NAME,METHOD_NAME)	\
	catch (...)\
	{\
		/*Get the last error*/\
		CErrorHandler::ReportStaticError(CLASS_NAME,METHOD_NAME);\
		throw;\
	}
#endif


//Unknown error
#ifndef ERROR_UNKNOWN_RETHROW
#define ERROR_UNKNOWN_RETHROW(METHOD_NAME) \
	catch (...)\
	{\
		/*Can't do anything to avoid circular catch*/\
		assert(0);\
		throw;\
	}
#endif

#endif