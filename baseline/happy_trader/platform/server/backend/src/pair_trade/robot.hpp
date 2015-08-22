#ifndef _PAIR_TARDE_ROBOT_INCLUDED
#define _PAIR_TARDE_ROBOT_INCLUDED

 // Many thanks to Zoubok Victor

#include "pair_tradedefs.hpp"
#include "tradeparams.hpp"
#include "event.hpp"
#include "state.hpp"
#include "action.hpp"
#include "brokerthroughcontext.hpp"
#include "brokerdialog.hpp"
#include "solverimpl.hpp"
#include "globaltardingdata.hpp"
#include "commonstruct.hpp"


namespace AlgLib {
	/**
	* This class is going to be executed sequentially
	* Necessary when we want to try with different parameters
	*/



	class CPairTrade;
	class TradingSequenceRobot
	{
		public:
			
			TradingSequenceRobot(CPairTrade& base);

			~TradingSequenceRobot();

		
			// ------------

			

			void init(
				HlpStruct::AlgBrokerDescriptor const& descriptor, 
				BrkLib::BrokerBase* brokerlib, 
				CppUtils::String const& runName,
				CppUtils::String const& comment,
				TradeParam const& tradeParam, 
				CppUtils::StringMap const& symbolsFinam,
				CppUtils::StringMap const& symbolsSmartcom,
				CppUtils::StringMap const& symbolsSmartcomReverse, 
				CppUtils::StringSet const& syntheticSmartcomList
			);

			void deinit();
			

			void onDataArrived(
				HlpStruct::RtData const& rtdata, 
				Inqueue::ContextAccessor& historyaccessor, 
				HlpStruct::TradingParameters const& descriptor,
				BrkLib::BrokerBase* brokerlib,
				CppUtils::String const& runName,
				CppUtils::String const& comment
			);
			
			void onRtProviderSynchEvent (
				Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
				double const& synchTime,
				CppUtils::String const& providerName,
				CppUtils::String const& message
			);

			void onLevel2DataArrived(
				HlpStruct::RtLevel2Data const& level2data,
				Inqueue::ContextAccessor& historyaccessor, 
				HlpStruct::TradingParameters const& descriptor,
				BrkLib::BrokerBase* brokerlib,
				CppUtils::String const& runName,
				CppUtils::String const& comment
			);

			void onLevel1DataArrived(
				HlpStruct::RtLevel1Data const& level1data,
				Inqueue::ContextAccessor& historyaccessor, 
				HlpStruct::TradingParameters const& descriptor,
				BrkLib::BrokerBase* brokerlib,
				CppUtils::String const& runName,
				CppUtils::String const& comment
			);

			void onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib );

			void onThreadFinished(Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib );


			// --------------------

			

			inline CppUtils::Uid const& getId() const
			{
				return uid_m;
			}

			inline TradeParam const& getTradeParam() const
			{
				return tradeParam_m;
			}

			
			inline CppUtils::FSM<	MachineContext >& getTradingMachine() {
				return tradingMachine_m;
			}

			inline CPairTrade& getBase()
			{
				return base_m;
			};

			inline HlpStruct::StorageProxy<GlobalTradingContext> getGlobalTradeContext()
			{
				return globalTradeContext_m.data();
			}

			inline CppUtils::StringMap const& getFinamSymbols() const
			{
				return symbolsFinamList_m;
			}

			// map real name -> pseudonym
			inline CppUtils::StringMap const& getSmartcomSymbols() const
			{
				return symbolsSmartcomList_m;
			}

			// map pseudonym -> real name
			inline CppUtils::StringMap const& getSmartcomSymbolsReverse() const
			{
				return symbolsSmartcomReverse_m;
			}

			inline CppUtils::StringSet const& getSyntheticSmartcomSymbols() const
			{
				return syntheticSmartcomList_m;
			}

			inline LevelCalculator const& getLevelCalculator() const
			{
				return calculatorLevel_m;
			}

			inline LevelCalculator& getLevelCalculator()
			{
				return calculatorLevel_m;
			}

			BrokerDialog& resolveBrokerDialog();

			void simulateUp();

			void simulateDown();

			// send initial event notifying we are we are
			void sendInitialEvent(BrkLib::TradeDirection const init_position);

			// send event stopping trade
			void sendStopTradeEvent();



		private:

			void initStateMachine();

			void deinitStateMachine();
						
			

	private:

			CPairTrade& base_m;

			BrokerDialog brokerDialogReal_m;

			BrokerDialog brokerDialogDemo_m;

			CppUtils::FSM<	MachineContext > tradingMachine_m;

			TradeParam tradeParam_m;

			// global trading context, thread safe
			HlpStruct::TradeGlobalStorage<GlobalTradingContext> globalTradeContext_m;

			// -------------

			CppUtils::Uid uid_m;

			LevelCalculator calculatorLevel_m;

		

			// this is going to be symbols lists 
			// including synthetic
			// this sysmbols must go as synonim, symbol
			// so we haev a map
			CppUtils::StringMap symbolsFinamList_m;

			CppUtils::StringMap symbolsSmartcomList_m;

			CppUtils::StringMap symbolsSmartcomReverse_m; 

			CppUtils::StringSet syntheticSmartcomList_m;

			
			
	};

};


#endif