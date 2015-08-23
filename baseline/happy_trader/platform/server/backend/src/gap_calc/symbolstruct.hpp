#ifndef _GAP_CALC_SYMBOLSTRUCT_INCLUDED
#define _GAP_CALC_SYMBOLSTRUCT_INCLUDED

#include "shared/cpputils/src/cpputils.hpp"



namespace AlgLib {

	// helper functions
	double calcAvr(CppUtils::DoubleList const& data);

	double calcStd(CppUtils::DoubleList const& data);


	


	// this encapsulate the netry for a symbol
	struct SymbolEntry {
		SymbolEntry():
			absoluteProfit_m(0),
			relativeProfit_m(0),
			absoluteMaxIncomePriceGo_m(0),
			relativeMaxIncomePriceGo_m(0),
			prevDayClose_m(0),
			is_short_m(false)
		{
		}

		// these are profits
		double absoluteProfit_m;

		// relative coparing to the previous day close
		double relativeProfit_m;

		// absolute price go
		double absoluteMaxIncomePriceGo_m;

		// relative price go relative to previous day closure
		double relativeMaxIncomePriceGo_m;

		// prev day closure
		double prevDayClose_m;

		// symbol
		CppUtils::String symbol_m;

		bool is_short_m;


	};

	// this encapsulate the entry for a day
	struct DailyEntry
	{
		DailyEntry():
		totalDeals_m(0)
		{
		}

		// total deals per day
		int totalDeals_m;

		// map of symbols
		map<int, SymbolEntry> daySymbols_m;

	};


	struct CalcResult {
		CalcResult(int const id):
			pattTotalCount_m(0),
			pattExecCount_m(0),
			pattStopLossCount_m(0),
			stolLossLevel_m(0),
			volumeFactorFilter_m(0),
			gapFilter_m(0),
			beginTestingDate_m(-1),
			endTestingDate_m(-1),
			fullPeriodCovered_m(false),
			checkedDaysCount_m(0),
			isSingleton_m( false ),
			gapFilterHitCount_m(0),
			volumeFactorHitCount_m(0),
			ID_m(id)
		{
		}

		int ID_m;

		double stolLossLevel_m;
		
		double volumeFactorFilter_m; 
		
		double gapFilter_m;

		double beginTestingDate_m;

		double endTestingDate_m;


		// conuts

		// counts total days
		int checkedDaysCount_m;

		int pattTotalCount_m;

		int pattExecCount_m;

		int pattStopLossCount_m;

		// hits of 2 conditions
		int gapFilterHitCount_m;

		int volumeFactorHitCount_m;

	

		// if fully covered by date time
		bool fullPeriodCovered_m;

		map<double,DailyEntry > dailyEntries_m;

		bool isSingleton_m;

		

		// function

		int getMaxDealsPerDay() const
		{
			int result = 0;

			for(map<double,DailyEntry >::const_iterator it = dailyEntries_m.begin(); it != dailyEntries_m.end(); it++) {
				if (result < it->second.totalDeals_m)
					result = it->second.totalDeals_m;
			}

			return result;

			
		}

		double getAverageDealsPerDay() const
		{

			CppUtils::DoubleList data;
			for(map<double,DailyEntry >::const_iterator it = dailyEntries_m.begin(); it != dailyEntries_m.end(); it++) {
				data.push_back( it->second.totalDeals_m );
			}

			return calcAvr(data);


		}

		

		// this iterates through all days
		// and tells the average relative to yesterday close - how much price has fallen - our profit
		double getAverageRelativeMaxIncomePriceGo() const
		{
			CppUtils::DoubleList data;

			for(map<double,DailyEntry >::const_iterator it = dailyEntries_m.begin(); it != dailyEntries_m.end(); it++) {
				DailyEntry const& dayly_i = it->second;

				for(map<int, SymbolEntry>::const_iterator it2 =  dayly_i.daySymbols_m.begin(); it2 != dayly_i.daySymbols_m.end(); it2++) {
					SymbolEntry symbol_j = it2->second;
					if (symbol_j.absoluteMaxIncomePriceGo_m >=0 ) {

						data.push_back( symbol_j.absoluteMaxIncomePriceGo_m );
						
					}
					
				}


			}

			return calcAvr(data);
		}

		double getStdRelativeMaxIncomePriceGo() const
		{
			CppUtils::DoubleList data;

			for(map<double,DailyEntry >::const_iterator it = dailyEntries_m.begin(); it != dailyEntries_m.end(); it++) {
				DailyEntry const& dayly_i = it->second;

				for(map<int, SymbolEntry>::const_iterator it2 =  dayly_i.daySymbols_m.begin(); it2 != dayly_i.daySymbols_m.end(); it2++) {
					SymbolEntry symbol_j = it2->second;
					if (symbol_j.absoluteMaxIncomePriceGo_m >=0 ) {

						data.push_back( symbol_j.absoluteMaxIncomePriceGo_m );
						
					}
					
				}


			}

			return calcStd(data);
		}

		double getAverageProfit() const
		{
			CppUtils::DoubleList data;

			for(map<double,DailyEntry >::const_iterator it = dailyEntries_m.begin(); it != dailyEntries_m.end(); it++) {
				DailyEntry const& dayly_i = it->second;

				for(map<int, SymbolEntry>::const_iterator it2 =  dayly_i.daySymbols_m.begin(); it2 != dayly_i.daySymbols_m.end(); it2++) {
					SymbolEntry symbol_j = it2->second;
					data.push_back( symbol_j.absoluteProfit_m );
				}


			}

			return calcAvr(data);
		}

		double getStdProfit() const
		{
			CppUtils::DoubleList data;

			for(map<double,DailyEntry >::const_iterator it = dailyEntries_m.begin(); it != dailyEntries_m.end(); it++) {
				DailyEntry const& dayly_i = it->second;

				for(map<int, SymbolEntry>::const_iterator it2 =  dayly_i.daySymbols_m.begin(); it2 != dayly_i.daySymbols_m.end(); it2++) {
					SymbolEntry symbol_j = it2->second;
					data.push_back( symbol_j.absoluteProfit_m );
				}


			}

			return calcStd(data);
		}

		// this will iterate through all symbols and calculate PL RATIO
		double getPLRatio() const
		{
			double onlyProfits = 0, onlyLosses = 0, result = -1;
			int cntProfits = 0, cntLosses = 0;
			
			for(map<double,DailyEntry >::const_iterator it = dailyEntries_m.begin(); it != dailyEntries_m.end(); it++) {
				DailyEntry const& dayly_i = it->second;

				for(map<int, SymbolEntry>::const_iterator it2 =  dayly_i.daySymbols_m.begin(); it2 != dayly_i.daySymbols_m.end(); it2++) {
					SymbolEntry symbol_j = it2->second;
					if (symbol_j.absoluteProfit_m > 0) 
					{
						onlyProfits += symbol_j.absoluteProfit_m;
						cntProfits++;
					}
					else  
					{
						onlyLosses += (-symbol_j.absoluteProfit_m);
						cntLosses++;
					}
				}  


			}

			if (cntProfits >0 && cntLosses > 0) {
				result =  (onlyProfits / cntProfits) / (onlyLosses/cntLosses);
			}
			else if (cntProfits > 0 && cntLosses == 0) {
				// very good profit
				result = 99999999999.0;
			}
			else if (cntProfits == 0 && cntLosses > 0) {
				result = 0.0;
			}
			
			

			return result;
		}

		// just create the symbol list
		CppUtils::String getSymbolList() const
		{
			CppUtils::String result;
			
			for(map<double,DailyEntry >::const_iterator it = dailyEntries_m.begin(); it != dailyEntries_m.end(); it++) {
				DailyEntry const& dayly_i = it->second;

				for(map<int, SymbolEntry>::const_iterator it2 =  dayly_i.daySymbols_m.begin(); it2 != dayly_i.daySymbols_m.end(); it2++) {
					SymbolEntry symbol_j = it2->second;
					
					result += symbol_j.symbol_m + " - "+ CppUtils::getGmtTime2(it->first) + " - " + CppUtils::float2String(symbol_j.absoluteProfit_m, -1,4)+
						(symbol_j.is_short_m ? " SHORT ":" LONG ")  +"\n";

				}

			};



			return result;
		}


		CppUtils::String toString() const
		{
			CppUtils::String result;

			result += CppUtils::String("TESTING PERIOD ") + (fullPeriodCovered_m ? " FULLY COVERED ": " NOT FULLY COVERED ")+ (beginTestingDate_m>0?CppUtils::getGmtTime(beginTestingDate_m):"N/A")+ " - " + 
				(endTestingDate_m>0? CppUtils::getGmtTime(endTestingDate_m):"N/A") + "\n";

			result += "PARAMETER STOP LOSS: " + CppUtils::float2String(stolLossLevel_m, -1, 4)+ "\n";
			result += "PARAMETER VOLUME FACTOR FILTER: " + CppUtils::float2String(volumeFactorFilter_m, -1, 4)+ "\n";
			result += "PARAMETER GAP FILTER: " + CppUtils::float2String(gapFilter_m, -1, 4)+ "\n";

			result += "TOTAL COUNT: " + CppUtils::long2String(pattTotalCount_m)+ "\n";
			result += "PROFIT COUNT: " + CppUtils::long2String(pattExecCount_m)+ "\n";
			result += "LOSS COUNT: " + CppUtils::long2String(pattStopLossCount_m)+ "\n";
			result += "TOTAL DAYS COUNT: " + CppUtils::long2String(checkedDaysCount_m)+ "\n";
			result += "ONE DEAL PER DAY: " + CppUtils::String(isSingleton_m ? "true":"false")+ "\n";

					
			result += "AVERAGE PROFIT: " + CppUtils::float2String(getAverageProfit(), -1, 4)+ "\n";

			return result;
		}

	};

	


	
}; // end of namespace

#endif