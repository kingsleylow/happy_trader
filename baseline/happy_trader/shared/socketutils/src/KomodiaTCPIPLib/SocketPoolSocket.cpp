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

#include "stdafx.h"
#include "SocketPoolSocket.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"
#include "SocketPool.h"

KOMODIA_NAMESPACE_START

#define CSocketPoolSocket_Class "CSocketPoolSocket"

CSocketPoolSocket::CSocketPoolSocket(CSocketPool* pFather) : CTCPSocketAsync(FALSE),
															 m_usPort(0),
															 m_pFather(pFather)
{		
	try
	{
		//Set our name
		SetName(CSocketPoolSocket_Class);
	}
	ERROR_HANDLER("CSocketPoolSocket")
}

CSocketPoolSocket::~CSocketPoolSocket()
{
}

void CSocketPoolSocket::SetConnectionData(const std::string& rAddress,
										  unsigned short usPort)
{
	//Save the data
	m_sAddress=rAddress;
	m_usPort=usPort;
}

BOOL CSocketPoolSocket::Connect()
{
	try
	{
		//Create ourselves
		if (!Create())
		{
			//Report it
			ReportError("Connect","Failed to create socket!");

			//Exit
			return FALSE;
		}

		if (!m_usPort ||
			m_sAddress.empty())
		{
			//Report it
			ReportError("Connect","No connection data!");

			//Exit
			return FALSE;
		}

		//Try to connect
		return CTCPSocketAsync::Connect(0,
									    m_sAddress.c_str(),
									    m_usPort);
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CSocketPoolSocket::OnSocketConnect(int iErrorCode)
{
	try
	{
		//Are we connected ?
		if (!iErrorCode)
			//Call our father
			m_pFather->SocketConnected(this);
		else
			//Indicate we are closed
			m_pFather->SocketClosed(this);

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("OnSocketConnect",TRUE)
}

CSocketPoolSocket::PoolSocketID CSocketPoolSocket::GetID()const
{
	return 0;
}

BOOL CSocketPoolSocket::OnSocketClose(int iErrorCode)
{
	try
	{
		//Indicate we are closed
		m_pFather->SocketClosed(this);

		//Exit
		return TRUE;
	}
	ERROR_HANDLER_RETURN("OnSocketClose",TRUE)
}

void CSocketPoolSocket::ClosePoolSocket()
{
	try
	{
		//Close ourself
		Close();

		//Notify
		m_pFather->SocketClosed(this);
	}
	ERROR_HANDLER("ClosePoolSocket")
}

KOMODIA_NAMESPACE_END
