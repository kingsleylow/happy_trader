#include "solverimpl.hpp"
#include "implement.hpp"
#include "event.hpp"

namespace AlgLib {

	LevelCalculator::LevelCalculator( TradingSequenceRobot& base):
		base_m(base),
		prevLevel_m(-1)
	{
	}
			
	LevelCalculator::~LevelCalculator()
	{
	}

	void LevelCalculator::onRtData(HlpStruct::RtData const& rtdata, void*)
	{
		
	}

	void LevelCalculator::onLevel2Data(HlpStruct::RtLevel2Data const& level2data, void*)
	{
		// don't do anything if no price is initialized
		//if (currentPrice_m < 0)
		//	return;

		if (level2data.getBestAskIndex() <0  || level2data.getBestBidIndex() < 0)
			return;
			

	
		double newBidLevel = -1, newAskLevel = -1;
		// assume price change from min to max
		// first we are in buy zone then we are in sell zone
		//thus:

		int bid_idx = 0, ask_idx = 0;
		// buy zone
		for(int i = level2data.getBestBidIndex(); i >= 0; i--) {
			HlpStruct::Level2Entry const& le= level2data.getLevel2Entry(i);
			if (le.getType() != HlpStruct::Level2Entry::IS_BID) 
				return;

			if (base_m.getTradeParam().cutoffVolume_m	<= le.getVolume()) {
				newBidLevel = le.getPrice();
				break;
			}

			bid_idx++;
				
		}

		// sell zone
		for(int i = level2data.getBestAskIndex(); i < level2data.getSize(); i++) {
			HlpStruct::Level2Entry const& le= level2data.getLevel2Entry(i);
			if (le.getType() != HlpStruct::Level2Entry::IS_ASK) 
				return;

			if (base_m.getTradeParam().cutoffVolume_m	<= le.getVolume()) {
				newAskLevel = le.getPrice(); 
				break;
			}

			ask_idx++;			  
		}

		BrkLib::TradeDirection td = BrkLib::TD_NONE;
		double newLevelPrice = -1;
		if (newBidLevel > 0 && newAskLevel < 0 ) {
			 newLevelPrice = newBidLevel;
			 td = BrkLib::TD_LONG;
		}
		else if (newBidLevel < 0 && newAskLevel > 0) {
			newLevelPrice = newAskLevel;
			td = BrkLib::TD_SHORT;
		}
		else if (newBidLevel > 0 && newAskLevel > 0) {
			if (ask_idx < bid_idx) {
				// ask is prevailing
				newLevelPrice = newAskLevel;
				td = BrkLib::TD_SHORT;
			}
			else {
				// bid is prevailing
				newLevelPrice = newBidLevel;
				td = BrkLib::TD_LONG;
			}
		}
	
		if (!Math::PriceEquality<4>::isEqual(prevLevel_m, newLevelPrice)) {
			// new level

			if (prevLevel_m > 0) {
				// notify lost old
				TE_Level_Lost e;
				base_m.getTradingMachines().processEvent<TE_Level_Lost>(level2data.getSymbol(), e);
			}

			// notify new
			TE_Level_Detected e;
			e.detectedPriceLevel_m = newLevelPrice;
			e.direction_m = td;
			base_m.getTradingMachines().processEvent<TE_Level_Detected>(level2data.getSymbol(), e);
		}
		


		// store
		prevLevel_m = newLevelPrice;
		
			
		
	}

};  // end of namespace