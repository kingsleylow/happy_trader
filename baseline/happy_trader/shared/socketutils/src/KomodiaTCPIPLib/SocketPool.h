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

#if !defined(AFX_SOCKETPOOL_H__BB880692_C252_4FBC_8FD2_B89833D0F897__INCLUDED_)
#define AFX_SOCKETPOOL_H__BB880692_C252_4FBC_8FD2_B89833D0F897__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../KomodiaInfraLib/ErrorHandler.h"

#include "SocketPoolSocket.h"

#include <deque>
#include <map>

KOMODIA_NAMESPACE_START

class CGenericCriticalSection;

class CSocketPool : public CErrorHandler
{
	friend class CSocketPoolSocket;
public:
	//Our allocator
	typedef CSocketPoolSocket* (*SocketAllocator)(CSocketPool* pPool);
	typedef SocketAllocator LPSocketAllocator;
public:
	//Add an address to connect to
	BOOL Connect(const std::string& rAddress,
				 unsigned short usPort);

	//Another type of connect
	BOOL Connect(const std::string& rAddress,
				 unsigned short usPort,
				 CSocketPoolSocket* pSocket);

	//Ctor and dtor
	CSocketPool(int iMaxSockets,
				LPSocketAllocator pAllocator,
				int iMaxQueue=200);
	virtual ~CSocketPool();
private:
	//Our queue
	typedef std::deque<CSocketPoolSocket*> SocketQueue;

	//Map of all running sockets
	typedef std::map<CSocketPoolSocket::PoolSocketID,CSocketPoolSocket*> SocketMap;
private:
	//No copy ctor
	CSocketPool(const CSocketPool& rPool);

	//A socket is connection
	void SocketConnected(CSocketPoolSocket* pSocket);

	//A socket is closed
	void SocketClosed(CSocketPoolSocket* pSocket);

	//Check if we have data in queue
	void CheckQueue();

	//Clear all the data
	void Clear();

	//Our number of sockets
	int m_iMaxSocket;

	//Our number of sockets
	int m_iActiveSockets;

	//Set the maximum back queue
	int m_iMaxBackQueue;

	//Our allocator
	LPSocketAllocator m_pAllocator;

	//Our queue
	SocketQueue m_aQueue;

	//Map of the existing sockets
	SocketMap m_aSocketMap;

	//Our CS
	CGenericCriticalSection* m_pCSection;

	//Queue CS
	CGenericCriticalSection* m_pCSectionQueue;

	//Are we closing
	BOOL m_bClosing;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_SOCKETPOOL_H__BB880692_C252_4FBC_8FD2_B89833D0F897__INCLUDED_)
