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
#include "SocketPool.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"
#include "../KomodiaInfraLib/OSManager.h"
#include "../KomodiaInfraLib/GenericCriticalSection.h"

KOMODIA_NAMESPACE_START

#define CSocketPool_Class "CSocketPool"

CSocketPool::CSocketPool(int iMaxSockets,
						 LPSocketAllocator pAllocator,
						 int iMaxQueue) : CErrorHandler(),
										  m_iMaxSocket(iMaxSockets),
										  m_pAllocator(pAllocator),
										  m_pCSection(NULL),
										  m_pCSectionQueue(NULL),
										  m_iActiveSockets(0),
										  m_bClosing(FALSE),
										  m_iMaxBackQueue(iMaxQueue)
{
	try
	{
		//Set our name
		SetName(CSocketPool_Class);

		//Create the CS
		m_pCSection=COSManager::CreateCriticalSection();
		m_pCSectionQueue=COSManager::CreateCriticalSection();
	}
	ERROR_HANDLER("CSocketPool")
}

CSocketPool::~CSocketPool()
{
	try
	{
		//Clear the queue
		Clear();

		//Delete the CS
		delete m_pCSection;
		delete m_pCSectionQueue;
	}
	ERROR_HANDLER("~CSocketPool")
}

void CSocketPool::Clear()
{
	try
	{
		//Lock the CS
		CCriticalAutoRelease aRelease(m_pCSection);

		//We are closing
		m_bClosing=TRUE;

		//Lock the map
		CCriticalAutoRelease aRelease2(m_pCSectionQueue);

		//Start to delete the sockets
		SocketMap::iterator aIterator;
		aIterator=m_aSocketMap.begin();

		//Iterate it
		while (aIterator!=m_aSocketMap.end())
		{
			//Delete the socket
			aIterator->second->DeleteSocketFromThread(1000);

			//Next socket
			++aIterator;
		}

		//Wait for all to close
		Sleep(5000);

		//No longer closing
		m_bClosing=FALSE;
	}
	ERROR_HANDLER("Clear")
}

BOOL CSocketPool::Connect(const std::string& rAddress,
						  unsigned short usPort)
{
	try
	{
		//Allocate the socket
		CSocketPoolSocket* pSocket;

		//Do we have an allocator?
		if (!m_pAllocator)
		{
			//Report it
			ReportError("Connect","No allocator!");

			//Exit
			return FALSE;
		}

		//Allocate it
		pSocket=(*m_pAllocator)(this);

		//Try to connect
		return Connect(rAddress,
					   usPort,
					   pSocket);
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CSocketPool::Connect(const std::string& rAddress,
						  unsigned short usPort,
						  CSocketPoolSocket* pSocket)
{
	try
	{
		//Protect the socket
		std::auto_ptr<CSocketPoolSocket> pProtection(pSocket);

		//Are we closing
		if (m_bClosing)
			return TRUE;

		//Set the connection data
		pSocket->SetConnectionData(rAddress,
								   usPort);

		//Lock the CSection
		CCriticalAutoRelease aRelease(m_pCSection);

		//Do we have enough space
		if (m_iActiveSockets>=m_iMaxSocket)
		{
			//Can we add to queue
			if (m_aQueue.size()>m_iMaxBackQueue)
				return FALSE;

			//Add the data to the queue
			m_aQueue.push_back(pSocket);

			//Release the protection
			pProtection.release();

			//Exit
			return TRUE;
		}
		
		//Get the socket
		++m_iActiveSockets;

		//Exit the CS
		aRelease.Exit();

		//Try to connect
		if (!pSocket->Connect())
		{
			//Check the reconnect
			CheckQueue();

			//Exit
			return FALSE;
		}
		else
		{
			//Release the protection
			pProtection.release();

			//Exit
			return TRUE;
		}
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

void CSocketPool::SocketConnected(CSocketPoolSocket* pSocket)
{
	try
	{
		//Add the socket to the map
		CCriticalAutoRelease aRelease(m_pCSectionQueue);

		//Add to the map
		m_aSocketMap.insert(SocketMap::value_type(pSocket->GetID(),
												  pSocket));
	}
	ERROR_HANDLER("SocketConnected")
}

void CSocketPool::SocketClosed(CSocketPoolSocket* pSocket)
{
	try
	{
		{
			//Add the socket to the map
			CCriticalAutoRelease aRelease(m_pCSectionQueue);

			//Add to the map
			m_aSocketMap.erase(pSocket->GetID());

			//Set it to autodelete
			pSocket->DeleteSocketFromThread(1000);
		}

		//Are we closing
		if (!m_bClosing)
			//Check the queue
			CheckQueue();
	}
	ERROR_HANDLER("SocketClosed")
}

void CSocketPool::CheckQueue()
{
	try
	{
		//Lock the CSection
		CCriticalAutoRelease aRelease(m_pCSection);

		//Do we have data
		if (m_aQueue.empty())
			//Decrease the count
			--m_iActiveSockets;
		else
		{
			//Take the socket
			CSocketPoolSocket* pSocket;
			pSocket=m_aQueue.front();

			//Protect the socket
			std::auto_ptr<CSocketPoolSocket> pProtection(pSocket);

			//Remove the front
			m_aQueue.pop_front();

			//Exit the CS
			aRelease.Exit();

			//Try to reconnect
			if (!pSocket->Connect())
				//Recheck the queue
				CheckQueue();
			else
				//Release the protection
				pProtection.release();
		}
	}
	ERROR_HANDLER("CheckQueue")
}

KOMODIA_NAMESPACE_END
