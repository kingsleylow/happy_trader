#ifndef _MTPROXYDEFS_INCLUDED
#define _MTPROXYDEFS_INCLUDED
//
// This library must be mediator between C++ code and Metatrader
// it will be loadad via MQL script and make communication via pipe
// static linkage to cpputils 


#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (MTPROXY_BCK_EXPORTS)
#				define MTPROXY_EXP __declspec(dllexport)
#			else
#				define MTPROXY_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define MTPROXY_EXP
#		endif		
#		define PROCCALL _stdcall
#	endif

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"

// Rapid XML
#include "../hlpstruct/rapidxml/rapidxml.hpp"
#include "../hlpstruct/rapidxml/rapidxml_iterators.hpp"
#include "../hlpstruct/rapidxml/rapidxml_print.hpp"
#include "../hlpstruct/rapidxml/rapidxml_utils.hpp"

#include "../hlpstruct/json/reader.h"
#include "../hlpstruct/json/writer.h"
#include "../hlpstruct/json/elements.h"




namespace MtProxy {

	class TimeSpan: private CppUtils::Mutex {
	public:
		TimeSpan():
			begin_m(CppUtils::getTime()),
			finished_m(true)
		{
		};

		void start()
		{
			LOCK_FOR_SCOPE(*this);
			begin_m = CppUtils::getTime();
			finished_m = false;
		}

		void stop()
		{
			LOCK_FOR_SCOPE(*this);
			finished_m = true;
	
		}

		bool exceededTime(double const timeThresHold)
		{
			LOCK_FOR_SCOPE(*this);

			// ignore
			if (timeThresHold <=0)
				return false;

			if (finished_m) {
				// already finished
				return false;
			}

			if ((CppUtils::getTime() - begin_m) > timeThresHold)
				return true;
		}

	private:
		double begin_m;
	
		bool finished_m;

	};

	// ---------------------


	/**
	* This queue is not going to be prioritized queue 
	* all elements are inserted to the end
	* we will use vector cause reading this is more probable operation that inserting
	*/
	class BaseQueueElement
	{
	public:

		

		BaseQueueElement():
			lastAccessTime_m(-1)
		{
			id_m = sequence_m;
			sequence_m++;
		}

		virtual ~BaseQueueElement()
		{
		}

		virtual bool equalKey(CppUtils::String const& key) const = 0;
	

		// returns life time in seconds 
		// -1 means infinite - persistent
		// 0 means remove immedaitely after returning
		// other figures means how much seconds to wait
		// after lastAccessTime is updated
		virtual int lifetime() const = 0;

		inline int& lastAccessTime()
		{
			return lastAccessTime_m;
		}

		inline int const& lastAccessTime() const
		{
			return lastAccessTime_m;
		}

		inline int getSequenceNumber() const
		{
			return id_m;
		}

		virtual CppUtils::String toString() const = 0;

	private:

		int lastAccessTime_m;

		int id_m;

		static atomic<int> sequence_m;


	};

	// ------------------------------

	class UserInfoStruct {
		public:

		UserInfoStruct(CppUtils::String const& userP,CppUtils::String const& passP, CppUtils::String const& serverP, int const lifetime ):
			user_m(userP), 
			password_m(passP),
			server_m(serverP),
			lifetime_m(lifetime)
		{
			
		}

		UserInfoStruct():
			lifetime_m(-1)
		{
		}

		~UserInfoStruct()
		{
		}
		
		bool equalKey(CppUtils::String const& key) const
		{
			return (user_m == key);
		}
	

		int lifetime() const
		{
			return lifetime_m;
		}

		
	

		CppUtils::String user_m;
		CppUtils::String password_m;
		CppUtils::String server_m;
		int lifetime_m;
	
	};

	// ------------------------

	class OrderStruct {
		public:
			OrderStruct(CppUtils::String const data):
				data_m(data)
			{
			}

			OrderStruct()
			{
			}

			CppUtils::String toString() const
			{
				return data_m;
			}

			CppUtils::String data_m;
	};

	// ------------------------------

	
	// this keeps general commnands
	class GeneralCommand: public BaseQueueElement
	{
	public:

		// command can be either connect user info or position
		enum CommandType
		{
			CT_ConnectUserInfo = 1,
			CT_Order = 2,
			CT_Dummy  =0
		};

		GeneralCommand():
			type_m(CT_Dummy)
		{
		}

		GeneralCommand( OrderStruct const& order):
			order_m(order),
			type_m(CT_Order)
		{
		};

		GeneralCommand( UserInfoStruct const& ui):
			user_m(ui),
			type_m(CT_ConnectUserInfo)
		{
		};

		virtual ~GeneralCommand()
		{
		}

		CommandType const getType() const
		{
			return type_m;
		};


		OrderStruct & getOrder() {
			return order_m;
		};

		UserInfoStruct& getUserInfo() {
			return user_m;
		};

		UserInfoStruct const& getUserInfo() const{
			return user_m;
		};
		
		// derived
		virtual bool equalKey(CppUtils::String const& key) const
		{
			if (type_m == CT_ConnectUserInfo) {
				return user_m.equalKey(key);
			}
			
			// not impl for others
			return false;
		}

		virtual int lifetime() const
		{
			if (type_m == CT_ConnectUserInfo) {
				return user_m.lifetime();
			}

			return -1;
		}

		virtual CppUtils::String toString() const
		{
			if (type_m == CT_ConnectUserInfo) {
				
				CppUtils::String r;
				r.append("user=").append(user_m.user_m).append("\n ").
				append("lifetime=").append(CppUtils::long2String(user_m.lifetime_m)).append("\n ").
				append("sequence ID=").append(CppUtils::long2String(getSequenceNumber())).append("\n ").
				append("server=").append(user_m.server_m).append("\n ").
				append("last access time=").append(CppUtils::getGmtTime(lastAccessTime()));


				return r;
			}
			else if (type_m == CT_Order) {
				return order_m.toString();
			}

			return "";
		}

		CppUtils::String getStringRepresentation() const
		{
			CppUtils::String r;

			if (type_m == CT_ConnectUserInfo) {
				r.append("CONNECT_USER;").append(user_m.user_m).append(";").append(user_m.password_m);
				r.append(";").append(user_m.server_m).append(";").append(CppUtils::long2String(user_m.lifetime_m));
				
			}
			else if (type_m == CT_Order) {
				r.append("ORDER;").append(order_m.data_m);	
			
			}

			return r;
		}

	private:

		CommandType type_m;
		OrderStruct order_m;
		UserInfoStruct user_m;

	};
	
	
	/*

	// user info periodically provided
	class UserInfo: public BaseQueueElement
	{
	public:

		UserInfo(CppUtils::String const& userP,CppUtils::String const& passP, CppUtils::String const& serverP, int const lifetime ):
			user_m(userP), 
			password_m(passP),
			server_m(serverP),
			lifetime_m(lifetime)
		{
			
		}

		UserInfo():
			lifetime_m(-1)
		{
		}

		virtual ~UserInfo()
		{
		}
		
		virtual bool equalKey(CppUtils::String const& key) const
		{
			return (user_m == key);
		}
	

		virtual int lifetime() const
		{
			return lifetime_m;
		}

		virtual CppUtils::String toString() const
		{
			CppUtils::String r;
			r.append("user=").append(user_m).append("\n ").
				append("lifetime=").append(CppUtils::long2String(lifetime_m)).append("\n ").
				append("sequence ID=").append(CppUtils::long2String(getSequenceNumber())).append("\n ").
				append("server=").append(server_m).append("\n ").
				append("last access time=").append(CppUtils::getGmtTime(lastAccessTime()));


			return r;
		};
	

		CppUtils::String user_m;
		CppUtils::String password_m;
		CppUtils::String server_m;
		int lifetime_m;
	
	};



	typedef vector<UserInfo> UserInfoList;
	typedef set<UserInfo> UserInfoSet;
	typedef map<CppUtils::String, UserInfo> UserInfoMap;

	*/

	typedef vector<GeneralCommand> GeneralCommandList;
	typedef set<GeneralCommand> GeneralCommandSet;
	typedef map<CppUtils::String, GeneralCommand> GeneralCommandMap;

	

};

//
#endif // _MTPROXYDEFS_INCLUDED
