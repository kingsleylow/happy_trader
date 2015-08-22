#ifndef _SYSTEM2_COMMONPARAMS_IMPL_INCLUDED
#define _SYSTEM2_COMMONPARAMS_IMPL_INCLUDED

namespace AlgLib {

	struct CommonParameters
	{
		CommonParameters()
		{
		}

		bool isWithinWorkingTime(double const &ttime) const
		{
			double diffFromDayBegin = ttime - CppUtils::roundToBeginningOfTheDay(ttime);

			if (diffFromDayBegin < unixTimeBeginTrade_m || diffFromDayBegin > unixTimeEndTrade_m)
				return false;
	
			return true;

		}

		bool isWithingSignalRaising(double const &ttime) const
		{
			double diffFromDayBegin = ttime - CppUtils::roundToBeginningOfTheDay(ttime);

			if (diffFromDayBegin < unixTimeBeginSignals_m || diffFromDayBegin > unixTimeEndTrade_m)
				return false;
	
			return true;
		}

		// members

		CppUtils::StringSet workSymbols_m;

		double unixTimeBeginTrade_m;

		double unixTimeBeginSignals_m;

		double unixTimeEndTrade_m;

		int hoursBegin_m;
		
		int minutesBegin_m;

		int hourEnd_m;
		
		int minutesEnd_m;

		// base path for export
		CppUtils::String exportBasePath_m;

		
	

		// this is provider containing RT stocks - laser provider
		CppUtils::String providerForRTSymbols_m;

	

	};


}; // end of namespace
#endif