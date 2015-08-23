#ifndef _PAIR_TARDE_TRADEPARAMS_INCLUDED
#define _PAIR_TARDE_TRADEPARAMS_INCLUDED

#include "pair_tradedefs.hpp"
#include "statemachinecontext.hpp"

namespace AlgLib {
	struct TradeParam
	{
		TradeParam()
		{
		}

		bool isInvalid() const
		{
			return false;
		}

		BEGIN_PARAM_ENTRIES_DECLARE(TradeParam)

			PARAM_ENTRY_DECLARE(int, tradePeriodMinutes_m);
			PARAM_ENTRY_DECLARE(bool, isDemoTrade_m);
			PARAM_ENTRY_DECLARE(int,	maSlowPeriod_m);
			PARAM_ENTRY_DECLARE(int,	maFastPeriod_m);
			PARAM_ENTRY_DECLARE(int,	tradeVolume_m);
			PARAM_ENTRY_DECLARE(CppUtils::String, synthSymbolToTrade_m);
			PARAM_ENTRY_DECLARE(CppUtils::String,	tradePrortfolio_m);

			PARAM_ENTRY_DECLARE(int ,	brokerTimeoutSec_m);
			

		END_PARAM_ENTRIES_DECLARE()

	};
};

#endif