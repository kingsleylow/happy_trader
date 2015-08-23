#ifndef _SYMBOLCONTEXT_IMPL_INCLUDED
#define _SYMBOLCONTEXT_IMPL_INCLUDED

namespace AlgLib {

	struct SymbolContext
	{
		SymbolContext():
			lastDayClose_m(-1),
			dateLastDayClose_m(-1),
			maxDay5minVolume_m(-1),
			currentHigh_m(-1),
			currentLow_m(-1),
			volume_signal_5MIN_m(false)
		{
		}

		double lastDayClose_m;

		double dateLastDayClose_m;

		double maxDay5minVolume_m; // this is the maximum volume for this day

		double currentHigh_m; // current high 

		double currentLow_m; // and current low

		// signals 
		// this signals is checked every 5 minutes
		// and will be valid until the next 5 min interval
		bool volume_signal_5MIN_m;

	};


}; // end of namespace
#endif