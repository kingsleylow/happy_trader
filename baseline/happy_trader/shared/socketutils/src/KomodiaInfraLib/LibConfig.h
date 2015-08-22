/*
 *
 *
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
 *
 * Contact info:
 * Site: http://www.komodia.com
 * Email: barak@komodia.com
 */

#if !defined(AFX_LIBCONFIG_H__7E8CEE3B_1A67_426D_948B_A8DB6A8184E7__INCLUDED_)
#define AFX_LIBCONFIG_H__7E8CEE3B_1A67_426D_948B_A8DB6A8184E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KomodiaSettings.h"

KOMODIA_NAMESPACE_START

class CLibConfig  
{
public:
	//Get our instance
	static CLibConfig& GetInstance();

	//Get max threads
	unsigned long GetMaxThreads()const;

	//Our Ctor
	~CLibConfig();
private:
	//Private Ctor and Dtor
	CLibConfig();

	//No copy Ctor
	CLibConfig(const CLibConfig& rConfig);

	//No copy operator
	CLibConfig& operator=(const CLibConfig& rConfig);
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_LIBCONFIG_H__7E8CEE3B_1A67_426D_948B_A8DB6A8184E7__INCLUDED_)
