#ifndef _BRKLIBLASER_LASERSESSION_INCLUDED
#define _BRKLIBLASER_LASERSESSION_INCLUDED

#include "brkliblaserdefs.hpp"


namespace BrkLib {

	class WindowThreadProc;

	// as logging happens upon first initialization 
	// this must be a faked connection which is always connected
	class BRKLIBLASER_EXP LaserSession: public BrokerSession, private CppUtils::Mutex {
		friend class LaserBroker;
	private:

		LaserSession(CppUtils::String const& connectstring, LaserBroker &broker, WindowThreadProc& winthreadproc);

		virtual ~LaserSession();
		
		virtual bool isConnected() const
		{
			return true;
		}

		inline WindowThreadProc& getWinThreadProc()
		{
			return winThreadProc_m;
		}

		inline void registerParentOrder(long orderID, CppUtils::Uid const& uid)
		{
			LOCK_FOR_SCOPE(*this);
			parentOrders_m[orderID] = uid;
		}

	
		inline void removeParentOrder(long orderID)
		{
			LOCK_FOR_SCOPE(*this);
			parentOrders_m.erase(orderID);

		}

		inline CppUtils::Uid const& resolveParentUid(long orderID) const
		{

			static CppUtils::Uid dummy;

			LOCK_FOR_SCOPE(*this);

			map<long, CppUtils::Uid>::const_iterator it = parentOrders_m.find(orderID);

			if (it==parentOrders_m.end())
				return dummy;
			else
				return it->second;

		}

		

		
		
	private:

		WindowThreadProc& winThreadProc_m;

		// this will be populated each time we request new position list
		BrkLib::PositionList positionList_m;

		map<long, CppUtils::Uid> parentOrders_m;


	
	
	};

}; // end of namepsace

#endif