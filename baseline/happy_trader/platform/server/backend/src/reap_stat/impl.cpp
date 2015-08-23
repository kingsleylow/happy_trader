#include "impl.hpp"
#include "../brklib/brklib.hpp"

// math stuff
#include "../math/math.hpp"

#define REAPSTAT "REAP_STAT_1"



namespace AlgLib {
	
	static id_m = 0;
	
	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new ReapStat(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

	/**
	*/
	// ctor & dtor
	ReapStat::ReapStat( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
	{
	}

	ReapStat::~ReapStat()
	{
	}
	
	// intreface to be implemented
	void ReapStat::onLibraryInitialized(
		Inqueue::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
	{
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  REAPSTAT, "Library Initialized" );
		ALERTEVENTNC(runName, BrkLib::AP_MEDIUM, "ReapStat initialized");
	}

	void ReapStat::onLibraryFreed()
	{
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  REAPSTAT, "Library deinitialized" );
		ALERTEVENTNC("", BrkLib::AP_MEDIUM, "ReapStat deinitialized");
	}

	// this will be called only 
	void ReapStat::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
		

	}


	// note that this is called from any thread which is subscribed as algrorith thread
	// 

	void ReapStat::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{
		// only the one time this is called
		LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  REAPSTAT, "*****On data arrival processing ticker" , rtdata.toString());

		if (historyaccessor.isFirst()) {
			// get one hour
			int symbol_id_min = historyaccessor.cacheSymbol(rtdata.getProvider(), rtdata.getSymbol(), Inqueue::AP_Minute, 1 );
			//int symbol_id_hour = historyaccessor.cacheSymbol(rtdata.getProvider(), rtdata.getSymbol(), Inqueue::AP_Hour, 1 );
			int symbol_id_day = historyaccessor.cacheSymbol(rtdata.getProvider(), rtdata.getSymbol(), Inqueue::AP_Day, 1 );
			
			if (symbol_id_min < 0) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  REAPSTAT, "On data arrival cannot cache data");
				return;
			}

			if (symbol_id_day < 0) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  REAPSTAT, "On data arrival cannot cache data");
				return;
			}

			CppUtils::Value symbol_id_day_v, symbol_id_min_v;
			symbol_id_day_v.getAsInt(true) = symbol_id_day;
			symbol_id_min_v.getAsInt(true) = symbol_id_min;


			getGlobalStorage().putGlobal(CppUtils::String("symb_hour_") + rtdata.getSymbol(), symbol_id_day_v);
			getGlobalStorage().putGlobal(CppUtils::String("symb_min_") + rtdata.getSymbol(), symbol_id_min_v);
		//
		}

	
		if (historyaccessor.isLast()) {
			int symbol_id_min = getGlobalStorage().getGlobal(CppUtils::String("symb_min_") + rtdata.getSymbol()).getAsInt();
			int symbol_id_day = getGlobalStorage().getGlobal(CppUtils::String("symb_hour_") + rtdata.getSymbol()).getAsInt();

			CppUtils::String tmpDir = CppUtils::getTempPath() + "\\stat_dir\\" + rtdata.getSymbol() + "\\";
			CppUtils::makeDir(tmpDir);

			CppUtils::String filename;
			FILE* f = NULL;

			
			CppUtils::String day_string, prev_day_string;

			Inqueue::DataRowPtr rowPtr = historyaccessor.dataRowFirst();
			while(historyaccessor.dataRowNext(rowPtr) ) {
				//HlpStruct::PriceData const & hour_data = historyaccessor.getCurrentDataRow(symbol_id_hour, rowPtr);
				HlpStruct::PriceData const & min_data = historyaccessor.getCurrentDataRow(symbol_id_min, rowPtr);
				HlpStruct::PriceData const & day_data = historyaccessor.getCurrentDataRow(symbol_id_day, rowPtr);

				if (day_data.isValid()) {
					// day switch

					// write
					if (prev_day_string.size() > 0)
						fwrite(prev_day_string.c_str(), prev_day_string.size(), 1, f);
					if (day_string.size() > 0)
						fwrite(day_string.c_str(), day_string.size(), 1, f);

					if (f) {
						fclose(f);
						f=0;
					}

					tm newtime;
					CppUtils::crackGmtTime(day_data.time_m, newtime);
									
					char buffer[256];
					sprintf( buffer, "%04d_%02d_%02d.txt",	newtime.tm_year + 1900, newtime.tm_mon + 1, newtime.tm_mday);
					filename = tmpDir + buffer;
					f = fopen(filename.c_str(), "wt" );

					prev_day_string = day_string;
					day_string.clear();
				}
			
			
				if (min_data.time_m > 0) {
					tm curtime;
					CppUtils::crackGmtTime(min_data.time_m, curtime);

					CppUtils::String s1 = 
						CppUtils::long2String(curtime.tm_year + 1900) +
						"," + CppUtils::long2String(curtime.tm_mon + 1) +
						"," + CppUtils::long2String(curtime.tm_mday) +
						"," + CppUtils::long2String(curtime.tm_hour) +
						"," + CppUtils::long2String(curtime.tm_min) +

						"," + CppUtils::long2String((long)min_data.time_m) + 
						"," + CppUtils::float2String(min_data.open2_m,-1,4) +
						"," + CppUtils::float2String(min_data.high2_m,-1,4) +
						"," + CppUtils::float2String(min_data.low2_m,-1,4) +
						"," + CppUtils::float2String(min_data.close2_m,-1,4) + 
						"," + CppUtils::float2String(min_data.volume_m,-1,4) + "\n";

					day_string += s1;

					
				}

			} // end loop

			if (f) {
				fclose(f);
				f = 0;
			}
		}

		LOGEVENT(HlpStruct::CommonLog::LL_DEBUGDETAIL,  REAPSTAT, "*****On data arrival completed" );
	

	}


	void ReapStat::onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::TradingParameters const& descriptor,
			CppUtils::String const& runName,
			CppUtils::String const& comment
	)
	{
	}

	bool ReapStat::onEventArrived(HlpStruct::EventData const &eventdata, HlpStruct::EventData& response)
	{
		// we may need to pass arbitrary data here
		return false;
	}

	CppUtils::String const& ReapStat::getName() const
	{
		static const CppUtils::String name("Reap Statistics");
		return name;
	}


} // end of namespace