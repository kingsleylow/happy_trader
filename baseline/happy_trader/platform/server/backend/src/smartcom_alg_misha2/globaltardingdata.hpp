#ifndef _SMARTCOM_ALG_MISHA2_GLOBALTRADINGDATA_INCLUDED
#define _SMARTCOM_ALG_MISHA2_GLOBALTRADINGDATA_INCLUDED

#include "smartcom_alg_misha2defs.hpp"


namespace AlgLib {


	/**
	* Global trading context - here are all common thing for broker
	*/

	

	class GlobalTradingContext
	{
		public:
			BEGIN_PARAM_ENTRIES_DECLARE(GlobalTradingContext)
				PARAM_ENTRY_DECLARE(CppUtils::String, brokerLimitOrderId_m);	
				PARAM_ENTRY_DECLARE(CppUtils::String, brokerLimitTPOrderId_m);	
				PARAM_ENTRY_DECLARE(CppUtils::String, brokerPositionId_m);
				PARAM_ENTRY_DECLARE(CppUtils::String, brokerStopOrderId_m);

				PARAM_ENTRY_DECLARE(BrkLib::TradeDirection, tradeDirection_m);
				PARAM_ENTRY_DECLARE(double, levelDetectedPrice_m);
				PARAM_ENTRY_DECLARE(double, limitOrderPrice_m);
				PARAM_ENTRY_DECLARE(double, limitTPOrderPrice_m);
				PARAM_ENTRY_DECLARE(double, stopOrderPrice_m);

				PARAM_ENTRY_DECLARE(CppUtils::String, errorReason_m);
			END_PARAM_ENTRIES_DECLARE()
	};

}; // end of namespace
#endif