#include "implement.hpp"
#include <algorithm>
#include <numeric>

// Many thanks to Zoubok Victor for programming this

namespace AlgLib {

	
		
					 
	
									  
	// ------------------------

	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new CEventBasedMath(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

	// intreface to be implemented
	CEventBasedMath::CEventBasedMath( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
	{

	}

	CEventBasedMath::~CEventBasedMath()
	{

	}

	void CEventBasedMath::onLibraryInitialized(
		HlpStruct::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		BrkLib::BrokerBase* brokerlib2,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

		
		// clear this event if necessary
		if (loadFinished_m.lock(0))
			loadFinished_m.clearEvent();

		// clear mutex
		if (dataLoadedMtx_m.isLocked())
			dataLoadedMtx_m.unlock();

		CppUtils::String symbolFile = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "SYMBOL_FILE");
		CppUtils::parseSymbolListFile(readSymbols_m,symbolFile);

		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  EVENT_ABSED_MATH, "Number of symbols: " << readSymbols_m.size());

		// provider
		provider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROVIDER");

		// read params
		historyDataProfile_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "HISTORY_DATA_PROFILE");

		// yesterday
		CppUtils::String yesterdaydate_s =	GET_ALG_TRADE_STRING_NR(descriptor.initialAlgBrkParams_m, "YESTERDAY");
		if (yesterdaydate_s.size() > 0) {
			yesterday_m = CppUtils::parseFormattedDateTime("M/d/Y",	yesterdaydate_s	, 0);
		}
		else {
			// automatically

			// today is not monday - 
			double now_t = CppUtils::getTime();
			double today = CppUtils::roundToBeginningOfTheDay(now_t);

			tm now_t_struct;
			CppUtils::crackGmtTime(now_t, now_t_struct);
			yesterday_m = -1;
			if (now_t_struct.tm_wday == 0) {
				// sunday
				yesterday_m = today	- 2*24*60*60;
			}
			else if (now_t_struct.tm_wday == 1) {
				// monday
				yesterday_m = today	- 3*24*60*60;

			}
			else {
				yesterday_m = today	- 1*24*60*60;
			}

		}

		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  EVENT_ABSED_MATH, "Yesterday was resolved to: " << CppUtils::getGmtTime2(yesterday_m));
		
		maxDaysToLoad_m = GET_ALG_TRADE_INT_NR(descriptor.initialAlgBrkParams_m, "MAX_DAYS_LOAD");
	}

	void CEventBasedMath::onLibraryFreed()
	{

	}

	void CEventBasedMath::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
		)
	{

	}

	void CEventBasedMath::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}

	void CEventBasedMath::onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
		)
	{

	}

	void CEventBasedMath::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}

	void CEventBasedMath::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}

	void CEventBasedMath::onEventArrived(
		Inqueue::ContextAccessor& contextaccessor, 
		HlpStruct::CallingContext& callContext,
		bool& result,
		CppUtils::String& customOutputData
		)
	{
	
		// must wait until thread initializes ok
		loadFinished_m.lock();

		HlpStruct::XmlParameter out_param;
		out_param.getName() = "response";

		
		CppUtils::String const& command = callContext.getCallCustomName();

		CppUtils::String const& data = callContext.getParameter<BrkLib::String_Proxy>("custom_data").get();

		// parse as simple structure
		HlpStruct::XmlParameter xmlparam_data;
		HlpStruct::HumanSerializer::deserializeXmlParameter(data, xmlparam_data);

		customOutputData = "";

		if (command=="get_symbols") {
			LOG(MSG_INFO, EVENT_ABSED_MATH, "Requesting symbol list");
			// symbols must be loaded already
		
			
			
			for(map<CppUtils::String, SymbolContext>::const_iterator it = symbols_m.begin(); it != symbols_m.end(); it++) {
				CppUtils::String const& symbol = it->first;
				SymbolContext const& scontext = it->second;
				

				HlpStruct::XmlParameter param_i;
				param_i.getParameter("symbol").getAsString(true) = symbol;
				param_i.getParameter("depth").getAsInt(true) = scontext.daysDepth_m;
				param_i.getParameter("last_day").getAsDouble(true) = scontext.lastDay_m;
				
				out_param.getXmlParameterList("symbols").push_back(param_i);

			}

			LOG(MSG_INFO, EVENT_ABSED_MATH, "Symbol list returned: " << symbols_m.size());

			if (symbols_m.size() == 0) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  EVENT_ABSED_MATH, "Symbol list is empty - cannot be!" );
				THROW(CppUtils::OperationFailed, "exc_SymbolListCannotBeEmpty", "ctx_GetSymbols", "");
			}
			
			
			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  EVENT_ABSED_MATH, "Returning symbols list, size: " << symbols_m.size());
		}
		else if (command=="probability_channel") {
			int depth = atoi(xmlparam_data.getStringParameter("depth").c_str()); 
			double pct_level_min = atof(xmlparam_data.getStringParameter("pct_level_min").c_str()) / 100.0;
			double pct_level_max = atof(xmlparam_data.getStringParameter("pct_level_max").c_str()) / 100.0;
			double open_price = atof(xmlparam_data.getStringParameter("open_price").c_str());

			if (pct_level_min < 0)
				pct_level_min = 0;

			if (pct_level_max < 0)
				pct_level_max = 0;

			if (pct_level_min > 1.0)
				pct_level_min = 1.0;

			if (pct_level_max > 1.0)
				pct_level_max = 1.0;

			CppUtils::String const& symbol = xmlparam_data.getStringParameter("symbol");
			LOG(MSG_INFO, EVENT_ABSED_MATH, "Making calculation: depth= " << depth << " symbol= " << symbol 
				<< " pct level min= " << pct_level_min << " pct level max= " << pct_level_max << " open price: " << open_price );
						
			CppUtils::DoubleList closeDiff, closeOpenDiff, highCloseDiff,lowCloseDiff;

			HlpStruct::PriceData last_data;
			prepareData(contextaccessor, depth, symbol, provider_m, closeDiff, closeOpenDiff, highCloseDiff, lowCloseDiff, last_data); 

			double closeDiff_level_min = -1, closeDiff_level_max=-1,
				closeOpenDiff_level_min=-1, closeOpenDiff_level_max=-1,
				highCloseDiff_level_min=-1, highCloseDiff_level_max=-1,
				lowCloseDiff_level_min=-1, lowCloseDiff_level_max=-1;

							  
			CppUtils::saveContentInFile2( CppUtils::getTempPath() + "\\closeDiff.txt", dumpPrices(closeDiff)); 
			CppUtils::saveContentInFile2( CppUtils::getTempPath() + "\\closeOpenDiff.txt", dumpPrices(closeOpenDiff)); 
			CppUtils::saveContentInFile2( CppUtils::getTempPath() + "\\highCloseDiff.txt", dumpPrices(highCloseDiff)); 
			CppUtils::saveContentInFile2( CppUtils::getTempPath() + "\\lowCloseDiff.txt", dumpPrices(lowCloseDiff)); 

			
			calcLevel(closeDiff, pct_level_min, pct_level_max,closeDiff_level_min, closeDiff_level_max);
			calcLevel(highCloseDiff, pct_level_min, pct_level_max,highCloseDiff_level_min, highCloseDiff_level_max);
			calcLevel(closeOpenDiff, pct_level_min, pct_level_max,closeOpenDiff_level_min, closeOpenDiff_level_max);
			calcLevel(lowCloseDiff, pct_level_min, pct_level_max,lowCloseDiff_level_min, lowCloseDiff_level_max);

			LOG(MSG_INFO, EVENT_ABSED_MATH, "closeDiff_level_min=" << closeDiff_level_min << " closeDiff_level_max=" << closeDiff_level_max );
			LOG(MSG_INFO, EVENT_ABSED_MATH, "highCloseDiff_level_min=" << highCloseDiff_level_min << " highCloseDiff_level_max=" << highCloseDiff_level_max );
			LOG(MSG_INFO, EVENT_ABSED_MATH, "closeOpenDiff_level_min=" << closeOpenDiff_level_min << " closeOpenDiff_level_max=" << closeOpenDiff_level_max );
			LOG(MSG_INFO, EVENT_ABSED_MATH, "lowCloseDiff_level_min=" << lowCloseDiff_level_min << " lowCloseDiff_level_max=" << lowCloseDiff_level_max );

							  
			double closeDiff_h = last_data.close2_m + closeDiff_level_max;
			double closeDiff_l = last_data.close2_m + closeDiff_level_min;
			
			double closeOpenDiff_h = (open_price > 0 ? open_price + closeOpenDiff_level_max : -1.0);
			double closeOpenDiff_l = (open_price > 0 ? open_price + closeOpenDiff_level_min: -1.0);
			
			double highCloseDiff_h  = last_data.close2_m + highCloseDiff_level_max;
			double lowCloseDiff_l = last_data.close2_m + lowCloseDiff_level_min;
			
			

			out_param.getParameter("symbol").getAsString(true)= symbol;
			out_param.getParameter("close_diff_h").getAsDouble(true) = closeDiff_h;
			out_param.getParameter("close_diff_l").getAsDouble(true) = closeDiff_l;

			out_param.getParameter("close_open_diff_h").getAsDouble(true) = closeOpenDiff_h;
			out_param.getParameter("close_open_diff_l").getAsDouble(true) = closeOpenDiff_l;

			out_param.getParameter("low_close_diff_l").getAsDouble(true) = lowCloseDiff_l;
			out_param.getParameter("high_close_diff_h").getAsDouble(true) = highCloseDiff_h;
			
															  
			//LOG(MSG_INFO, EVENT_ABSED_MATH, "Finished calculation: depth= " << depth << " symbol= " << symbol );
			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  EVENT_ABSED_MATH, "Performed calculations for symbol: " << symbol << " depth: " << depth);
		
			//out_param.getParameter("dummy").getAsString(true) = "OK";
		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidCommand", "ctx_onEventArrived", command);

		// no exceptions
		CppUtils::String content;
		HlpStruct::serializeXmlParameter(out_param, content);
		
		customOutputData = CppUtils::hexlify( content );
	}

	void CEventBasedMath::onThreadStarted(
		Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		)
	{

		
		//if (initSymbolsFinished_m.lock(0))
		//	initSymbolsFinished_m.clearEvent();

	
		if (dataLoadedMtx_m.tryLock()) {
			try{
				double beg_time = -1;
				if (maxDaysToLoad_m > 0) {
					beg_time = CppUtils::getTime() - maxDaysToLoad_m * 24 * 60 * 60;
					LOGEVENT(HlpStruct::CommonLog::LL_INFO,  EVENT_ABSED_MATH, "Expected begin date/time on load: " << CppUtils::getGmtTime2(beg_time));
				}
				else {
					LOGEVENT(HlpStruct::CommonLog::LL_INFO,  EVENT_ABSED_MATH, "Load all history data");
				}
				
				//THROW(CppUtils::OperationFailed, "msg", "ctx", "");
				HlpStruct::ProviderSymbolList providerSymbolList;
				for(CppUtils::StringSet::const_iterator it = readSymbols_m.begin(); it != readSymbols_m.end(); it++) {
					HlpStruct::ProviderSymbolPair p( provider_m ,*it);
					providerSymbolList.push_back(p);
				};

				bool loaded = contextaccessor.loadDataFromMaster(providerSymbolList, historyDataProfile_m, false, beg_time, -1);

				if (!loaded) 
					THROW(CppUtils::OperationFailed, "exc_cannotLoadSymbols", "ctx_ThreadStarted", historyDataProfile_m );

				prepareSymbolsListFromCache();
				//LOG(MSG_INFO, EVENT_ABSED_MATH, "Symbols just loaded: " << symbols_m.size());
				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  EVENT_ABSED_MATH, "Loaded history for profile: " << historyDataProfile_m << " size: " << symbols_m.size());
	
			}
			catch(CppUtils::OperationFailed& e)
			{
				//LOG(MSG_ERROR, EVENT_ABSED_MATH, "Exception while load data from profile: " << historyDataProfile_m << " - " << e.message());
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  EVENT_ABSED_MATH, "Exception while load data from profile: " << historyDataProfile_m << " - " << e.message());
			}
			catch(...)
			{
				//LOG(MSG_ERROR, EVENT_ABSED_MATH, "Unknown exception while load data from profile: " << historyDataProfile_m);
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  EVENT_ABSED_MATH, "Unknown exception while load data from profile: " << historyDataProfile_m );

			}


			//CppUtils::sleep(20.0);

			// signal that we can go on
			loadFinished_m.signalEvent();
			
		}
		
		
		
	}

	void CEventBasedMath::onThreadFinished(
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
		)
	{

	}

	CppUtils::String const& CEventBasedMath::getName() const
	{
		static const CppUtils::String name("[ alg lib: CEventBasedMath ]");
		return name;
	}

	/**
	* Helpesr
	*/

	void CEventBasedMath::calculateProbabilityBasedOnLevels(
			CppUtils::String const& symbol, double const &levelhigh, double const& levellow,
			double &problevhigh, double &problevlow	)
	{

	}

	void CEventBasedMath::prepareData(
			Inqueue::ContextAccessor& contextaccessor,
			int const depth,
			CppUtils::String const& symbol, 
			CppUtils::String const& provider,
			CppUtils::DoubleList & closeDiff,
			CppUtils::DoubleList & closeOpenDiff,
			CppUtils::DoubleList & highCloseDiff,
			CppUtils::DoubleList & lowCloseDiff,
			HlpStruct::PriceData& last_data
		)
	{
		HTASSERT(depth > 0);

		closeDiff.clear();
		closeOpenDiff.clear();
		highCloseDiff.clear();
		lowCloseDiff.clear();

		int cache_symbol_id = contextaccessor.cacheSymbol(provider, symbol, HlpStruct::AP_Day, 1);
		if (cache_symbol_id <= 0) {
			THROW(CppUtils::OperationFailed, "exc_CannotCacheSymbol", "ctx_prepareData", provider + " : " + symbol );
		}

		Inqueue::DataPtr ptr= contextaccessor.dataOffsetFromLast( cache_symbol_id, depth+1 );
		
		int cnt = 0;
		HlpStruct::PriceData  pdata_prev;
		while(ptr.isValid())
		{
			HlpStruct::PriceData const&  pdata = contextaccessor.getCurrentPriceData(cache_symbol_id, ptr);
			contextaccessor.dataNext(cache_symbol_id, ptr);

			if (cnt++==0) {
				pdata_prev = pdata;
				continue;
			}

			closeDiff.push_back(pdata.close2_m - pdata_prev.close2_m);
			closeOpenDiff.push_back(pdata.close2_m - pdata.open2_m);
			highCloseDiff.push_back(pdata.high2_m - pdata_prev.close2_m);
			lowCloseDiff.push_back(pdata.low2_m- pdata_prev.close2_m);


			//
			pdata_prev = pdata;

			last_data = pdata;
		}

		if (closeDiff.size() != depth)
			THROW(CppUtils::OperationFailed, "exc_InvalidArrayLength", "ctx_prepareData", provider + " : " + symbol + " - " + CppUtils::long2String(closeDiff.size()) );

		if (!last_data.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidInvalidLastdata", "ctx_prepareData", last_data.toString() );
	}

	void CEventBasedMath::prepareSymbolsListFromCache()
	{
			

			symbols_m.clear();
			vector<Inqueue::CacheFileContext> cachedData;
			CppUtils::getRootObject<Inqueue::HistoryContext>()->getCachedContext(cachedData);

			//LOG(MSG_ERROR, EVENT_ABSED_MATH, "getCachedContext(...) returned: " << cachedData.size() );

			for(int i = 0; i < cachedData.size(); i++) {

				tm time1, time2;
				CppUtils::crackGmtTime(cachedData[i].getEndTime(), time1);
				CppUtils::crackGmtTime(yesterday_m, time2);

				if (time1.tm_mday == time2.tm_mday && time1.tm_mon == time2.tm_mon && time1.tm_year == time2.tm_year ) {
					
					int real_depth =((int)(cachedData[i].getEndTime() - cachedData[i].getBeginTime()))/(60*60*24);
					// inserts
					CppUtils::String const& symbol = cachedData[i].getSymbol();


					SymbolContext ctx;
					ctx.daysDepth_m = real_depth - 1;
					ctx.lastDay_m = cachedData[i].getEndTime();

					symbols_m.insert(pair<CppUtils::String, SymbolContext>(symbol, ctx));
					LOG(MSG_INFO, EVENT_ABSED_MATH, "Symbol: " << symbol << " inserted");
				
				} else {				   
					LOG(MSG_INFO, EVENT_ABSED_MATH, "Symbol: " << cachedData[i].getSymbol() << " discarded");
				}
			}
			
			if (symbols_m.size() != cachedData.size() ) {
				LOGEVENT(HlpStruct::CommonLog::LL_WARN,  EVENT_ABSED_MATH, "Some symbols were disregarded, number of symbols ready: " << symbols_m.size());
			}
			else {
				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  EVENT_ABSED_MATH, "Prepared symbols, number of symbols ready: " << symbols_m.size());
			}
	}

	void CEventBasedMath::createHistohram(CppUtils::IntList& hist, CppUtils::DoubleList const& series,CppUtils::DoubleList const& edges)
	{
		HTASSERT(edges.size() > 0);
		hist.clear();
		hist.resize( edges.size() );
		

		// edges contains the number of bins
		for(int i = 0; i < series.size(); i++) {
			for(int k = 0; k < edges.size(); k++) {
				 //N(k) will count the value X(i) if EDGES(k) <= X(i) < EDGES(k+1)
				if (k < (edges.size()-1))	{
					if (edges[k] <= series[i] && series[i] < edges[k + 1]) {
					 hist[k]++;
					}
				} else {
					// k == edges.size()-1
					if (series[i] ==  edges[k])
						hist[k]++;
				}
			}	
		}
	}

	void CEventBasedMath::createEdges(CppUtils::DoubleList& edges, double const& min_val, double const& max_val, int const count)
	{
		HTASSERT(count > 0);
		edges.resize(count);
		
		int count_m1 = count-1;
		double delta =	(max_val-min_val)/count_m1;
		for(int i = 0; i < count; i++) {
			if (i < count_m1)
				edges[i] = min_val + i*delta;
			else
				edges[i] = max_val; 

		}

	}

	void CEventBasedMath::estimateNumberOfBeans(CppUtils::DoubleList const& series, CppUtils::IntList& histTrg, CppUtils::DoubleList& histEdges)
	{
		double min_val = Math::calcMin(series, 0);
		double max_val = Math::calcMax(series, 0);
		double diff_val = max_val - min_val;
		//LOG(MSG_INFO, EVENT_ABSED_MATH, "Min value: " << min_val << " Max val " << max_val);

		map<int, CppUtils::IntList> allBins;
		map<int, CppUtils::DoubleList> allEdges;
		map<int, double> weights;
		CppUtils::DoubleList weight_val(N_MAX - N_MIN);
		for(int i = N_MIN; i <= N_MAX; i++) {
			
			 int cur_bin_num = i+1;		   

			 // claculate edges
			 CppUtils::DoubleList edges;
			 createEdges(edges,min_val, max_val, cur_bin_num );

			 CppUtils::IntList hist, hist_c;
			 createHistohram(hist, series, edges );

			 // store
			 allBins[cur_bin_num] = hist;
			 allEdges[cur_bin_num] = edges;

			 // resize 
			 hist_c = hist;
			 hist_c.resize(hist_c.size()-1);
			 CppUtils::DoubleList hist_d;

			 CppUtils::copyVectors<int, double>(hist_c,hist_d);
			 double m = Math::calcMavr(	hist_d, 0);

			 
			 double v = 0;
			 for(int g = 0; g < hist_c.size(); g++) {
				  double rf = (hist_c[g] - m);
					//LOG(MSG_INFO, EVENT_ABSED_MATH, "hist_c[g]=" << hist_c[g] << " m=" << m << " i=" << i);
					v += rf*rf;
			 }

			 v = v / ((double)i);

			 double d = diff_val / ((double)i);
			 //
			 double c = (2*m-v) / (d*d);
			 weights[cur_bin_num] = c;

			 //LOG(MSG_INFO, EVENT_ABSED_MATH, "c=" << c << " for " << cur_bin_num << " and m=" << m << " and v=" << v << " and d=" << d);
		}

		// find min
		double min_w_val = -1.0;
		int cur_bin_num = -1;
		for(map<int, double>::iterator it = weights.begin(); it != weights.end(); it++) {
		
			if (min_w_val == -1.0)	{
				min_w_val = it->second;
				cur_bin_num = it->first;
				continue;
			}

			if (min_w_val > it->second) {
				min_w_val = it->second;
				cur_bin_num = it->first;
			}

		}

		map<int, CppUtils::IntList>::iterator it = allBins.find(cur_bin_num);
		if (it == allBins.end())
			THROW(CppUtils::OperationFailed, "exc_CannotFindCalculatedHist", "ctx_EstimateNumberOfBeans", "cur_bin_num");

		// eventually create
		histTrg = it->second;
		histEdges = allEdges[cur_bin_num];
		
	};

	void CEventBasedMath::calcLevel(CppUtils::DoubleList const& series, double const& pct_min, double const& pct_max, double& level_min, double& level_max )
	{
		if (pct_min < 0 || pct_min > 1.0)
			THROW(CppUtils::OperationFailed, "exc_InvalidPctMinLevel", "ctx_calcLevel", CppUtils::float2String(pct_min));

		if (pct_max < 0 || pct_max > 1.0)
			THROW(CppUtils::OperationFailed, "exc_InvalidPctMaxLevel", "ctx_calcLevel", CppUtils::float2String(pct_max));

		CppUtils::IntList hist;
		CppUtils::DoubleList edges;
		estimateNumberOfBeans( series, hist, edges );
		//LOG(MSG_INFO, EVENT_ABSED_MATH, "number of bins: " << hist.size() << " pct Min: " << pct_min << " pct Max: " << pct_max);
		double cummProb  = 0;
		double prevCummProb = 0;
		
		int total = 0;
		for(int i = 0; i < hist.size(); i++) {
			total += hist[i];
		}
		
		bool found_min = false, found_max=false;

		for(int i = 0; i < hist.size(); i++) {
			double curProb = ((double)hist[i]/(double)total);
			cummProb += curProb;
														   
			//LOG(MSG_INFO, EVENT_ABSED_MATH, "cummProb= " << cummProb);

			/*
			
			*/


			
			if (!found_min && cummProb >= pct_min && ( i==0 ? prevCummProb <= pct_min : prevCummProb < pct_min)) {
				// get left border for prevCummProb
				int idx = i - 1;
				if (idx < 0)
					idx = 0;

				level_min = edges[i]; 
				found_min = true;
			}

			if (!found_max && cummProb >= pct_max && ( i==0 ? prevCummProb <= pct_max : prevCummProb < pct_max)) {
				// get right  border for cummProb
				int idx = i + 1;
				if (idx > edges.size())
					idx = edges.size() - 1;

				level_max = edges[i];
				found_max = true;							 
											 
			}		  

			//
			prevCummProb = cummProb;
		}

		// assume the highest
		if (!found_max) {
			level_max = edges[edges.size() - 1];
			found_max = true;	
		}

		if (!found_min) {
			level_min = edges[edges.size() - 1];
			found_min = true;	
		}

		//LOG(MSG_INFO, EVENT_ABSED_MATH, "Min level: " << level_min << " max level: " << level_max);

		/*
		if (!found_min)
			THROW(CppUtils::OperationFailed, "exc_InvalidMinLevel", "ctx_calcLevel", (float)pct_min);

		if (!found_max)
			THROW(CppUtils::OperationFailed, "exc_InvalidMaxLevel", "ctx_calcLevel", (float)pct_max);

		*/
	}

	template <class T>
	void CEventBasedMath::calcLevelFromProbablility(CppUtils::DoubleList const& series, double const& pct, double& level)
	{
		// 
		double min_val = Math::calcMin(series, 0);
		double max_val = Math::calcMax(series, 0);

										  
		//LOG(MSG_INFO, EVENT_ABSED_MATH, "Max: " << max_val << " Min: " << min_val << " pct: " << pct);

		double l = min_val;
		double u = max_val;
		level = -1;
		
		// p_l == 0
		// p_u == 1

		double prev_prob_threshold = -1;
		while( l <= u ) {
			level = (l+u)/ 2.0;
			//LOG(MSG_INFO, EVENT_ABSED_MATH, "Current u: " << u << " l: " << l << " level: " << level );
					
			double prob_threshold = T::calculate(false, level, series);
			if (pct < prob_threshold) {
				u = level;
			}
			else if (pct > prob_threshold) {
				l = level;
			}
			else {
				break;
			}

			//LOG(MSG_INFO, EVENT_ABSED_MATH, "New u: " << u << " l: " << l << " prob_threshold: " << prob_threshold );

			// probability does  not change
			if (prev_prob_threshold > 0 && abs(prev_prob_threshold - prob_threshold) <= 0.0000001)
				break;

			prev_prob_threshold = prob_threshold;
		}

		
		
	}

	double ProbabilityCalculatorLess::calculate(bool const equal, double const thrshold, CppUtils::DoubleList const& series)
	{
		int count = 0;
		for(int i = 0; i < series.size(); i++) {
			//LOG(MSG_INFO, EVENT_ABSED_MATH, "Series i: " << series[i] << " threshold: " << thrshold);
			if (equal) {
				if (series[i] <= thrshold) {
					count++;
				}
			}
			else {
				if (series[i] < thrshold) {
					 count++;
				}
			}
		}
																								 

		//LOG(MSG_INFO, EVENT_ABSED_MATH, "probabilityLess(...) max: " << *(std::max_element( series.begin(), series.end() )) );
		//LOG(MSG_INFO, EVENT_ABSED_MATH, "probabilityLess(...) min: " << *(std::min_element( series.begin(), series.end() )) );
		//LOG(MSG_INFO, EVENT_ABSED_MATH, "probabilityLess(...) count: " << count << " series.size(): " << series.size()
		//	<< "thrshold: " << thrshold	);
		return  ((double) count) / ((double)	series.size()) ;
	}

	double ProbabilityCalculatorMore::calculate(bool const equal, double const thrshold, CppUtils::DoubleList const& series)
	{
		int count = 0;
		for(int i = 0; i < series.size(); i++) {
			if (equal) {
				if (series[i] >= thrshold) {
					count++;
				}
			}
			else {
				if (series[i] > thrshold) {
					 count++;
				}
			}
		}

		return  ((double) count) / ((double)	series.size()) ;
	}
} // end of namespace
