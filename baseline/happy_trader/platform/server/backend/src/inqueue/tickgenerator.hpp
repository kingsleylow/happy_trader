#ifndef _BACKEND_INQUEUE_TICKGENERATOR_INCLUDED
#define _BACKEND_INQUEUE_TICKGENERATOR_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "../brklib/brklib.hpp"

namespace Inqueue {

	/**
	 This is class for simulation ticks generation, used by EngineThread
	*/
	class SimulationGenerator {
	
	public:
		

		SimulationGenerator(
			HlpStruct::TickGenerator const generType, 
			double const& simCandlePeriod, // this is the length of simulation candle in minutes
			HlpStruct::TickUsage const whatKindOfPriceToUse,
			int const nticks
		);

		~SimulationGenerator();

		// generate chunk of ticks 
		void generateSimulationTicks(HlpStruct::PriceData const& currentcandle, double const& simulationSpread);
		
		inline int SimulationGenerator::getTicksNumber() const
		{
			return ticks_m.size();
		}

		
		inline HlpStruct::RtData& getTick(int const idx)
		{
			return ticks_m[idx];
		}
	
	private:

		int generateRandom(int const lower, int const upper);

		double generateRandom(double const lower, double const upper);

		
		// type of generation
		HlpStruct::TickGenerator type_m;
		
		// currently generated ticks
		HlpStruct::RtDataList ticks_m;
	
		//this is the length of simulation candle in minutes
		double simCandlePeriod_m;

		// 2 ways of setting the spread
		//


		// whether we use bid, ask or both prices on simulation
		HlpStruct::TickUsage whatKindOfPriceToUse_m;

		// the number of ticks
		int nticks_m;

		double diff_m;



	};


}; // end of namespace

#endif
