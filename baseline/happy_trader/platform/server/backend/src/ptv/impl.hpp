#ifndef _PTV_IMP_INCLUDED
#define _PTV_IMP_INCLUDED

#include "../brklib/brklib.hpp"
#include "ptvdefs.hpp"
#include "ptv.hpp"

namespace AlgLib {

	// the structure containing what is calculated during the ady
	// must have the same for the previous day
	struct DaylyCalculation
	{
		DaylyCalculation()
		{
			clear();
		}

		inline bool isValid() const
		{
			return lowerChannel_m > 0;
		}

		void clear()
		{
			memset(this, '\0', sizeof(DaylyCalculation) );
		}

		
		// 0 - no trend
		// 1 - upper trend
		// -1 lower trend
		int trend_signal_m;

		// how many days no trend
		int days_no_trend_m;

		double dayOpen_m;

		double dayHigh_m;

		double dayLow_m;

		double dayClose_m;

		double dayATR_m;

		double upperChannel_m;

		double lowerChannel_m;
	
	};

	// these are signals fired 
	struct DaylyFire
	{
		DaylyFire()
		{
			clear();
		}

		void clear()
		{
			memset(this, '\0', sizeof(DaylyFire) );
		}

		// ----------------

		// channel breakthrough
		int lowChannelBreakThrough_m;

		int upperChannelBreakThrough_m;

				
		int lowChannelTouch_m;

		int upperChannelTouch_m;

	};

	struct TradeParams {
		TradeParams()
		{
			memset(this, '\0', sizeof(TradeParams));
		}

		bool isValid() const
		{
			return point_value_m > 0;
		}

		void clear() 
		{
			memset(this, '\0', sizeof(TradeParams));
		}

		int bars_no_trend_m;
		int trend_channel_m;

		// hours to work
		int hour_signal_begin_m;
		int hour_signal_end_m;

		int atr_period_m;
		double point_value_m; 

		// the number of days to remove orders
		int days_remove_orders_m;

	};

	// ------------------------------------

	class PTV_EXP AlgorithmPtv: public AlgorithmBase {

	public:

		// ctor & dtor
		AlgorithmPtv( Inqueue::AlgorithmHolder& algHolder);

		virtual ~AlgorithmPtv();
		
		// intreface to be implemented
		virtual void onLibraryInitialized(
			Inqueue::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		);

		virtual bool onEventArrived(HlpStruct::EventData const &eventdata, Inqueue::ContextAccessor& contextaccessor, HlpStruct::EventData& response);
	
		virtual bool onThreadStarted(
			Inqueue::ContextAccessor& contextaccessor
		);


		virtual CppUtils::String const& getName() const;
		
		// -----------------------

	private:

		inline bool isLong(BrkLib::PosType const posType) const
		{
			if (posType==BrkLib::POS_BUY || posType==BrkLib::POS_LIMIT_BUY || posType==BrkLib::POS_STOP_BUY)
				return true;
			else if (posType==BrkLib::POS_SELL || posType==BrkLib::POS_LIMIT_SELL || posType==BrkLib::POS_STOP_SELL)
				return false;
			
			THROW(CppUtils::OperationFailed, "exc_PositionIsNotInConsistentState", "ctx_IsPositionLong", "");
		}

		// helper allowing intercept alerts
		void orderSendHelper(BrkLib::BrokerOperation& oper, BrkLib::Order const& order);
		
		void sendHandshakeOrder(BrkLib::BrokerConnect& brkConnect);

		void sendMetainfoOrder(
			BrkLib::BrokerConnect& brkConnect, 
			CppUtils::String const& symbol 
		);

		// recalculate parameters for the last day
		void recalcLastDay(
			Inqueue::ContextAccessor& historyaccessor, 
			BrkLib::BrokerConnect& brkConnect, 
			TradeParams const& tradePars, 
			int const symbol_id 
		);

		// work with break through orders
		void eachTickBreakThroughOrderManage(
			HlpStruct::RtData const& rtdata,
			Inqueue::ContextAccessor& historyaccessor, 
			BrkLib::BrokerConnect& brkConnect, 
			TradeParams const& tradePars, 
			int const symbol_id 
		);

		void eachTickReflectionOrderManage(
			HlpStruct::RtData const& rtdata,
			Inqueue::ContextAccessor& historyaccessor, 
			BrkLib::BrokerConnect& brkConnect, 
			TradeParams const& tradePars, 
			int const symbol_id 
		);

		void lastDayBreakThroughOrderManage(
			HlpStruct::RtData const& rtdata,
			Inqueue::ContextAccessor& historyaccessor, 
			BrkLib::BrokerConnect& brkConnect, 
			TradeParams const& tradePars, 
			int const symbol_id 
		);

		void lastDayReflectionOrderManage(
			HlpStruct::RtData const& rtdata,
			Inqueue::ContextAccessor& historyaccessor, 
			BrkLib::BrokerConnect& brkConnect, 
			TradeParams const& tradePars, 
			int const symbol_id 
		);

		
	
		// --------------------------

		map<CppUtils::String, TradeParams> tradeParams_m;

		// map of breakthrough opened positions
		map<CppUtils::String, int> breakThroughPositionsState_m;

		// map of reflection positions
		map<CppUtils::String, int> reflectionPositionsState_m;

		// broker connect entry to reuse it
		BrkLib::BrokerConnect brokerConnect_m;

		// current and previous days
		DaylyCalculation currentDay_m;

		DaylyCalculation previousDay_m;

		// fired signals collected
		DaylyFire firedSignals_m;
			
	};

}; // end of namespace


#endif