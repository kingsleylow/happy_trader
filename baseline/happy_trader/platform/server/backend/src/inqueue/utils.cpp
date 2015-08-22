#include "utils.hpp"
#include "histcontext.hpp"
#include "enginethread.hpp"
#include "serversocketacceptor.hpp"


#define INQUEUEUTILS "INQUEUEUTILS"

namespace Inqueue {

	void requestRunExternalPackageHelper(
		CppUtils::String const& package_name,
		bool const do_not_wait_response,
		bool &result,
		CppUtils::String& returned_log
	)
	{
		LOG(MSG_INFO, INQUEUEUTILS, "requestRunExternalPackageHelper(...) starting...");

		CTCPSocket clientSocket;

		result = false;
		long cnt = 0;

		returned_log.clear();
		try {

			Inqueue::HistoryContext* hstcontext = CppUtils::getRootObject<Inqueue::HistoryContext>();

			if (!clientSocket.Create()) {
				THROW(CppUtils::OperationFailed, "exc_CannotCreateSocket", "ctx_requestRunExternalPackage", CTCPSocket::ErrorCodeToString(clientSocket.GetLastError()));
			}

			if (!clientSocket.Connect(CppUtils::getRootObject<Inqueue::ServerSocketController>()->getServiceHost(), CppUtils::getRootObject<Inqueue::ServerSocketController>()->getServivePort())) {
				THROW(CppUtils::OperationFailed, "exc_CannotConnectToService", "ctx_requestRunExternalPackage", CTCPSocket::ErrorCodeToString(clientSocket.GetLastError()));
			}


			HlpStruct::XmlParameter request_param;
		
			request_param.getParameter("package_name").getAsString(true) = package_name;
			request_param.getParameter("return_immedaitely").getAsInt(true) = (do_not_wait_response ? 1:0);
			
			// send
			HtInquotePacket out_packet;
			HtInquotePacket in_packet;
			HlpStruct::EventData eventData;
			HlpStruct::ParametersHandler paramsProc;
			HlpStruct::XmlParameter request;

			out_packet.flag_m = HtInquotePacket::FLAG_COMMON_SERVICE_REQUEST_EXT_PACKAGE_RUN;

			HlpStruct::serializeXmlParameter(request_param, out_packet.data_m );
			writeToSocketHtInquotePacket(clientSocket, out_packet);
					

			// receive

			while (true) {
				if (!readFromSocketHtInquotePacket<CTCPSocket>(clientSocket, in_packet) )
					THROW(CppUtils::OperationFailed, "exc_CannotReadFromSocket", "ctx_requestRunExternalPackage", CTCPSocket::ErrorCodeToString(clientSocket.GetLastError()));					


				if (in_packet.flag_m == HtInquotePacket::FLAG_TRANSMITION_FINISH) {
						
						break;
				}
				else if (in_packet.flag_m == HtInquotePacket::FLAG_XMLEVENT_PACKET) {
							
						paramsProc.deserializeXmlParameter(request, in_packet.data_m);
						HlpStruct::convertXmlParameterToEvent(eventData, request);

						returned_log.append( eventData.getAsString() );
						LOG(MSG_DEBUG, INQUEUEUTILS, "Log line from external package run service returned: " << eventData.getAsString());

				}
				else
					THROW(CppUtils::OperationFailed, "exc_InvalidPacketFlag", "ctx_requestRunExternalPackage", (long)in_packet.flag_m );


			}
			

			// result is true
			result = true;

		}
		catch(CppUtils::Exception &e)
		{
			LOG(MSG_ERROR, INQUEUEUTILS, "Exception on requestRunExternalPackageHelper(...) " << e.message() << ", so far loaded: " << cnt);
		}
		catch(...)
		{
			LOG(MSG_ERROR, INQUEUEUTILS, "Unknown exception on requestRunExternalPackageHelper(...), so far loaded: " << cnt);
		}

		// finished, may close
		try {
			clientSocket.Close();
			clientSocket.Shutdown(CSocket::ssBoth);
		}
		catch(...)
		{
		}

		LOG(MSG_INFO, INQUEUEUTILS, "requestRunExternalPackageHelper(...) finished");
		
	}

	bool loadDataFromMasterHelper(
		HlpStruct::ProviderSymbolList const& providerSymbolList,
		CppUtils::String const& dataProfile,
		bool const keepCacheAlive,
		double const& beginTime,
		double const& endTime
		)
	{
		LOG(MSG_INFO, INQUEUEUTILS, "loadDataFromMasterHelper(...) starting...");

		// somehow read these symbols

		// iterate through all received list, and firstly 
		// hstContext_m.clearHistoryCache(provider, symbol);

		// hstContext_m.passHistoryChunk(rtdataList);

		CTCPSocket clientSocket;

		bool result = false;
		long cnt = 0;

		try {

			Inqueue::HistoryContext* hstcontext = CppUtils::getRootObject<Inqueue::HistoryContext>();

			if (!clientSocket.Create()) {
				THROW(CppUtils::OperationFailed, "exc_CannotCreateSocket", "ctx_loadDataFromMaster", CTCPSocket::ErrorCodeToString(clientSocket.GetLastError()));
			}

			if (!clientSocket.Connect(CppUtils::getRootObject<Inqueue::ServerSocketController>()->getServiceHost(), CppUtils::getRootObject<Inqueue::ServerSocketController>()->getServivePort())) {
				THROW(CppUtils::OperationFailed, "exc_CannotConnectToService", "ctx_loadDataFromMaster", CTCPSocket::ErrorCodeToString(clientSocket.GetLastError()));
			}


			HlpStruct::XmlParameter request_param;
			if (beginTime > 0)
				request_param.getParameter("begin_date").getAsDate(true) = beginTime * 1000.0;
			else
				request_param.getParameter("begin_date").getAsDate(true)  =-1;

			if (endTime > 0)
				request_param.getParameter("end_date").getAsDate(true) = endTime * 1000.0;
			else
				request_param.getParameter("end_date").getAsDate(true)  =-1;

			request_param.getParameter("profile_name").getAsString(true) = dataProfile;
			HlpStruct::XmlParameter::XmlParameterList paramlist;
			for(int i = 0; i < providerSymbolList.size(); i++) {
				HlpStruct::ProviderSymbolPair const& pair_i = providerSymbolList[i];

				HlpStruct::XmlParameter param_i;
				param_i.getParameter("provider").getAsString(true) = pair_i.provider_m;
				param_i.getParameter("symbol").getAsString(true) = pair_i.symbol_m;

				paramlist.push_back(param_i);

				// clear old cache entries
				hstcontext->clearHistoryCache(pair_i.provider_m, pair_i.symbol_m );
				hstcontext->setReuseCacheFlag(pair_i.provider_m, pair_i.symbol_m, keepCacheAlive);
			}

			if (providerSymbolList.size() == 0) {
				hstcontext->clearHistoryCache();
				hstcontext->setReuseCacheFlag(keepCacheAlive);
			}


			request_param.getXmlParameterList("provider_symbol_list") = paramlist;


			// send
			HtInquotePacket out_packet;
			HtInquotePacket in_packet;
			HlpStruct::RtData rt_data;

			out_packet.flag_m = HtInquotePacket::FLAG_COMMON_SERVICE_REQUEST_HISTORY;

			HlpStruct::serializeXmlParameter(request_param, out_packet.data_m );
			writeToSocketHtInquotePacket(clientSocket, out_packet);


			HlpStruct::RtBufferData bufferRtData;

			// receive

			while(true) {
				if (!readFromSocketHtInquotePacket<CTCPSocket>(clientSocket, in_packet) )
					THROW(CppUtils::OperationFailed, "exc_CannotReadFromSocket", "ctx_loadDataFromMaster", CTCPSocket::ErrorCodeToString(clientSocket.GetLastError()));					

				if (in_packet.flag_m == HtInquotePacket::FLAG_RT_PACKET) {

					// deserialize
					HlpStruct::convertFromBinary(rt_data, in_packet.data_m );
					hstcontext->passHistoryEntry(rt_data, bufferRtData );


					if ((++cnt % 10000) == 0) {
						LOG(MSG_INFO, INQUEUEUTILS, "loadDataFromMasterHelper(...) inserted: " << cnt);
					}

				}
				else if (in_packet.flag_m == HtInquotePacket::FLAG_TRANSMITION_FINISH) {
					break;
				}
				else 
					THROW(CppUtils::OperationFailed, "exc_InvalidPacketFlag", "ctx_loadDataFromMaster", (long)in_packet.flag_m );

			};

			LOG(MSG_INFO, INQUEUEUTILS, "loadDataFromMasterHelper(...) inserted totally: " << cnt);


			// update all our history cache
			// notify that we need to update cache for all our threads 
			Inqueue::Engine* engine = CppUtils::getRootObject<Inqueue::Engine>();
			engine->forceThreadsToUpdateCache();

			result = true;

		}
		catch(CppUtils::Exception &e)
		{
			LOG(MSG_ERROR, INQUEUEUTILS, "Exception on loadDataFromMasterHelper(...) " << e.message() << ", so far loaded: " << cnt);
		}
		catch(...)
		{
			LOG(MSG_ERROR, INQUEUEUTILS, "Unknown exception on loadDataFromMasterHelper(...), so far loaded: " << cnt);
		}

		// finished, may close
		try {
			clientSocket.Close();
			clientSocket.Shutdown(CSocket::ssBoth);
		}
		catch(...)
		{
		}

		LOG(MSG_INFO, INQUEUEUTILS, "loadDataFromMasterHelper(...) finished");

		return result;
	}

	void serializeSimulationProfile( BrkLib::SimulationProfileData const& simProfile, HlpStruct::XmlParameter &matadata)
	{
		matadata.getParameter("aggr_period").getAsInt(true) = simProfile.aggrPeriod_m;
		matadata.getParameter("mult_factor").getAsInt(true) = simProfile.multFactor_m;
		matadata.getParameter("gen_tick_method").getAsInt(true) = (int)simProfile.genTickMethod_m;
		matadata.getParameter("ticks_split_number").getAsInt(true) = (int)simProfile.tickSplitNumber_m;
		matadata.getParameter("tick_usage").getAsInt(true) = (int)simProfile.tickUsage_m;
		matadata.getParameter("sim_candle_period").getAsInt(true) = (int)simProfile.simCandlePeriod_m;

		HlpStruct::XmlParameter::XmlParameterList paramlist;
		for(map<CppUtils::String, map<CppUtils::String, BrkLib::SymbolTradingInfo > >::const_iterator it=
			simProfile.symbolsTradingInfo_m.begin(); it != simProfile.symbolsTradingInfo_m.end();it++)
		{
			CppUtils::String const& provider = it->first;
			map<CppUtils::String, BrkLib::SymbolTradingInfo > const& symbMap  = it->second;

			for(map<CppUtils::String, BrkLib::SymbolTradingInfo >::const_iterator it2 = symbMap.begin(); it2!=symbMap.end();it2++) {
				CppUtils::String const& symbol = it2->first;
				BrkLib::SymbolTradingInfo const& sinfo_i = it2->second;

				HlpStruct::XmlParameter xmlparam_i;
				xmlparam_i.getParameter("symbol").getAsString(true) = symbol;
				xmlparam_i.getParameter("provider").getAsString(true) = provider;

				xmlparam_i.getParameter("spread").getAsDouble(true) = sinfo_i.spread_m; 
				xmlparam_i.getParameter("slippage").getAsDouble(true) = sinfo_i.slippage_m;
				xmlparam_i.getParameter("min_sl_dist").getAsDouble(true) = sinfo_i.minStopLossDistance_m;
				xmlparam_i.getParameter("min_tp_dist").getAsDouble(true) = sinfo_i.minTakeProfitDistance_m;
				xmlparam_i.getParameter("min_lo_dist").getAsDouble(true) = sinfo_i.minLimitOrderDistance_m;
				xmlparam_i.getParameter("min_so_dist").getAsDouble(true) = sinfo_i.minStopOrderDistance_m;
				xmlparam_i.getParameter("comission").getAsDouble(true) = sinfo_i.comission_m;
				xmlparam_i.getParameter("comissionPct").getAsDouble(true) = sinfo_i.comissionPct_m;
				xmlparam_i.getParameter("sign_digits").getAsInt(true) = sinfo_i.signDigits_m;
				xmlparam_i.getParameter("point_value").getAsDouble(true) = sinfo_i.pointValue_m;

				paramlist.push_back(xmlparam_i);

			}
		}

		matadata.getXmlParameterList("sinfo_list") = paramlist;


	}

	void deserializeSimulationProfile( BrkLib::SimulationProfileData& simProfile, HlpStruct::XmlParameter const& matadata)
	{
		simProfile.aggrPeriod_m = (HlpStruct::AggregationPeriods)matadata.getParameter("aggr_period").getAsInt();
		simProfile.multFactor_m = (HlpStruct::TickGenerator)matadata.getParameter("mult_factor").getAsInt();
		simProfile.genTickMethod_m = (HlpStruct::TickGenerator)matadata.getParameter("gen_tick_method").getAsInt();
		simProfile.tickSplitNumber_m = matadata.getParameter("ticks_split_number").getAsInt();
		simProfile.tickUsage_m = (HlpStruct::TickUsage)matadata.getParameter("tick_usage").getAsInt();
		simProfile.simCandlePeriod_m = matadata.getParameter("sim_candle_period").getAsInt();

		HlpStruct::XmlParameter::XmlParameterList const& paramlist = matadata.getXmlParameterList("sinfo_list");
		for(int i = 0; i < paramlist.size(); i++) {
			HlpStruct::XmlParameter const& xmlparam_i = paramlist[i];

			BrkLib::SymbolTradingInfo sinfo_i;

			sinfo_i.spread_m = xmlparam_i.getParameter("spread").getAsDouble(); 
			sinfo_i.slippage_m = xmlparam_i.getParameter("slippage").getAsDouble();
			sinfo_i.minStopLossDistance_m = xmlparam_i.getParameter("min_sl_dist").getAsDouble();
			sinfo_i.minTakeProfitDistance_m = xmlparam_i.getParameter("min_tp_dist").getAsDouble();
			sinfo_i.minLimitOrderDistance_m = xmlparam_i.getParameter("min_lo_dist").getAsDouble();
			sinfo_i.minStopOrderDistance_m = xmlparam_i.getParameter("min_so_dist").getAsDouble();
			sinfo_i.comission_m = xmlparam_i.getParameter("comission").getAsDouble();
			sinfo_i.comissionPct_m = xmlparam_i.getParameter("comissionPct").getAsDouble();
			sinfo_i.signDigits_m = xmlparam_i.getParameter("sign_digits").getAsInt();
			sinfo_i.pointValue_m = xmlparam_i.getParameter("point_value").getAsDouble();

			simProfile.symbolsTradingInfo_m[xmlparam_i.getParameter("provider").getAsString()][xmlparam_i.getParameter("symbol").getAsString()] = sinfo_i;

		}



	}

	void prepareSimulationTicksHelper(
		Inqueue::SimulHistoryContext &simhistcontext, 
		BrkLib::BrokerBase::ProviderSymbolCookieMap &providerSymbolCookieMap,
		BrkLib::SimulationProfileData const& simulationProfileStorage,
		double const& startSimulationTime,
		double const& endSimulationTime,
		CppUtils::String const& idString
		)
	{
		HlpStruct::ProviderSymbolList provSymbSimulationList;
		CppUtils::DoubleList provSymbSimulationSpreadList;
		initProviderSimulationListFromParameters(simulationProfileStorage, provSymbSimulationSpreadList,provSymbSimulationList);

		HlpStruct::XmlParameter matadata;
		HlpStruct::XmlParameter symbollist_data;

		AggregatedCache aggregatedCache(CppUtils::getRootObject<HistoryContext>());

		double lowest_time = -1;
		double highest_time = -1;


		CppUtils::IntList cookies;
		for(int i = 0; i < provSymbSimulationList.size(); i++) {

			CppUtils::String const& provider = provSymbSimulationList[ i ].provider_m;
			CppUtils::String const& symbol = provSymbSimulationList[ i ].symbol_m;


			// do not remove as will be using after
			simhistcontext.setReuseCacheFlag(provider, symbol, true);

			// that case forcible cache this
			int cookie = aggregatedCache.cacheSymbol(
				provider,
				symbol, 
				simulationProfileStorage.aggrPeriod_m,
				simulationProfileStorage.multFactor_m,
				startSimulationTime,
				endSimulationTime
				);

			double beginDate, endDate;
			aggregatedCache.checkSymbolCached(provider, symbol, simulationProfileStorage.aggrPeriod_m, simulationProfileStorage.multFactor_m, cookie, beginDate, endDate);

			if (beginDate > 0) {
				if (lowest_time < 0)
					lowest_time = beginDate;
				else
					if (lowest_time > beginDate)
						lowest_time = beginDate;
			}

			if (endDate > 0) {
				if (highest_time < 0)
					highest_time = endDate;
				else
					if (highest_time < endDate)
						highest_time = endDate;
			}

			// after we cached update the status of the first and last entry
			DataPtr dataptr;
			aggregatedCache.dataFirst(cookie, dataptr);

			aggregatedCache.dataNext(cookie, dataptr);
			if (dataptr.isValid()) {
				aggregatedCache.getCurrentPriceData(cookie, dataptr).setFirstInChain();
			}

			aggregatedCache.dataLast(cookie, dataptr);
			aggregatedCache.dataPrev(cookie, dataptr);
			if (dataptr.isValid()) {
				aggregatedCache.getCurrentPriceData(cookie, dataptr).setLastInChain();
			}


			// init this map
			cookies.push_back(cookie);
			providerSymbolCookieMap[provider][symbol]=cookie;

			LOG( MSG_DEBUG, INQUEUEUTILS, "On preparing ticks for simulation cached (renewed) data for provider: " << provider << " symbol: " << 
				symbol << " aggregation period: " << HlpStruct::aggrPeriodToString()(simulationProfileStorage.aggrPeriod_m) <<
				" mult factor: " << simulationProfileStorage.multFactor_m  
				);


		} // end loop

		matadata.getParameter("low_time").getAsDate(true) = lowest_time;
		matadata.getParameter("high_time").getAsDate(true) = highest_time;
		matadata.getParameter("created_on").getAsDate(true) = CppUtils::getTime();

		symbollist_data.getParameter("low_time").getAsDate(true) = lowest_time;
		symbollist_data.getParameter("high_time").getAsDate(true) = highest_time;
		symbollist_data.getParameter("created_on").getAsDate(true) = CppUtils::getTime();
		symbollist_data.getParameter("id_string").getAsString(true) = idString;

		// some profile values
		symbollist_data.getParameter("aggr_period").getAsInt(true) = simulationProfileStorage.aggrPeriod_m;
		symbollist_data.getParameter("mult_factor").getAsInt(true) = simulationProfileStorage.multFactor_m;
		symbollist_data.getParameter("gen_tick_method").getAsInt(true) = (int)simulationProfileStorage.genTickMethod_m;
		symbollist_data.getParameter("ticks_split_number").getAsInt(true) = (int)simulationProfileStorage.tickSplitNumber_m;
		symbollist_data.getParameter("tick_usage").getAsInt(true) = (int)simulationProfileStorage.tickUsage_m;


		HlpStruct::XmlParameter::XmlParameterList &symbol_descr_xml_list = symbollist_data.getXmlParameterList("symbols_list");
		HlpStruct::XmlParameter symbol_descr_i;


		serializeSimulationProfile( simulationProfileStorage, matadata);



		// create cache
		DataRowPtr rowPtr;
		// iterate through all symbols
		aggregatedCache.dataRowFirst(rowPtr);

		// next
		aggregatedCache.dataRowNext(rowPtr);

		vector<HlpStruct::PriceData*> historyPlist(provSymbSimulationList.size()), prevHistoryPlist(provSymbSimulationList.size()); 
		vector< vector<HlpStruct::RtData> > delayedTicks(provSymbSimulationList.size()); // for delayed ticks




		if (rowPtr.isValid()) {  

			// simulation generator class
			if (simulationProfileStorage.genTickMethod_m == HlpStruct::TG_OHLC) {
				LOG( MSG_INFO, INQUEUEUTILS, "Ticks generation started with actual 4 ticks (OHLC) generator... " << HlpStruct::genTicksToString()(simulationProfileStorage.genTickMethod_m) );

				for(int i = 0; i < delayedTicks.size(); i++) {
					delayedTicks[i].resize(4);
				}

				// hold OHLC for a row
				while (true) { 


					for(int i = 0; i < cookies.size(); i++) {
						historyPlist[i] = &((HlpStruct::PriceData&)aggregatedCache.getCurrentDataRow(cookies[ i ], rowPtr));
					}


					for(int i = 0; i < historyPlist.size(); i++) {

						for(int t = 0; t < 4; t++) {
							delayedTicks[i][t].clear();
						}

						if (!historyPlist[i]->isValid())
							continue;

						CppUtils::String const& provider = provSymbSimulationList[ i ].provider_m;
						CppUtils::String const& symbol = provSymbSimulationList[ i ].symbol_m;


						// spitting exactly by 4 ticks
						double diff = (simulationProfileStorage.simCandlePeriod_m*60.0-0.004) / 4;

						////
						HlpStruct::RtData rtdata_open(HlpStruct::RtData::AT_NonClosed, HlpStruct::RtData::RT_Type, historyPlist[i]->time_m + 0.002, provider, symbol);
						HlpStruct::RtData rtdata_high(HlpStruct::RtData::AT_NonClosed, HlpStruct::RtData::RT_Type, rtdata_open.getTime() + diff, provider, symbol);
						HlpStruct::RtData rtdata_low(HlpStruct::RtData::AT_NonClosed, HlpStruct::RtData::RT_Type, rtdata_open.getTime() + 2*diff, provider, symbol);
						HlpStruct::RtData rtdata_close(HlpStruct::RtData::AT_NonClosed, HlpStruct::RtData::RT_Type, rtdata_open.getTime() + 3*diff, provider, symbol);

						double volume_chunk = historyPlist[i]->volume_m / 4.0;

						rtdata_open.getRtData().volume_m = volume_chunk;
						rtdata_high.getRtData().volume_m = volume_chunk;
						rtdata_low.getRtData().volume_m = volume_chunk;
						rtdata_close.getRtData().volume_m = volume_chunk;


						if (simulationProfileStorage.tickUsage_m==HlpStruct::PC_USE_ASK ) {

							rtdata_open.getRtData().ask_m = historyPlist[i]->open_m;
							rtdata_open.getRtData().bid_m = rtdata_open.getRtData().ask_m - provSymbSimulationSpreadList[ i ];

							rtdata_high.getRtData().ask_m = historyPlist[i]->high_m;
							rtdata_high.getRtData().bid_m = rtdata_high.getRtData().ask_m - provSymbSimulationSpreadList[ i ];

							rtdata_low.getRtData().ask_m = historyPlist[i]->low_m;
							rtdata_low.getRtData().bid_m = rtdata_low.getRtData().ask_m - provSymbSimulationSpreadList[ i ];

							rtdata_close.getRtData().ask_m = historyPlist[i]->close_m;
							rtdata_close.getRtData().bid_m = rtdata_close.getRtData().ask_m - provSymbSimulationSpreadList[ i ];

						}
						else if (simulationProfileStorage.tickUsage_m==HlpStruct::PC_USE_BID) {

							rtdata_open.getRtData().bid_m = historyPlist[i]->open2_m;
							rtdata_open.getRtData().ask_m = rtdata_open.getRtData().bid_m + provSymbSimulationSpreadList[ i ];

							rtdata_high.getRtData().bid_m = historyPlist[i]->high2_m;
							rtdata_high.getRtData().ask_m = rtdata_high.getRtData().bid_m + provSymbSimulationSpreadList[ i ];

							rtdata_low.getRtData().bid_m = historyPlist[i]->low2_m;
							rtdata_low.getRtData().ask_m = rtdata_low.getRtData().bid_m + provSymbSimulationSpreadList[ i ];

							rtdata_close.getRtData().bid_m = historyPlist[i]->close2_m;
							rtdata_close.getRtData().ask_m = rtdata_close.getRtData().bid_m + provSymbSimulationSpreadList[ i ];

						}
						else if (simulationProfileStorage.tickUsage_m==HlpStruct::PC_USE_BOTH) {

							rtdata_open.getRtData().ask_m = historyPlist[i]->open_m;
							rtdata_open.getRtData().bid_m = historyPlist[i]->open2_m;

							rtdata_high.getRtData().ask_m = historyPlist[i]->high_m;
							rtdata_high.getRtData().bid_m = historyPlist[i]->high2_m;

							rtdata_low.getRtData().ask_m = historyPlist[i]->low_m;
							rtdata_low.getRtData().bid_m = historyPlist[i]->low2_m;

							rtdata_close.getRtData().ask_m = historyPlist[i]->close_m;
							rtdata_close.getRtData().bid_m = historyPlist[i]->close2_m;
						}


						// valid only here - in btest
						rtdata_open.setProviderSymbolCookie( cookies[ i ] );
						rtdata_high.setProviderSymbolCookie( cookies[ i ] );
						rtdata_low.setProviderSymbolCookie( cookies[ i ] );
						rtdata_close.setProviderSymbolCookie( cookies[ i ] );

						bool tickerInsideExistingTime;
						simhistcontext.passRtData(rtdata_open, tickerInsideExistingTime);
						delayedTicks[i][0] = rtdata_open;

						simhistcontext.passRtData(rtdata_high, tickerInsideExistingTime);
						delayedTicks[i][1] = rtdata_high;

						simhistcontext.passRtData(rtdata_low, tickerInsideExistingTime);
						delayedTicks[i][2] = rtdata_low;

						simhistcontext.passRtData(rtdata_close, tickerInsideExistingTime);
						delayedTicks[i][3] = rtdata_close;


					} // end iteration on each data in the row

					// propagate
					for(int j = 0; j < 4; j++) {
						for(int i =0; i < delayedTicks.size(); i++) {
							if (delayedTicks[i][j].isValid()) {
								simhistcontext.storeRtDataSeparately(delayedTicks[i][j]);
							}
						}
					}


					//
					prevHistoryPlist = historyPlist;


					// check if the next row is valid
					aggregatedCache.dataRowNext(rowPtr);
					if (!rowPtr.isValid())
						break;



				} // rows finished

			} // TG_OHLC
			else if (simulationProfileStorage.genTickMethod_m==HlpStruct::TG_Uniform) {

				LOG( MSG_INFO, INQUEUEUTILS, "Ticks generation started with actual tick generator... " << HlpStruct::genTicksToString()(simulationProfileStorage.genTickMethod_m) );


				SimulationGenerator simGener(
					simulationProfileStorage.genTickMethod_m,
					simulationProfileStorage.simCandlePeriod_m,
					simulationProfileStorage.tickUsage_m,
					simulationProfileStorage.tickSplitNumber_m	);

				for(int i = 0; i < delayedTicks.size(); i++) {
					delayedTicks[i].resize(simulationProfileStorage.tickSplitNumber_m);
				}

				while (true) { 



					for(int i = 0; i < cookies.size(); i++) {
						historyPlist[i] = &((HlpStruct::PriceData&)aggregatedCache.getCurrentDataRow(cookies[ i ], rowPtr));
					}


					for(int i = 0; i < historyPlist.size(); i++) {

						for(int t = 0; t < simGener.getTicksNumber(); t++) {
							delayedTicks[i][t].clear();
						}

						if (!historyPlist[i]->isValid())
							continue;

						//LOG( MSG_DEBUG, INQUEUEUTILS, "FROM SIMULATION CACNE->" << historyPlist[i]->toString() );

						CppUtils::String const& provider = provSymbSimulationList[ i ].provider_m;
						CppUtils::String const& symbol = provSymbSimulationList[ i ].symbol_m;

						// double const& simulationSpread = simulationProfile_m.symbolsTradingInfo_m[provider][symbol].spread_m;

						simGener.generateSimulationTicks(*historyPlist[i], provSymbSimulationSpreadList[ i ] );
						////


						for(int j = 0; j < simGener.getTicksNumber(); j++) {

							// at that point we implicetely notify broker simulation library
							HlpStruct::RtData& rtdata = simGener.getTick( j );
							rtdata.setProvider(provider.c_str());
							rtdata.setSymbol(symbol.c_str());

							// valid only here - in btest
							rtdata.setProviderSymbolCookie( cookies[ i ] );

							// get a list of all AggregationList objects participating 

							// will be aggregating manually

							// store in cache
							bool tickerInsideExistingTime;
							simhistcontext.passRtData(rtdata, tickerInsideExistingTime);
							delayedTicks[i][j] = rtdata;


						} // end iteration over ticks


					} // end iteration on each data in the row

					// propagate
					for(int j = 0; j < simGener.getTicksNumber(); j++) {
						for(int i =0; i < delayedTicks.size(); i++) {
							if (delayedTicks[i][j].isValid()) {
								simhistcontext.storeRtDataSeparately(delayedTicks[i][j]);
							}
						}
					}	


					//
					prevHistoryPlist = historyPlist;


					// check if the next row is valid
					aggregatedCache.dataRowNext(rowPtr);
					if (!rowPtr.isValid())
						break;



				} // rows finished

			} // simulationProfile_m.genTickMethod_m!=TG_CloseOnly
			else if (simulationProfileStorage.genTickMethod_m==HlpStruct::TG_CloseOnly) {

				LOG( MSG_INFO, INQUEUEUTILS, "Ticks generation started with only CLOSE prices... " <<  HlpStruct::genTicksToString()(simulationProfileStorage.genTickMethod_m));
				// onlu close prices

				for(int i = 0; i < delayedTicks.size(); i++) {
					delayedTicks[i].resize(1);
				}

				while (true) { 


					for(int i = 0; i < cookies.size(); i++) {
						historyPlist[i] = &((HlpStruct::PriceData&)aggregatedCache.getCurrentDataRow(cookies[ i ], rowPtr));
					}


					for(int i = 0; i < historyPlist.size(); i++) {

						delayedTicks[i][0].clear();

						if (!historyPlist[i]->isValid())
							continue;

						CppUtils::String const& provider = provSymbSimulationList[ i ].provider_m;
						CppUtils::String const& symbol = provSymbSimulationList[ i ].symbol_m;


						////
						HlpStruct::RtData rtdata;
						rtdata.getAFlag() = HlpStruct::RtData::AT_NonClosed;
						rtdata.getType() = HlpStruct::RtData::RT_Type;
						rtdata.getTime() = historyPlist[i]->time_m;

						//CppUtils::String times = CppUtils::getGmtTime(historyPlist[i]->time_m);
						rtdata.getRtData().volume_m = historyPlist[i]->volume_m;

						if (simulationProfileStorage.tickUsage_m==HlpStruct::PC_USE_ASK ) {

							rtdata.getRtData().ask_m = historyPlist[i]->close_m;
							rtdata.getRtData().bid_m = rtdata.getRtData().ask_m - provSymbSimulationSpreadList[ i ];

						}
						else if (simulationProfileStorage.tickUsage_m==HlpStruct::PC_USE_BID) {
							rtdata.getRtData().bid_m = historyPlist[i]->close2_m;
							rtdata.getRtData().ask_m = historyPlist[i]->close2_m + provSymbSimulationSpreadList[ i ];

						}
						else if (simulationProfileStorage.tickUsage_m==HlpStruct::PC_USE_BOTH) {
							rtdata.getRtData().ask_m = historyPlist[i]->close_m;
							rtdata.getRtData().bid_m = historyPlist[i]->close2_m;
						}


						rtdata.setProvider(provider.c_str());
						rtdata.setSymbol(symbol.c_str());

						// valid only here - in btest
						rtdata.setProviderSymbolCookie( cookies[ i ] );

						// get a list of all AggregationList objects participating 

						bool tickerInsideExistingTime;
						simhistcontext.passRtData(rtdata, tickerInsideExistingTime);
						delayedTicks[i][0] = rtdata;




					} // end iteration on each data in the row

					// propagate
					for(int i =0; i < delayedTicks.size(); i++) {
						if (delayedTicks[i][0].isValid()) {
							simhistcontext.storeRtDataSeparately(delayedTicks[i][0]);
						}
					}


					//
					prevHistoryPlist = historyPlist;


					// check if the next row is valid
					aggregatedCache.dataRowNext(rowPtr);
					if (!rowPtr.isValid())
						break;


				} // rows finished

			} // simulationProfile_m.genTickMethod_m==TG_CloseOnly

		}


		// clean up

		for(int i = 0; i < cookies.size(); i++) {
			aggregatedCache.clearCache( cookies[i] );
			aggregatedCache.cleanupAggregatedData( cookies[i] );
		}

		// save
		CppUtils::String matadata_content;
		HlpStruct::serializeXmlParameter(matadata, matadata_content); 

		CppUtils::saveContentInFile2(simhistcontext.getMetadataPath(), matadata_content);

		// symbol list save
		vector<CacheFileContext> cachedData;
		simhistcontext.getCachedContext( cachedData );
		for(int i = 0; i < cachedData.size(); i++) {
			symbol_descr_i.getParameter("symbol").getAsString(true) = cachedData[i].getSymbol();
			symbol_descr_i.getParameter("provider").getAsString(true) = cachedData[i].getProvider();

			symbol_descr_i.getParameter("begin_time").getAsDate(true) = cachedData[i].getBeginTime();
			symbol_descr_i.getParameter("end_time").getAsDate(true) = cachedData[i].getEndTime();

			symbol_descr_xml_list.push_back(symbol_descr_i);

		}

		CppUtils::String symbollist_content;
		HlpStruct::serializeXmlParameter(symbollist_data, symbollist_content); 
		CppUtils::saveContentInFile2(SimulHistoryContext::getSymbolListPath(simhistcontext.getStoragePrefix()), symbollist_content);


		// log
		LOG( MSG_INFO, INQUEUEUTILS, "Cleaned up cached data for ticks generator" );

	}

	void initProviderSimulationListFromParameters(
		BrkLib::SimulationProfileData const& simulationProfileStorage,
		CppUtils::DoubleList& provSymbSimulationSpreadList, 
		HlpStruct::ProviderSymbolList& provSymbSimulationList
		)
	{
		provSymbSimulationSpreadList.clear();
		provSymbSimulationList.clear();

		for(map<CppUtils::String, map<CppUtils::String, BrkLib::SymbolTradingInfo > >::const_iterator it=
			simulationProfileStorage.symbolsTradingInfo_m.begin(); it != simulationProfileStorage.symbolsTradingInfo_m.end();it++)
		{
			CppUtils::String const& provider = it->first;
			map<CppUtils::String, BrkLib::SymbolTradingInfo > const& symbMap  = it->second;

			for(map<CppUtils::String, BrkLib::SymbolTradingInfo >::const_iterator it2 = symbMap.begin(); it2!=symbMap.end();it2++) {
				CppUtils::String const& symbol = it2->first;


				provSymbSimulationList.push_back(HlpStruct::ProviderSymbolPair(provider, symbol));
				provSymbSimulationSpreadList.push_back( it2->second.spread_m );
			}
		}
	}


};