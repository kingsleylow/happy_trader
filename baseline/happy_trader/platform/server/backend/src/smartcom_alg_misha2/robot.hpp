#ifndef _SMARTCOM_ALG_MISHA2_ROBOT_INCLUDED
#define _SMARTCOM_ALG_MISHA2_ROBOT_INCLUDED

 // Many thanks to Zoubok Victor

#include "smartcom_alg_misha2defs.hpp"
#include "tradeparams.hpp"
#include "event.hpp"
#include "state.hpp"
#include "action.hpp"
#include "brokerthroughcontext.hpp"
#include "brokerdialog.hpp"
#include "solverimpl.hpp"
#include "globaltardingdata.hpp"


namespace AlgLib {
	/**
	* This class is going to be executed sequentially
	* Necessary when we want to try with different parameters
	*/

	class CAlgMisha2;
	class TradingSequenceRobot
	{
		public:
			
			TradingSequenceRobot(CAlgMisha2& base);

			~TradingSequenceRobot();

		
			// ------------

			

			void init(
				HlpStruct::AlgBrokerDescriptor const& descriptor, 
				BrkLib::BrokerBase* brokerlib, 
				CppUtils::String const& runName,
				CppUtils::String const& comment,
				TradeParam const& tradeParam, 
				CppUtils::StringSet const& symbols
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

			// --------------------

			

			inline CppUtils::Uid const& getId() const
			{
				return uid_m;
			}

			inline TradeParam const& getTradeParam() const {
				return tradeParam_m;
			}
			
			inline CppUtils::FSMMap<	MachineContext >& getTradingMachines() {
				return tradingMachines_m;
			}

			inline CAlgMisha2& getBase()
			{
				return base_m;
			};

			inline HlpStruct::StorageProxy<GlobalTradingContext> getGlobalTradeContext()
			{
				return globalTradeContext_m.data();
			}


			BrokerDialog& resolveBrokerDialog();

		private:

			void initStateMachine(CppUtils::StringSet const& symbols);

			void deinitStateMachine();
						
			

	private:

			CAlgMisha2& base_m;

			BrokerDialog brokerDialogReal_m;

			BrokerDialog brokerDialogDemo_m;

			CppUtils::FSMMap<	MachineContext > tradingMachines_m;

			TradeParam tradeParam_m;

			// global trading context, thread safe
			HlpStruct::TradeGlobalStorage<GlobalTradingContext> globalTradeContext_m;

			// -------------

			CppUtils::Uid uid_m;

			LevelCalculator calculatorLevel_m;

	};

};


#endif