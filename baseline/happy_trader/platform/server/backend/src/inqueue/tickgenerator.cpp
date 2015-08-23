#include "tickgenerator.hpp" 

namespace Inqueue {

	


SimulationGenerator::SimulationGenerator(
	HlpStruct::TickGenerator const generType,
	double const& simCandlePeriod,
	HlpStruct::TickUsage const whatKindOfPriceToUse,
	int const nticks
):
type_m(generType),
simCandlePeriod_m(simCandlePeriod),
whatKindOfPriceToUse_m(whatKindOfPriceToUse),
nticks_m(nticks)
{
	// init random generator
	srand((int)CppUtils::getTime());

	if (whatKindOfPriceToUse_m == HlpStruct::PC_USE_DUMMY)
		THROW(CppUtils::OperationFailed, "exc_InvalidPriceType", "ctx_CreateSimulationEngine", "");

	if (nticks_m < 4 && type_m==HlpStruct::TG_Uniform)
			THROW(CppUtils::OperationFailed, "exc_TheNumberOfTicksMustBeMoreOrEqualThanFour", "ctx_CreateSimulationEngine" , (long)nticks_m);


	diff_m = (simCandlePeriod_m*60.0 - 0.004)/nticks_m;


	
}

SimulationGenerator::~SimulationGenerator()
{
}

void SimulationGenerator::generateSimulationTicks(
			HlpStruct::PriceData const& currentcandle, 
			double const& simulationSpread
)
{
		ticks_m.clear();

		/*
		bool isask = false, isbid = false, isvolume=false;
		if (currentcandle.open_m > 0 && currentcandle.high_m > 0 && currentcandle.low_m > 0 && currentcandle.close_m > 0)
				isask = true;

		if (currentcandle.open2_m > 0 && currentcandle.high2_m > 0 && currentcandle.low2_m > 0 && currentcandle.close2_m > 0)
				isbid = true;
		*/
		
		bool isvolume = (currentcandle.volume_m > 0);


		// returns firstcandle but do not return last candle
		if (type_m==HlpStruct::TG_Uniform) {
			// in that mode only 4 or more ticks are allowed

			double t1 = currentcandle.time_m + 0.002;
	
		
			double volumechunk = -1;
			if (isvolume)
				volumechunk = currentcandle.volume_m/nticks_m;

			// default indexes for low and high idx
			int lowidx, highidx;
			
			if (nticks_m == 4) {
				lowidx = 1;
				highidx = 2;
			}
			else if (nticks_m == 5) {
				lowidx = generateRandom(1, 2);
				highidx = 3;
			}
			else if (nticks_m >=6) {
				do {
					lowidx = generateRandom(1, nticks_m - 2);
					highidx = generateRandom(1, nticks_m - 2);
				} while (lowidx == highidx);
			}
			

			HlpStruct::RtData rtdata;
			int lastidx = nticks_m-1;

			for(int i = 0; i < nticks_m; i++) {
				//
				rtdata.clear();

				rtdata.getAFlag() = HlpStruct::RtData::AT_NonClosed;
				rtdata.getType() = HlpStruct::RtData::RT_Type;
				//rtdata.setProvider( provider_m.c_str() );
				//rtdata.setSymbol( symbol_m.c_str() );

				// adjust time
				rtdata.getTime() = t1 + diff_m*i;
				rtdata.getRtData().volume_m = volumechunk;
				
				
				// first tick
				if (i==0) {
					if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_ASK ) {

						rtdata.getRtData().ask_m = currentcandle.open_m;
						rtdata.getRtData().bid_m = rtdata.getRtData().ask_m - simulationSpread;

					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BID) {
						rtdata.getRtData().bid_m = currentcandle.open2_m;
						rtdata.getRtData().ask_m = currentcandle.open2_m + simulationSpread;
	
					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BOTH) {
						rtdata.getRtData().ask_m = currentcandle.open_m;
						rtdata.getRtData().bid_m = currentcandle.open2_m;
					}

				}
				// last tick
				else if (i==lastidx) {

					if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_ASK ) {

						rtdata.getRtData().ask_m = currentcandle.close_m;
						rtdata.getRtData().bid_m = rtdata.getRtData().ask_m - simulationSpread;

					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BID) {
						rtdata.getRtData().bid_m = currentcandle.close2_m;
						rtdata.getRtData().ask_m = currentcandle.close2_m + simulationSpread;
	
					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BOTH) {
						rtdata.getRtData().ask_m = currentcandle.close_m;
						rtdata.getRtData().bid_m = currentcandle.close2_m;
					}

					

				}
				else if (i==lowidx) {
					// low index
					if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_ASK ) {

						rtdata.getRtData().ask_m = currentcandle.low_m;
						rtdata.getRtData().bid_m = rtdata.getRtData().ask_m - simulationSpread;

					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BID) {
						rtdata.getRtData().bid_m = currentcandle.low2_m;
						rtdata.getRtData().ask_m = currentcandle.low2_m + simulationSpread;
	
					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BOTH) {
						rtdata.getRtData().ask_m = currentcandle.low_m;
						rtdata.getRtData().bid_m = currentcandle.low2_m;
					}

				}
				else if (i==highidx) {
					// high index
					if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_ASK ) {

						rtdata.getRtData().ask_m = currentcandle.high_m;
						rtdata.getRtData().bid_m = rtdata.getRtData().ask_m - simulationSpread;

					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BID) {
						rtdata.getRtData().bid_m = currentcandle.high2_m;
						rtdata.getRtData().ask_m = currentcandle.high2_m + simulationSpread;
	
					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BOTH) {
						rtdata.getRtData().ask_m = currentcandle.high_m;
						rtdata.getRtData().bid_m = currentcandle.high2_m;
					}

				}
				else if (nticks_m > 4) {

					// generate random here
					if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_ASK ) {
						rtdata.getRtData().ask_m = generateRandom(currentcandle.low_m, currentcandle.high_m);
						rtdata.getRtData().bid_m = rtdata.getRtData().ask_m - simulationSpread;
					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BID ) {	
						rtdata.getRtData().bid_m = generateRandom(currentcandle.low2_m, currentcandle.high2_m);
						rtdata.getRtData().ask_m = rtdata.getRtData().bid_m + simulationSpread;
					}
					else if (whatKindOfPriceToUse_m==HlpStruct::PC_USE_BOTH) {
							rtdata.getRtData().bid_m = generateRandom(currentcandle.low2_m, currentcandle.high2_m);
							rtdata.getRtData().ask_m = generateRandom(currentcandle.low_m, currentcandle.high_m);
					}
				}
				else 
					HTASSERT(false);

				// 
				ticks_m.push_back(rtdata);

			} // end loop
			
		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidSimulationType", "ctx_generateSimulationticks_m", (long)type_m)
}





int SimulationGenerator::generateRandom(int const lower, int const upper)
{
		//HTASSERT(upper >= lower);

		double range = upper-lower;
		double rnd = (double)rand();

		rnd *= range/RAND_MAX;

		int result = lower + rnd;

		return result;
	}

	double SimulationGenerator::generateRandom(double const lower, double const upper)
	{
		//HTASSERT(upper >= lower);

		double range = upper-lower;
		double rnd = (double)rand();

		rnd *= range/RAND_MAX;

		double result = lower + rnd;

		return result;
	}

}; // end of namespace