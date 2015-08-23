#ifndef _PAIR_TRADE_GLOBALTRADINGDATA_INCLUDED
#define _PAIR_TRADE_GLOBALTRADINGDATA_INCLUDED

#include "pair_tradedefs.hpp"
#include "commonstruct.hpp"


namespace AlgLib {


	/**
	* Global trading context - here are all common thing for broker
	*/

	

	class GlobalTradingContext
	{
		public:
		
			// ctor
			GlobalTradingContext()
			{
				allowTrading_m = BrkLib::TD_NONE;
				state_Life_Time_TS_Ready_Open = -1;
				
			}

		 BEGIN_PARAM_ENTRIES_DECLARE(GlobalTradingContext)
			
			// flag stating allowed trade direction
			PARAM_ENTRY_DECLARE(BrkLib::TradeDirection, allowTrading_m);
			
			// next trading paraneters
			PARAM_ENTRY_DECLARE(MultuSymbolRequestEntryList, nextPairTradeParam_m );
			PARAM_ENTRY_DECLARE(BrkLib::TradeDirection, ourTradeDirection_m);

			// life time of Time_TS_Ready_Open
			PARAM_ENTRY_DECLARE(int, state_Life_Time_TS_Ready_Open);
		

		 END_PARAM_ENTRIES_DECLARE()
			
	};

}; // end of namespace
#endif