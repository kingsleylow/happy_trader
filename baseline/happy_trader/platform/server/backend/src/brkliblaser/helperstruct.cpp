#include "helperstruct.hpp"
#include "laserexcept.hpp"

namespace BrkLib {

	#define LASERAPISETTINGS "LASERAPISETTINGS"
	#define TRADER_SEC "Trader"

	LaserApiSettings::LaserApiSettings():
		loginTimeoutSec_m(60),
		laserPrintsOnly_m(false),
		connectLevel1_m(true),
		connectLevel2_m(true),
		connectChart_m(true),
		enableLooping_m(true)
	{
	}
		
	LaserApiSettings::~LaserApiSettings()
	{
	}

	void LaserApiSettings::initialize(
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
	)
	{
		LOCK_FOR_SCOPE(*this);
		
		enableLooping_m = true;

		if (!CppUtils::fileExists(iniPath))
			THROW(LaserFailed, "exc_IniFileNotExists", "ctx_initialize", iniPath);
		
		CppUtils::CIniReader iniReader(iniPath);

		// get all section names
		CppUtils::StringList secNamesList;
		iniReader.getSectionNames( secNamesList );

		
		// read executor
		if (iniReader.sectionExists(TRADER_SEC)) {

			// executor
			ServerAddress sa(
				iniReader.getKeyValue("ExecutorServerAddress", TRADER_SEC),
				iniReader.getKeyValue("ExecutorServerService", TRADER_SEC),
				"1" // only one
			);
			
			executors_m.insert(sa); 
			currentExecutor_m = sa;

			// chart servers - read as much as possible
			CppUtils::String baseServerName("ChartServerAddress");
			CppUtils::String basePortName("ChartServerService");

			int i = 0;
			do{

				CppUtils::String seq_id = CppUtils::long2String(i);
				
				CppUtils::String srvName = baseServerName + seq_id;
				CppUtils::String portName = basePortName +seq_id;
				
				CppUtils::String chartAddress = iniReader.getKeyValue(srvName, TRADER_SEC);
				CppUtils::String chartPort = iniReader.getKeyValue(portName, TRADER_SEC);

				if (chartAddress.size() > 0 && chartPort.size() > 0) {
					ServerAddress sa(chartAddress, chartPort, CppUtils::long2String(i+1));
					chartServers_m.insert(sa);

					if (i==0) 
						currentChartServer_m = sa;
					
				} else {
					break;
				}

				i++;

			} while( true );
  

			// quote servers list
			CppUtils::String quoteSrvListStr = iniReader.getKeyValue("QuoteServers", TRADER_SEC);
			currentQuoteServerKey_m = iniReader.getKeyValue("QuoteServerSelected", TRADER_SEC);
			CppUtils::trimBoth( currentQuoteServerKey_m );
		


			CppUtils::StringList quoteSrvList;
			CppUtils::parseCommaSeparated(',', quoteSrvListStr, quoteSrvList);
			for(int i = 0; i < quoteSrvList.size(); i++) {
				
				CppUtils::trimBoth( quoteSrvList[i] );
							
				if (quoteSrvList[i].size() > 0) {
					QuoteServerEntry qse(quoteSrvList[i], currentQuoteServerKey_m == quoteSrvList[i]);
					quoteServers_m[  quoteSrvList[i] ] = qse;
				}


			}

			// get quote server details
			for(int i = 0; i < secNamesList.size(); i++) {
				CppUtils::String const& sectionName = secNamesList[i];
				
				for(map<CppUtils::String, QuoteServerEntry>::iterator it = quoteServers_m.begin(); it != quoteServers_m.end(); it++) {
					CppUtils::String const& kkey = it->first;
					if (sectionName.find(kkey) != CppUtils::String::npos) {
						
						// read all values from that section
						QuoteServerEntry &qse = it->second;		  

						ServerEntry se(
							iniReader.getKeyValue("SymbolBegin", secNamesList[i]) + "-" +iniReader.getKeyValue("SymbolEnd", sectionName),
							ServerAddress(iniReader.getKeyValue("L1Address", secNamesList[i]),iniReader.getKeyValue("L1Port", sectionName), kkey),
							ServerAddress(iniReader.getKeyValue("L2Address", secNamesList[i]),iniReader.getKeyValue("L2Port", sectionName), kkey)
						);

						
						CppUtils::StringList s1;
						CppUtils::parseCommaSeparated(' ', sectionName, s1);
						if (s1.size() == 2) {
							CppUtils::String quoteName = s1[1];
							CppUtils::trimBoth( quoteName );

							qse.serverMap_m[quoteName] = se;
						}
		
					}
				}
			}

  
		}
		else 
			THROW(LaserFailed, "exc_CannotReadLaserServersIniSection", "ctx_initialize", TRADER_SEC);

		laserUser_m = user;
		laserPassword_m = password;
		loginTimeoutSec_m = logTimeoutSec;
		if (loginTimeoutSec_m < 0)
			loginTimeoutSec_m = 60;

		laserPrintsOnly_m = laserPrintsOnly;
		connectLevel1_m = connectLevel1;
		connectLevel2_m = connectLevel2;
		connectChart_m = connectChart;
		providerForRT_m = providerForRT;
		symbolList_m = symbolList;
	}

	// --------------

	ServerAddress LaserApiSettings::getCurrentChartServer() const
	{
		LOCK_FOR_SCOPE(*this);
		return currentChartServer_m;
	}

	LaserApiSettings::QuoteServerEntry LaserApiSettings::getCurrentQuoteEntry() const
	{
		LOCK_FOR_SCOPE(*this);

		map<CppUtils::String, QuoteServerEntry>::const_iterator it = quoteServers_m.find(currentQuoteServerKey_m);
		if (it != quoteServers_m.end())
			return it->second;

		THROW(LaserFailed, "exc_CannotLocateQuoteServerEntry", "ctx_getCurrentQuoteEntry", currentQuoteServerKey_m);
	}

	ServerAddress LaserApiSettings::getCurrentExecutor() const
	{
		LOCK_FOR_SCOPE(*this);

		return currentExecutor_m;
	}


	
	// ------------

	CppUtils::String LaserApiSettings::getUser() const 
	{
		LOCK_FOR_SCOPE(*this);

		return laserUser_m;
	}

	CppUtils::String LaserApiSettings::getPassword()  const
	{
		LOCK_FOR_SCOPE(*this);

		return laserPassword_m;
	}

	int LaserApiSettings::getLoginTimeout() const
	{
		LOCK_FOR_SCOPE(*this);

		return loginTimeoutSec_m;
	}

	bool LaserApiSettings::isPrintsOnly() const 
	{
		LOCK_FOR_SCOPE(*this);

		return laserPrintsOnly_m;
	}
	
	bool LaserApiSettings::isConnectLevel1() const 
	{
		LOCK_FOR_SCOPE(*this);

		return connectLevel1_m;
	}

	bool LaserApiSettings::isConnectLevel2() const 
	{
		LOCK_FOR_SCOPE(*this);

		return connectLevel2_m;
	}

	bool LaserApiSettings::isConnectChart() const 
	{
		LOCK_FOR_SCOPE(*this);

		return connectChart_m;
	}

	CppUtils::String LaserApiSettings::getProviderForRT() const 
	{
		LOCK_FOR_SCOPE(*this);

		return providerForRT_m;
	}

	CppUtils::StringList LaserApiSettings::getSymbolList() const
	{
		LOCK_FOR_SCOPE(*this);

		return symbolList_m;
	}


		// thse functions changes current ptrs so using the 2 past functions u would be able to locate current. If list is finishes it returs
		// back to zero
	void LaserApiSettings::loopChartServers()
	{
		LOCK_FOR_SCOPE(*this);

		if (!enableLooping_m)
			return;

		set<ServerAddress>::const_iterator it = chartServers_m.find(currentChartServer_m);
		if (it != chartServers_m.end()) {
			it++;
			if (it == chartServers_m.end()) 
				it = chartServers_m.begin();

			currentChartServer_m = *it;
		}
		else {
			THROW(LaserFailed, "exc_CannotLocateChartServerEntry", "ctx_loopChartServers", currentChartServer_m.toString());
		}
	}

	void LaserApiSettings::loopQuoteEntries()
	{
		LOCK_FOR_SCOPE(*this);

		if (!enableLooping_m)
			return;

		map<CppUtils::String, QuoteServerEntry>::iterator it = quoteServers_m.find(currentQuoteServerKey_m);
		if (it != quoteServers_m.end()) {
			it++;
			if (it == quoteServers_m.end()) 
				it = quoteServers_m.begin();

			currentQuoteServerKey_m = it->first;
			it->second.isSelected_m = true;
		}
		else {
			THROW(LaserFailed, "exc_CannotLocateQuoteServerEntry", "ctx_loopQuoteEntries", currentQuoteServerKey_m);
		}
	}

	void LaserApiSettings::loopExecutor()
	{
		LOCK_FOR_SCOPE(*this);

		if (!enableLooping_m)
			return;

		set<ServerAddress>::const_iterator it = executors_m.find(currentExecutor_m);
		if (it != executors_m.end()) {
			it++;
			if (it == executors_m.end()) 
				it = executors_m.begin();

			currentExecutor_m = *it;
		}
		else {
			THROW(LaserFailed, "exc_CannotLocateExecutorEntry", "ctx_loopExecutor", currentExecutor_m.toString());
		}
	}

	void LaserApiSettings::enableLoopEntries(bool const isEnable)
	{
		LOCK_FOR_SCOPE(*this);

		enableLooping_m = isEnable;
	}

	// -----------------------------
	// helpers

	void LaserApiSettings::parseIpString(CppUtils::String const& str, CppUtils::String& host, int& port)
	{
			host.clear();
			port = -1;

			size_t idx = str.find(':');

			if (idx >=0) {
				host = str.substr(0, idx);
				port = atoi(str.substr(idx+1).c_str());
			}

	}




};
