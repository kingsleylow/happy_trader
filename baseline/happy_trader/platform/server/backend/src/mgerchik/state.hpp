#ifndef _MGERCHIK_STATE_INCLUDED
#define _MGERCHIK_STATE_INCLUDED

#include "../brklib/brklib.hpp"

namespace AlgLib {
	struct TradingState {
		TradingState():
			state_m(TS_Initial), // initial state
			ch_SizeBidSizeAsk_On_Long_m(false),
			ch_SizeBidSizeAsk_On_Short_m(false),
			ch_PriceIsRising_On_m(false),
			ch_PriceIsFalling_On_m(false),
			ch_Risk_More_Threashold_m(false)
		{
		}

		// enumeration of states
		enum TS {
			TS_Dummy =	 0,
			TS_Initial = 1, // only in initial state it reaps the enter conditions, once it reached it 
			
			TS_Ready_Long = 2, // when it is ready, will send order
			TS_Ready_Short =3, // 
			
			TS_Open_Long	 =4, // trade is confirmed  - e.i. we made trade - we opend pos and have our pos opened
			TS_Open_Short = 5, // 
			
			TS_Pending_Long	 =6, // trade is not confirmed  - e.i. we made trade - but order is till pending
			TS_Pending_Short = 7, // 

			//TS_Ready_Close_Long=8, // we are ready to close from normal position
			//TS_Ready_Close_Short=9, // 

			//TS_Failed_Close_Long=10, // trade with limit is failed, will need to apply exit 2
			//TS_Failed_Close_Short=11, // 

			TS_Ready_Close_2_Long = 12, // ready make close via 2nd condition
			TS_Ready_Close_2_Short = 13, 

			//TS_Failed_Close_2_Long=14, // exit 2 failed
			//TS_Failed_Close_2_Short=15, // 

			TS_Ready_Cancel_Long = 16, // ready make cancel (from pending order at the first stage)
			TS_Ready_Cancel_Short = 17,

			TS_Inconsistent = 18,

			TS_Ready_Close_3_Long = 19, // ready to make urgent close
			TS_Ready_Close_3_Short = 20


		};

		
		bool operator == (TradingState const& rhs) const
		{
			return (
        symbol_m==rhs.symbol_m && 
				ch_Risk_More_Threashold_m == rhs.ch_Risk_More_Threashold_m &&
				ch_PriceIsFalling_On_m == rhs.ch_PriceIsFalling_On_m &&
				indexesFalling_On_m == rhs.indexesFalling_On_m &&
				indexesRising_On_m == rhs.indexesRising_On_m &&
				ch_PriceIsRising_On_m == rhs.ch_PriceIsRising_On_m &&
				ch_SizeBidSizeAsk_On_Long_m == rhs.ch_SizeBidSizeAsk_On_Long_m &&
				ch_SizeBidSizeAsk_On_Short_m == rhs.ch_SizeBidSizeAsk_On_Short_m);
		}

		// -----------------

		bool isIdle() const
		{
			if (state_m==TS_Initial || state_m == TS_Dummy)
				return true;
			else
				return false;
		}

		CppUtils::String toString() const {

			CppUtils::String result = "STATE: \n";
			switch(state_m) {
				case TS_Dummy: result += " [ "+ symbol_m + " ] " + "TE_Dummy"; break;
				case TS_Initial: result += " [ "+  symbol_m + " ] " + "TS_Initial"; break;
				
				case TS_Ready_Long: result += " [ "+  symbol_m + " ] " + "TS_Ready_Long"; break;
				case TS_Ready_Short: result += " [ "+  symbol_m + " ] " + "TS_Ready_Short"; break;
						
				case TS_Open_Long: result += " [ "+  symbol_m + " ] " + "TS_Open_Long"; break;
				case TS_Open_Short: result += " [ "+  symbol_m + " ] " + "TS_Open_Short"; break;

				case TS_Pending_Long: result += " [ "+  symbol_m + " ] " + "TS_Pending_Long"; break;
				case TS_Pending_Short: result += " [ "+  symbol_m + " ] " + "TS_Pending_Short"; break;

				//case TS_Ready_Close_Long: result += " [ "+  symbol_m + " ] " + "TS_Ready_Close_Long"; break;
				//case TS_Ready_Close_Short: result += " [ "+  symbol_m + " ] " + "TS_Ready_Close_Short"; break;

				//case TS_Failed_Close_Long: result += " [ "+  symbol_m + " ] " + "TS_Failed_Close_Long"; break;
				//case TS_Failed_Close_Short: result += " [ "+  symbol_m + " ] " + "TS_Failed_Close_Short"; break;

				case TS_Ready_Close_2_Long: result += " [ "+  symbol_m + " ] " + "TS_Ready_Close_2_Long"; break;
				case TS_Ready_Close_2_Short: result += " [ "+  symbol_m + " ] " + "TS_Ready_Close_2_Short"; break;

				//case TS_Failed_Close_2_Long: result += " [ "+  symbol_m + " ] " + "TS_Failed_Close_2_Long"; break;
				//case TS_Failed_Close_2_Short: result += " [ "+  symbol_m + " ] " + "TS_Failed_Close_2_Short"; break;

				case TS_Ready_Cancel_Long: result += " [ "+  symbol_m + " ] " + "TS_Ready_Cancel_Long"; break;
				case TS_Ready_Cancel_Short: result += " [ "+  symbol_m + " ] " + "TS_Ready_Cancel_Short"; break;

				case TS_Inconsistent: result += " [ "+  symbol_m + " ] " + "TS_Inconsistent"; break;

				case TS_Ready_Close_3_Long: result += " [ "+  symbol_m + " ] " + "TS_Ready_Close_3_Long"; break;
				case TS_Ready_Close_3_Short: result += " [ "+  symbol_m + " ] " + "TS_Ready_Close_3_Short"; break;

				
				
				default:
					result += " [ "+ symbol_m + " ] " + "N/A"; 
			}

			result += "\nstate variables:\n";
			
			result += CppUtils::String("ch_SizeBidSizeAsk_On_Long_m=") + (ch_SizeBidSizeAsk_On_Long_m ? "true" : "false") + "\n";
			result += CppUtils::String("ch_SizeBidSizeAsk_On_Short_m=") + (ch_SizeBidSizeAsk_On_Short_m ? "true" : "false") + "\n";

			result += CppUtils::String("ch_PriceIsRising_On_m=") + (ch_PriceIsRising_On_m ? "true" : "false") + "\n";
			result += CppUtils::String("ch_PriceIsFalling_On_m=") + (ch_PriceIsFalling_On_m ? "true" : "false") + "\n";

			result += CppUtils::String("ch_Risk_More_Threashold_m=") + (ch_Risk_More_Threashold_m ? "true" : "false") + "\n";
			
			for(map<CppUtils::String, bool>::const_iterator it = indexesRising_On_m.begin(); it != indexesRising_On_m.end(); it++) {
				result += "indexesRising_On_m: " + it->first + "=" + (it->second ? "true":"false") + "\n";
			}

			for(map<CppUtils::String, bool>::const_iterator it = indexesFalling_On_m.begin(); it != indexesFalling_On_m.end(); it++) {
				result += "indexesFalling_On_m: " + it->first + "=" + (it->second ? "true":"false") + "\n";
			}
			
			result += "STATE END\n";
		
			return result;
		}

		// members
	public:

			// these are variables uniquely describing our states
		bool ch_SizeBidSizeAsk_On_Long_m;

		bool ch_SizeBidSizeAsk_On_Short_m;

		bool ch_PriceIsRising_On_m;

		bool ch_Risk_More_Threashold_m;

		map<CppUtils::String, bool> indexesRising_On_m;

		map<CppUtils::String, bool> indexesFalling_On_m;


		bool ch_PriceIsFalling_On_m;

		CppUtils::String symbol_m;

		TS state_m;
		
	};

} // end of namespace

#endif