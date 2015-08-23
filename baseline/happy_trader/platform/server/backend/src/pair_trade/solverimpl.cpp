#include "solverimpl.hpp"
#include "implement.hpp"
#include "event.hpp"
#include "robot.hpp"

#define SOLVERIMPL "SOLVERIMPL"

namespace AlgLib {

	LevelCalculator::LevelCalculator(TradingSequenceRobot& base):
		base_m(base),
		//cacheCallbackCookie_m(-1),
		dataset_m(this)
	{
	}
			
	LevelCalculator::~LevelCalculator()
	{
	}


	void LevelCalculator::initialized()
	{
		initialized_m = false;
		parsers_m.clear();
		dataset_m.clear();
		
		cache_ids_m.clear();

		// parsers
		CppUtils::StringSet const& synthetic_symbols = getBase().getSyntheticSmartcomSymbols();
		for( CppUtils::StringSet::const_iterator i3 = synthetic_symbols.begin(); i3 != synthetic_symbols.end(); i3++) {
			
			CppUtils::String const& synth_symbol  = *i3;

			parsers_m[*i3] = new CppUtils::Parser(synth_symbol);

			// register synthetic symbol
			dataset_m.registerCalculatedSymbol( synth_symbol );

			// init volume coefficients
			if (getBase().getTradeParam().synthSymbolToTrade_m.get() == *i3) {

				int double_volume =	  getBase().getTradeParam().tradeVolume_m * 2;


				getTradeRequestFromSynthSymbol(entryListLong_SingleVolume_m, synth_symbol, BrkLib::TD_LONG, getBase().getTradeParam().tradeVolume_m);
				getTradeRequestFromSynthSymbol(entryListShort_SingleVolume_m, synth_symbol, BrkLib::TD_SHORT, getBase().getTradeParam().tradeVolume_m);

				getTradeRequestFromSynthSymbol(entryListLong_DoubleVolume_m, synth_symbol, BrkLib::TD_LONG, double_volume);
				getTradeRequestFromSynthSymbol(entryListShort_DoubleVolume_m, synth_symbol, BrkLib::TD_SHORT, double_volume);

				LOG(MSG_INFO, SOLVERIMPL, "As our synth trading symbol is: " << *i3 << " initialized this!" );
			}
		}

		int fastPer = getBase().getTradeParam().maFastPeriod_m;
		int slowPer = getBase().getTradeParam().maSlowPeriod_m;
		int cachePer = max(fastPer, slowPer);

		signalContextMap_m.clear();

		

		
		//dataset_m.setUpParameters(cachePer*2, false);
	}

	void LevelCalculator::deinitialized()
	{
		CppUtils::StringSet const& synthetic_symbols = getBase().getSyntheticSmartcomSymbols();
		for( set<CppUtils::String>::const_iterator i3 = synthetic_symbols.begin(); i3 != synthetic_symbols.end(); i3++) {

			//LOG(MSG_INFO, SOLVERIMPL, "Destroying parser for synthetic symbol: " << *i3)

			delete parsers_m[*i3];

			dataset_m.unregisterCalculatedSymbol(*i3);
		}

		CppUtils::StringMap const& smartcom_symbols = getBase().getSmartcomSymbols();
		for(CppUtils::StringMap::const_iterator it = smartcom_symbols.begin(); it != smartcom_symbols.end(); it++) {
			CppUtils::String const& symbol_i = it->first;
			dataset_m.unregisterSymbol(symbol_i);
		}

		parsers_m.clear();
	}

	void LevelCalculator::onThreadStarted(CalcContext* ctx)
	{
		
		//cacheCallbackCookie_m = ctx->historyaccessorPtr->registerCallBack(this);

		

		

		LOG(MSG_INFO, SOLVERIMPL, "Thread started");

	}

	void LevelCalculator::onThreadFinished(CalcContext* ctx)
	{
		//if (cacheCallbackCookie_m >=0)
		//	ctx->historyaccessorPtr->removeCallBack(cacheCallbackCookie_m);

		LOG(MSG_INFO, SOLVERIMPL, "Thread finished");
	}

	
	void LevelCalculator::onUpdateCaclulatedField(
			HlpStruct::MultiSymbolCache const& base,
			HlpStruct::MultiSymbolCache::RowType const& physical_row, 
			HlpStruct::MultiSymbolCache::RowType const& calculated_row,  
			CppUtils::String const& calcFieldName,
			double const& key_ttime,
			HlpStruct::PriceData& calcPriceData,
			bool& calculated)
	{
			if (physical_row.size() != cache_ids_m.size()) 
				// ready to calculate
				return;

			CppUtils::StringSet const& synthetic_symbols = getBase().getSyntheticSmartcomSymbols();
			CppUtils::StringMap const& smartcom_symbols = getBase().getSmartcomSymbols();

			CppUtils::StringSet::const_iterator it = synthetic_symbols.find(calcFieldName);
			if (it == synthetic_symbols.end())
				// synthetic symbol found
				return;

			try {
				CppUtils::Parser& parser_for_synthetic_symbol = *(parsers_m[calcFieldName]);
				calcPriceData.close2_m = calculate<Math::P_CloseBid>(parser_for_synthetic_symbol,physical_row);
				calcPriceData.open2_m = calculate<Math::P_OpenBid>(parser_for_synthetic_symbol, physical_row);

				// store calculated
				DRAWOBJ_LINEINDIC_3(
					getBase().getBase().getRunName(), 
					CppUtils::getTime(), 
					"CALC_DATA", 
					calcFieldName, getBase().getBase().getBarDataProvider_Smartcom(), 
					"OPEN", calcPriceData.open2_m, -1,
					"CLOSE", calcPriceData.close2_m, -1,
					"ROW_TTIME", key_ttime, -1	  
					);

				// calc open
				calculated = true;
			}
			catch(CppUtils::Exception& e)
			{
				LOG(MSG_INFO, SOLVERIMPL, "Exception on calling onUpdateCaclulatedField(...) for symbol: " << calcFieldName << " - " << e.message());
			}


		
	}

	template<Math::CandlePrices const typeOfPrice>
	double LevelCalculator::calculate(CppUtils::Parser& parser, HlpStruct::MultiSymbolCache::RowType const& physical_row)
	{
		CppUtils::StringMap const& smartcom_symbols = getBase().getSmartcomSymbols();
		parser.ClearBind();
		for( HlpStruct::MultiSymbolCache::RowType::const_iterator it = physical_row.begin(); it != physical_row.end(); it++) {
				HlpStruct::PriceData const& pdata_i = it->second;
				CppUtils::String const& physical_field = it->first;

				if (!pdata_i.isValid()) 
					THROW(CppUtils::OperationFailed, "exc_PriceDataInvalid", "ctx_calculate", "Field name: " << physical_field);
				

				// fins synthetic symbol
				
				CppUtils::StringMap::const_iterator ia = smartcom_symbols.find( physical_field );
				if (ia == smartcom_symbols.end() ) 
					THROW(CppUtils::OperationFailed, "exc_ArtificalSymbolNotFound", "ctx_calculate", "Field name: " << physical_field);
			
				CppUtils::String const& symbol_art_i = ia->second;

				//LOG(MSG_INFO, SOLVERIMPL, "Symbol: " << genuine_symbol << "(" << symbol_art_i << ")" << CppUtils::getGmtTime(ttime) << " is: " << pdata.close2_m)
				
				// bind
				parser[symbol_art_i] = Math::GetPrice2<typeOfPrice>::getCandlePrice(pdata_i);
				
			}

		// evaluate
		double val = -1;
		try {
			val = parser.Evaluate();
		}
		catch(std::exception& e)																						 
		{
			THROW(CppUtils::OperationFailed, "exc_ParserError", "ctx_calculate", e.what());
			
		}

		return val;

	}

	void LevelCalculator::getTradeRequestFromSynthSymbol(
			MultuSymbolRequestEntryList& reqList, 
			CppUtils::String const& synthSymbol, 
			BrkLib::TradeDirection const td,
			int const volume
		)
	{
		reqList.clear();
		LOG(MSG_DEBUG, SOLVERIMPL, "Evaluating volume coefficients for synth symbol: " << synthSymbol );


		map<CppUtils::String, CppUtils::Parser*>::iterator it = parsers_m.find(synthSymbol);
		if (it == parsers_m.end())
			THROW(CppUtils::OperationFailed, "exc_ParserNotRegistered", "ctx_getTradeRequestFromSynthSymbol", synthSymbol );

		CppUtils::StringMap const& smartcom_symbols_reverse = getBase().getSmartcomSymbolsReverse();

		CppUtils::Parser & parser = *it->second;
		
		// need to get cooeficients in formula 
		//

		
		

		// iterate through all reverse symbols
		for( CppUtils::StringMap::const_iterator it = smartcom_symbols_reverse.begin(); it != smartcom_symbols_reverse.end(); it++) {
			CppUtils::String const& symbol_pseudonym = it->first;
			CppUtils::String const& symbol_real = it->second;

			
			parser.ClearBind();
			for( CppUtils::StringMap::const_iterator it2 = smartcom_symbols_reverse.begin(); it2 != smartcom_symbols_reverse.end(); it2++) {
				CppUtils::String const& symbol_pseudonym_2 = it2->first;

				if (symbol_pseudonym == symbol_pseudonym_2)
					parser[symbol_pseudonym_2]  = 1;
				else
					parser[symbol_pseudonym_2]  = 0;

			}

			// evaluate here
			try {
				double volume_coeff = parser.Evaluate();
				

				// setup
				MultuSymbolRequestEntry reqEntry;
				reqEntry.symbol_m = symbol_real;
				reqEntry.direction_m = td;
				
				reqEntry.volume_m = abs(volume * (int)volume_coeff);

				if (td == BrkLib::TD_LONG) {
					if (volume_coeff < 0) 
						reqEntry.direction_m = BrkLib::TD_SHORT;
					else if (volume_coeff > 0) 
						reqEntry.direction_m = BrkLib::TD_LONG;		  
					else
						THROW(CppUtils::OperationFailed, "exc_VolumeCannotBeZero", "ctx_getTradeRequestFromSynthSymbol", "");
				}
				else if (td == BrkLib::TD_SHORT) {
					if (volume_coeff < 0) 
						reqEntry.direction_m = BrkLib::TD_LONG;
					else if (volume_coeff > 0) 
						reqEntry.direction_m = BrkLib::TD_SHORT;		  
					else
						THROW(CppUtils::OperationFailed, "exc_VolumeCannotBeZero", "ctx_getTradeRequestFromSynthSymbol", "");
				}													     
				else
					THROW(CppUtils::OperationFailed, "exc_DirectionTradeInvalid", "ctx_getTradeRequestFromSynthSymbol", "");

				reqList.push_back(reqEntry);
				LOG(MSG_DEBUG, SOLVERIMPL, "Added request entry: " << reqEntry.toString() << "for synthetic symbol: " << synthSymbol << "( pseudonym: " << symbol_pseudonym << ")");

			}
			catch(std::exception& e)
			{
				THROW(CppUtils::OperationFailed, "exc_ParserError", "ctx_getTradeRequestFromSynthSymbol", e.what());
			}
																	  
			// resolve

			
		}


	}
	
	void LevelCalculator::onRtData(HlpStruct::RtData const& rtdata,CalcContext* ctx)
	{
		// need to extract symbols
		// calculate synthetic if any

		
		if (getBase().getBase().getBarDataProvider_Smartcom() != rtdata.getProvider2())
			return;


		CppUtils::StringMap const& smartcom_symbols = getBase().getSmartcomSymbols();
		CppUtils::StringSet const& synthetic_symbols = getBase().getSyntheticSmartcomSymbols();

		if (!initialized_m) {
			
			// iterate through all synbols and cache them
			for(CppUtils::StringMap::const_iterator it = smartcom_symbols.begin(); it != smartcom_symbols.end(); it++) {
				CppUtils::String const& symbol_i = it->first;

				int id = ctx->historyaccessorPtr->cacheSymbol( getBase().getBase().getBarDataProvider_Smartcom(), symbol_i, HlpStruct::AP_Minute, getBase().getTradeParam().tradePeriodMinutes_m);
				cache_ids_m[ symbol_i ] = id;
				dataset_m.registerSymbol( symbol_i );

			}

			LOG(MSG_INFO, SOLVERIMPL, "Initialized all" );
			initialized_m =  true;

			//flag_m = 0;
		}

		
		int fastPer = getBase().getTradeParam().maFastPeriod_m;
		int slowPer = getBase().getTradeParam().maSlowPeriod_m;
		int cachePer = max(fastPer, slowPer);

		// will raise on each symbol arrival
		map<CppUtils::String, int>::const_iterator it2 = cache_ids_m.find(rtdata.getSymbol2()); 
		if (it2 == cache_ids_m.end())
			return;

		int cache_id = it2->second;
		HlpStruct::PriceData const& unclosed = ctx->historyaccessorPtr->getUnclosedCandle(cache_id);

		if (unclosed.isNewPeriod()) {
			//LOG(MSG_INFO, SOLVERIMPL, "Unclosed symbol: " << rtdata.getSymbol() << " - " << unclosed.toString());
			Inqueue::DataRowPtr rptr =  ctx->historyaccessorPtr->dataRowLast();
			
			int total_idx = cachePer*2; 

			// clear dataset because we could chaise symbol from the past and need to recalculate everything
			dataset_m.clearData();
			
			// iterate through all rows
			while( ctx->historyaccessorPtr->dataRowPrev(rptr)) {
				// get current price data
				

				if (total_idx-- < 0)
					break;

				// iterate through all symbols
				double ttime = ctx->historyaccessorPtr->getCurrentDataRowTime(rptr);
				for( map<CppUtils::String, int>::const_iterator it2 = cache_ids_m.begin(); it2 != cache_ids_m.end(); it2 ++) {
					
					CppUtils::String const& symbol_i = it2->first;
					HlpStruct::PriceData const& pdata = ctx->historyaccessorPtr->getCurrentDataRow(it2->second, rptr );

					//LOG(MSG_INFO, SOLVERIMPL, "time: " << CppUtils::getGmtTime(ttime) << " symbol: " << symbol_i << " data: " << pdata.toString() );
						
					if (pdata.isValid()) {
						dataset_m.addPriceData(symbol_i, pdata);
					} else {
						HlpStruct::PriceData pdata_prev = ctx->historyaccessorPtr->getCurrentDataRowTryLocatePrevValid( it2->second, rptr );
						if (pdata_prev.isValid())	 {
							//LOG(MSG_INFO, SOLVERIMPL, "time: " << CppUtils::getGmtTime(ttime) << " symbol: " << symbol_i << " Found prev data: " << pdata_prev.toString() );
							dataset_m.addPriceDataAsImportedFromPast(ttime, symbol_i, pdata_prev.close2_m);
						} else {
							LOG(MSG_INFO, SOLVERIMPL, "time: " << CppUtils::getGmtTime(ttime) << " symbol: " << symbol_i << " Cannot find prev data" );
						}

					}
	

				} // end iterating through symbols

				
				
																  
			} // end loop	through rows
			
			// log
			/*
			CppUtils::UnsignedLongLong tm_stamp  = CppUtils::UnsignedLongLong(CppUtils::getTime() * 1000.0);
			CppUtils::String file = "c:\\temp\\data\\content_raw_i_" + CppUtils::long64ToString(tm_stamp) + ".txt";
			CppUtils::saveContentInFile2(file, dataset_m.dump());

			// debug dump
			file = "c:\\temp\\data\\content_debug_i_" + CppUtils::long64ToString(tm_stamp) + ".txt";
			CppUtils::saveContentInFile2(file, ctx->historyaccessorPtr->makeDebugDump(true));
			*/

			
			// because this stuff below is calculated for finished candles - time is contained in thel last row
			double row_ttime = -1;
			if (dataset_m.last()) {
				row_ttime = dataset_m.getTimeKey();
			}
			
			typedef Math::MovAverMathEngineNoNegativePrice<Math::MultiSymbolCacheSeriesProvider> MaEngineType;

			// iterate through synthetic symbols
			for( set<CppUtils::String>::const_iterator i3 = synthetic_symbols.begin(); i3 != synthetic_symbols.end(); i3++) {
				CppUtils::String const& symbol_i3 = *i3;
				MaEngineType ma_channel_fast(fastPer);
				MaEngineType::Result res_ma_channel_fast = 
					ctx->historyaccessorPtr->callMathEngineWithMultiSymbolCacheSeries< MaEngineType::Result, Math::P_CloseBid, MaEngineType >(dataset_m, *i3, ma_channel_fast);

				MaEngineType ma_channel_slow(slowPer);
				MaEngineType::Result res_ma_channel_slow = 
					ctx->historyaccessorPtr->callMathEngineWithMultiSymbolCacheSeries< MaEngineType::Result, Math::P_CloseBid, MaEngineType >(dataset_m, *i3, ma_channel_slow);

				
				if (!res_ma_channel_slow.isValid_m) {
					//LOG(MSG_INFO, SOLVERIMPL, "Trading symbol: " << symbol_i3 << " time:" << CppUtils::getGmtTime(row_ttime) <<  " MA slow is invalid");
					continue;
				}

				if (!res_ma_channel_fast.isValid_m) {
					//LOG(MSG_INFO, SOLVERIMPL, "Trading symbol: " << symbol_i3 << " time:" << CppUtils::getGmtTime(row_ttime) << " MA fast is invalid");
				
					continue;
				}


				BrkLib::TradeDirection td = signalContextMap_m[symbol_i3].update(row_ttime, res_ma_channel_slow.result_m, res_ma_channel_fast.result_m);

				//LOG(MSG_INFO, SOLVERIMPL, "symbol: " << symbol_i3 << " time:" << CppUtils::getGmtTime(row_ttime) << " MA slow: " << res_ma_channel_slow << " MA fast: " << res_ma_channel_fast << ", result: " << BrkLib::tradeDirection2String(td));

				// main notification
				DRAWOBJ_LINEINDIC_4(
					getBase().getBase().getRunName(), 
					CppUtils::getTime(), 
					"MA", 
					symbol_i3, getBase().getBase().getBarDataProvider_Smartcom(), 
					"MASLOW", res_ma_channel_slow.result_m, -1, 
					"MAFAST", res_ma_channel_fast.result_m, -1,
					"ROW_TTIME", row_ttime, -1,
					"SIGNAL",td, -1
				);

				if (getBase().getTradeParam().synthSymbolToTrade_m.get() != symbol_i3)
					continue;
	

				LOG(MSG_INFO, SOLVERIMPL, "Trading symbol: " << symbol_i3 << " time:" << CppUtils::getGmtTime(row_ttime) << " MA slow: " << res_ma_channel_slow.result_m << " MA fast: " << res_ma_channel_fast.result_m << ", result: " << BrkLib::tradeDirection2String(td));
				
				
				// make trading solution
				if (td == BrkLib::TD_LONG && (
					getBase().getGlobalTradeContext().get().allowTrading_m == BrkLib::TD_BOTH ||
					getBase().getGlobalTradeContext().get().allowTrading_m == BrkLib::TD_LONG
					)
				) {

					TE_Signal_Long e;
					getBase().getTradingMachine().processEvent<TE_Signal_Long>(e);	

				}
				else if (td == BrkLib::TD_SHORT	 && (
					getBase().getGlobalTradeContext().get().allowTrading_m == BrkLib::TD_BOTH ||
					getBase().getGlobalTradeContext().get().allowTrading_m == BrkLib::TD_SHORT
					)
				) {

					TE_Signal_Short e;
					getBase().getTradingMachine().processEvent<TE_Signal_Short>(e);
				}
				

				
				/*
				if (flag_m == 0) {
					flag_m = 1;

					TE_Signal_Long e;
					getBase().getTradingMachine().processEvent<TE_Signal_Long>(e);	
				}
				else {
					flag_m = 0;

					TE_Signal_Short e;
					getBase().getTradingMachine().processEvent<TE_Signal_Short>(e);
				}
				*/
				
				
			} // loop through suymbols
			


				
			



			/*
			CppUtils::UnsignedLongLong tm_stamp  = CppUtils::UnsignedLongLong(CppUtils::getTime() * 1000.0);
			CppUtils::String file = "c:\\temp\\data\\content_allrows_i_" + CppUtils::long64ToString(tm_stamp) + ".txt";
			CppUtils::String data = ctx->historyaccessorPtr->dumpAllRows();
			CppUtils::saveContentInFile2(file, data);

			
			file = "c:\\temp\\data\\content_debug_i_" + CppUtils::long64ToString(tm_stamp) + ".txt";
			CppUtils::saveContentInFile2(file, ctx->historyaccessorPtr->makeDebugDump(true));
			*/
			
			
						

			/*
			HlpStruct::MultiSymbolCache dataset_fixed = dataset_m;
			fixGapsInDataset(dataset_fixed, -1);
			file = "c:\\temp\\data\\content_fixed_i_" + CppUtils::long64ToString(tm_stamp) + ".txt";
			CppUtils::saveContentInFile2(file, dataset_fixed.dump());

			// calculate data

			HlpStruct::MultiSymbolCache synthetic_dataset;
			calculateSynthetic(dataset_fixed, synthetic_dataset);
			file = "c:\\temp\\data\\synthetic_i_" + CppUtils::long64ToString(tm_stamp) + ".txt";
			CppUtils::saveContentInFile2(file, synthetic_dataset.dump());
			*/

			
		
		} // if new period


		
	}

	void LevelCalculator::onLevel2Data(HlpStruct::RtLevel2Data const& level2data, CalcContext* ctx)
	{
				
	}


	void LevelCalculator::pushPendingSingal(BrkLib::TradeDirection const td)
	{
		pendingSignals_m.push_back( td );
	}


	int LevelCalculator::sendPendingSignals()
	{
		int sz = pendingSignals_m.size();
		while(!pendingSignals_m.isEmpty()) {
			BrkLib::TradeDirection td = pendingSignals_m.get_front();
			pendingSignals_m.delete_front();

			// send
			if (td == BrkLib::TD_LONG) {
				TE_Signal_Long e;
				getBase().getTradingMachine().processEvent<TE_Signal_Long>(e);
			}
			else if (td == BrkLib::TD_SHORT) {
				TE_Signal_Short e;
				getBase().getTradingMachine().processEvent<TE_Signal_Short>(e);
			}

		}

		return sz;
	}

	void LevelCalculator::heartBeat()
	{
		if (getBase().getGlobalTradeContext().get().state_Life_Time_TS_Ready_Open >= 0) {
			if (getBase().getGlobalTradeContext().get().state_Life_Time_TS_Ready_Open-- <= 0 ) {
	
				// reset this timeout
				getBase().getGlobalTradeContext().get().state_Life_Time_TS_Ready_Open = -1;
				
				// timeout event
				TE_Timeout e;
				getBase().getTradingMachine().processEvent<TE_Timeout>(e);

			}
		}
	}

	
};  // end of namespace