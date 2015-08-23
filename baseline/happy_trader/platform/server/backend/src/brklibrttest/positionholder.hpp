#ifndef _BRKLIBRTTEST_POSITIONHOLDER_INCLUDED
#define _BRKLIBRTTEST_POSITIONHOLDER_INCLUDED

#include "brklibrttestdefs.hpp"
#include "commonstruct.hpp"

namespace BrkLib {


  class BrokerEngine;
	
	typedef map< CppUtils::String, map<CppUtils::String, PositionsDataMap > > PositionHolderMap;

	class PositionHolder: public CppUtils::Mutex {
	public:
		
		class PositionHolderCallback
		{
		public:
			// after position closed
			virtual void onPositionUpdate(
				CppUtils::String const& provider, 
				CppUtils::String const& symbol, 
				PositionForSymbol const& position
			) = 0;

			virtual void onRequireBuyOperation(
				CppUtils::String const& provider, 
				CppUtils::String const& symbol, 
				double const& price, 
				double const& volume, 
				double const& ttime, 
				BrokerSession& brkSess, 
				OrderOperation const& oper
			) = 0;

			virtual void onRequireSellOperation(
				CppUtils::String const& provider, 
				CppUtils::String const& symbol, 
				double const& price, 
				double const& volume, 
				double const& ttime, 
				BrokerSession& brkSess, 
				OrderOperation const& oper
			) = 0;
		
		};

		PositionHolder(OperationProcessorCallBack& processorCallback, BrokerEngine& brokerEngine, PositionHolderCallback& collback);
		
		~PositionHolder();

		 void initialize(bool const onlyOnePositionPerSymbol);

		 
		 /**
		 * This set of high-levle functions works differently depending if we would like to keep many positions for same symbols or only one
		 */

		 // it either open new or add to existing	depending on mode and positionId
		 // if it is new one it initializws pozition id
		 void goLong(
			 CppUtils::String const& provider, 
			 CppUtils::String const& symbol, 
			 double const& tradeTime, 
			 double const& tradePrice, 
			 double const& volume,
			 CppUtils::Uid& positionId,
			 BrokerSession* brkSess,
			 OrderOperation const& oper
			);

		 // it either open new or add to existing, depending on mode and positionId
		 // if it is new one it initializees position id
		 void goShort(
			 CppUtils::String const& provider, 
			 CppUtils::String const& symbol, 
			 double const& tradeTime, 
			 double const& tradePrice, 
			 double const& volume,
			 CppUtils::Uid& positionId,
			 BrokerSession* brkSess,
			 OrderOperation const& oper
		);

		inline bool isOnlyOnePositionPerSymbol() const
		{
			return onlyOnePositionPerSymbol_m;
		}

		void queryOpenPositions(PositionList& posList) const;

		void queryHistoryPositions(PositionList& posList) const;

		
		
	private:
		void queryPositions(PositionList& posList, PositionHolderMap const& posHolderMap, PosState const posState) const;


		// create position
		 CppUtils::Uid createPosition(
			 CppUtils::String const& provider, 
			 CppUtils::String const& symbol, 
			 TradeDirection const direction, 
			 double const& tradeTime, 
			 double const& tradePrice, 
			 double const& volume,
			 BrokerSession* brkSess,
			 OrderOperation const& oper
		);

		 /**
		  Increase position
		 */

		 void increasePosition(
			 PositionForSymbol& cur_pos,
			 CppUtils::String const& provider, 
			 CppUtils::String const& symbol,  
			 CppUtils::Uid const& positionId, 
			 double const& tradeTime, 
			 double const& tradePrice, 
			 double const& volume,
			 BrokerSession* brkSess,
			 OrderOperation const& oper,
			 TradeDirection const& direction
			);

		 /*
		 * Decrease position and closes if it reaches zero. If necessary opens opposite position - if closing volume is more than imbalance
		 */

		 void decreasePosition(
			 PositionsDataMap& openPosMap,
			 PositionForSymbol& cur_pos,
			 CppUtils::String const& provider, 
			 CppUtils::String const& symbol,  
			 CppUtils::Uid const& positionId, 
			 double const& tradeTime, 
			 double const& tradePrice, 
			 double const& volume,
			 BrokerSession* brkSess,
			 OrderOperation const& oper,
			 TradeDirection const& direction
			);


		void resolveOpenPositionMap(CppUtils::String const& provider, CppUtils::String const& symbol, PositionsDataMap **posDataMap);
		
		void resolvePositionInMap(PositionsDataMap& posMap, CppUtils::Uid const& positionId, PositionForSymbol** posForSymbol);
		
		PositionsDataMap& resolveOpenPositionMap(CppUtils::String const& provider, CppUtils::String const& symbol);

		PositionForSymbol& resolvePositionInMap(PositionsDataMap& posMap, CppUtils::Uid const& positionId);

		void removePositionInMap(CppUtils::String const& provider, CppUtils::String const& symbol, PositionsDataMap& posMap, CppUtils::Uid const& positionId);

	

		 inline BrokerEngine& getBrokerEngine()
		 {
			 return brokerEngine_m;
		 }

		 inline BrokerEngine const& getBrokerEngine() const
		 {
			 return brokerEngine_m;
		 }

		 

	private:

		void checkValidity(
				double const& tradeTime, 
				double const& tradePrice, 
				double const& volume
		) const;

		PositionHolderCallback& callback_m;

		BrokerEngine& brokerEngine_m;

		// map of positions
		PositionHolderMap openPositions_m;

		PositionHolderMap historyPositions_m;
	
		bool onlyOnePositionPerSymbol_m;

		OperationProcessorCallBack &processorCallback_m;
	};
};

#endif