#ifndef _SMARTCOM_ALG_MISHA2_SOLVERIMPL_INCLUDED
#define _SMARTCOM_ALG_MISHA2_SOLVERIMPL_INCLUDED

#include "smartcom_alg_misha2defs.hpp"
#include "statemachinecontext.hpp"
#include "event.hpp"




namespace AlgLib {
	class CAlgMisha2; 
	class TradingSequenceRobot;

	class LevelCalculator: public HlpStruct::CalculatorBase<void>
	{
		public:
			LevelCalculator(TradingSequenceRobot& base);
			
			virtual ~LevelCalculator();

			virtual void onRtData(HlpStruct::RtData const& rtdata, void*);

			virtual void onLevel2Data(HlpStruct::RtLevel2Data const& level2data, void*);

			virtual void onThreadStarted(void*)
			{
			}

			virtual void onThreadFinished(void*)
			{
			}

			virtual void initialized()
			{
			}

			virtual void deinitialized()
			{
			}

	private:

	
				
		TradingSequenceRobot& base_m;

		// previous level
		
		double prevLevel_m;
		
	};
};

#endif