#ifndef _BRKLIBLASER_HELPERSTRUCT_INCLUDED
#define _BRKLIBLASER_HELPERSTRUCT_INCLUDED

#include "brkliblaserdefs.hpp"
#include "laserexcept.hpp"	

namespace BrkLib {

	struct ServerAddress
		{
		public:

			ServerAddress(CppUtils::String const& ipAddress, int const port, CppUtils::String const& id):
				port_m(port),
				id_m(id)
			{
				CppUtils::String s(ipAddress);
				CppUtils::trimBoth(s);
				address_m = s;

			}

			ServerAddress(CppUtils::String const& ipAddress, CppUtils::String const& port, CppUtils::String const& id):
				port_m(atoi(port.c_str())),
				id_m(id)
			{
				CppUtils::String s(ipAddress);
				CppUtils::trimBoth(s);
				address_m = s;
			}

			ServerAddress():
				port_m(-1)
			{
			}

			inline CppUtils::String const& getIpAddress() const
			{
				return address_m;
			}

			inline int const getPort() const
			{
				return port_m;
			}

			void setup(CppUtils::String const& serverAddress, int const port)
			{
				CppUtils::String s(serverAddress);
				CppUtils::trimBoth(s);
				address_m = s;

				port_m = port;
			}

			void setup(CppUtils::String const& serverAddress, CppUtils::String const& port)
			{
				CppUtils::String s(serverAddress);
				CppUtils::trimBoth(s);
				address_m = s;

				port_m = atoi(port.c_str());
			}

			CppUtils::String const& getId() const
			{
				return id_m;
			}


			CppUtils::String toString() const
			{
				if (port_m > 0)
					return address_m + ":" + CppUtils::long2String(port_m);
				else 
					return "Not available";
			}

			bool operator < (ServerAddress const& rhs) const
			{
				return this->toString() < rhs.toString();
			}


		private:

			CppUtils::String address_m;

			CppUtils::String id_m;

			int port_m;
		};

	/**
	* This class reads genesis.ini and on disconnection set up the next server
	*/
	class LaserApiSettings: private CppUtils::Mutex {

	public:

		
		struct ServerEntry
		{
			ServerEntry()
			{
			}

			ServerEntry(
				CppUtils::String const& symbolRange, 
				ServerAddress const& l1addr, 
				ServerAddress const& l2addr 
			):
				symbolRange_m(symbolRange),
				L1_address_m(l1addr),
				L2_address_m(l2addr)
			{
			}

			CppUtils::String symbolRange_m;
			ServerAddress L1_address_m;
			ServerAddress L2_address_m;
		};


		struct QuoteServerEntry {

			QuoteServerEntry(CppUtils::String  const& keyName, bool const isSelected):
				keyName_m(keyName),
				isSelected_m( isSelected )
			{
			}

			QuoteServerEntry()
			{
			}


			// for multithreading return only instance, not reference
			ServerEntry getEntry(CppUtils::String const& key) const
			{
				map<CppUtils::String, ServerEntry>::const_iterator it = serverMap_m.find(key);
				if (it == serverMap_m.end())
					THROW(LaserFailed, "exc_CannotLocateQuoteServerEntry", "ctx_getEntry", key);

				return it->second;
			}

			CppUtils::String keyName_m;
			bool isSelected_m;
			
			//
			map<CppUtils::String, ServerEntry> serverMap_m;

		};
		
		// ctor & dtor
		LaserApiSettings();
		
		~LaserApiSettings();

		void initialize(
			CppUtils::String const& iniPath,
			CppUtils::String const& user,
			CppUtils::String const& password,
			int const logTimeoutSec,
			bool const laserPrintsOnly,
			bool const connectLevel1,
			bool const connectLevel2,
			bool const connectChart,
			CppUtils::String const& providerForRT,
			CppUtils::StringList const &symbolList
		);

	
		

		// 
		ServerAddress getCurrentChartServer() const;

		QuoteServerEntry getCurrentQuoteEntry() const;

		ServerAddress getCurrentExecutor() const;
		//

		
		//

		CppUtils::String getUser() const ;
		
		CppUtils::String getPassword()  const;
		
		int getLoginTimeout() const;
		
		bool isPrintsOnly() const ;
		
		bool isConnectLevel1() const ;
		
		bool isConnectLevel2() const ;
		
		bool isConnectChart() const ;
		
		CppUtils::String getProviderForRT() const ;
		
		CppUtils::StringList getSymbolList() const;

		// may stop looping and just recconect with our server
		void enableLoopEntries(bool const isEnable);
		
		


		// thse functions changes current ptrs so using the 2 past functions u would be able to locate current. If list is finishes it returs
		// back to zero
		void loopChartServers();

		void loopQuoteEntries();

		void loopExecutor();


	private:

		void parseIpString(CppUtils::String const& str, CppUtils::String& host, int& port);

		set<ServerAddress> chartServers_m;

		set<ServerAddress> executors_m;

		map<CppUtils::String, QuoteServerEntry> quoteServers_m;

		// currents
		ServerAddress currentExecutor_m;

		ServerAddress currentChartServer_m;

		CppUtils::String currentQuoteServerKey_m;

	

		//

		CppUtils::String laserUser_m;

		CppUtils::String laserPassword_m;

		CppUtils::String providerForRT_m;

		CppUtils::StringList symbolList_m;

		// flags 
		int loginTimeoutSec_m;

		bool laserPrintsOnly_m;

		bool connectLevel1_m;

		bool connectLevel2_m;

		bool connectChart_m;

		bool enableLooping_m;

	};
};

#endif