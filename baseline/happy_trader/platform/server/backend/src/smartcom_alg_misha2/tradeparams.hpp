#ifndef _SMARTCOM_ALG_MISHA2_TRADEPARAMS_INCLUDED
#define _SMARTCOM_ALG_MISHA2_TRADEPARAMS_INCLUDED

#include "smartcom_alg_misha2defs.hpp"
#include "statemachinecontext.hpp"

namespace AlgLib {
	struct TradeParam
	{
		TradeParam()
		{
		}

		bool const isInvalid() const
		{
			return (
				tradePeriodMinutes_m <=0 || cutoffVolume_m<=0	|| tradeVolume_m <=0 || maxMinutesLevelTrack_m <=0 ||
				maxPriceDifference_m <=0 || tp_relativeLevel_m <=0 || sl_relativeLevel_m <=0
			);
		}

		BEGIN_PARAM_ENTRIES_DECLARE(TradeParam)

			PARAM_ENTRY_DECLARE(HlpStruct::WorkTimeHolder, workTimeHolder_m);
			PARAM_ENTRY_DECLARE(int, tradePeriodMinutes_m);
			PARAM_ENTRY_DECLARE(double, cutoffVolume_m);
			// -1 means infinit
			PARAM_ENTRY_DECLARE(double, maxMinutesLevelTrack_m);
			PARAM_ENTRY_DECLARE(double, maxPriceDifference_m);
			PARAM_ENTRY_DECLARE(double, tradeVolume_m);
			PARAM_ENTRY_DECLARE(double, tp_relativeLevel_m);
			PARAM_ENTRY_DECLARE(double, sl_relativeLevel_m);
			PARAM_ENTRY_DECLARE(bool, isDemoTrade_m);
			
			
		END_PARAM_ENTRIES_DECLARE()

	};
};

#endif