#ifndef _SYSTEM2_SYMBOLCONTEXT_IMPL_INCLUDED
#define _SYSTEM2_SYMBOLCONTEXT_IMPL_INCLUDED

namespace AlgLib {

	struct SymbolContext
	{
		SymbolContext():
			initialized_m(false),
			SDR_m(-1),
			ADR_m(-1),
			ADM_m(-1),

			UPDN_m(1),
			UPDN_PREV_m(0),

			LONGSTOP_m(0),
			SHORTSTOP_m(999999),
			SHORTSTOP_PREV_m(-1),
			LONGSTOP_PREV_m(-1)
			
		{
		}

		
		double SDR_m;
		double ADR_m;
		double ADM_m;

		bool initialized_m;

		// current file where to write
		CppUtils::String exportPath_m; 

		// container to hold true range to apply RSI then
		deque<HlpStruct::PriceData> rsiSeries_m;

		// modes - e.i. what positions do we have
		int UPDN_m;

		int UPDN_PREV_m;

		double LONGSTOP_m;

		double SHORTSTOP_m;

		double SHORTSTOP_PREV_m;

		double LONGSTOP_PREV_m;


		// if pos is opened  there is an open pos ID for the first TP
		CppUtils::String openPosId1_m;
		
		// open pos ID for the second TP
		CppUtils::String openPosId2_m;

				
		


	};


}; // end of namespace
#endif