/*
 *  Copyright (c) 2002 Barak Weichselbaum <barak@komodia.com>
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *  If you would like to receive a copy of this library outside the GNU GPL
 *  license, contact us for further details.
 *
 *  Contact info:
 *  Site: http://www.komodia.com
 *  Email: sales@komodia.com
 */

#if !defined(AFX_SOCKETPOOLSOCKET_H__2390326C_7FD3_41A2_AC89_36A6582BEB91__INCLUDED_)
#define AFX_SOCKETPOOLSOCKET_H__2390326C_7FD3_41A2_AC89_36A6582BEB91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TCPSocketAsync.h"

KOMODIA_NAMESPACE_START

class CSocketPool;

class SOCKETUTILS_EXP CSocketPoolSocket : public CTCPSocketAsync
{
public:
	//Our ID
	typedef DWORD PoolSocketID;
public:
	//Get our ID
	PoolSocketID GetID()const;

	//Connect to the destination
	BOOL Connect();

	//Set the connection data
	void SetConnectionData(const std::string& rAddress,
						   unsigned short usPort);

	//Ctor and dtor
	CSocketPoolSocket(CSocketPool* pFather);
	virtual ~CSocketPoolSocket();
protected:
	//Connection event
	virtual BOOL OnSocketConnect(int iErrorCode);

	//Our disconnection event
	virtual BOOL OnSocketClose(int iErrorCode);

	//Close the pool socket
	void ClosePoolSocket();
private:
	//No copy ctor
	CSocketPoolSocket(const CSocketPoolSocket& rSocket);

	//Connection data (where we want to connect to)
	std::string m_sAddress;
	unsigned short m_usPort;

	//Our father
	CSocketPool* m_pFather;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_SOCKETPOOLSOCKET_H__2390326C_7FD3_41A2_AC89_36A6582BEB91__INCLUDED_)
