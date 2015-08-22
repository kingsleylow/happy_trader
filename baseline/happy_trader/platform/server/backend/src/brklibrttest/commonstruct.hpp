#ifndef _BRKLIBRTTEST_COMMONSTRUCT_INCLUDED
#define _BRKLIBRTTEST_COMMONSTRUCT_INCLUDED

#include "brklibrttestdefs.hpp"

namespace BrkLib {

	// ----------------------------------------------
	// helper functions

	struct OrderOperation;
	struct Price;
	enum OrderOperationType;
	enum PositionTradeAction;

	CppUtils::String getCommonOperationDescription(OrderOperation const& oper,Price const& p);

	CppUtils::String orderOperationType2String(OrderOperationType const oper_type);

	CppUtils::String positionTradeAction2String(PositionTradeAction const pt);
	// ----------------------------------------------

	struct Price {
		
		Price(double const& bid, double const ask, double const ttime);
		Price();
		Price& operator = (Price const& src);
		void create(double const& bid, double const ask, double const ttime);
		void clear();
		bool isValid() const;
		CppUtils::String toString() const;
		
		// ------

		double bid_m;
		double ask_m;
		double time_m;
	};

	// ------------------------------------------------

	struct PositionEntry
	{
		PositionEntry(double const& ttime, double const& tprice, double const& volume):
			ttime_m(ttime),
			tprice_m(tprice),
			volume_m(volume)
		{
		};

		PositionEntry():
			ttime_m(-1),
			tprice_m(-1),
			volume_m(-1)
		{
		}

		CppUtils::String toString() const
		{

			CppUtils::StringConcat r;

			r << STRINGCONB << 
				STRINGCONTPAIR_DATE(ttime_m) <<
				STRINGCONTPAIR(tprice_m) << 
				STRINGCONTPAIR(volume_m) << 
				STRINGCONE;

			return r.getString();
		}

		double ttime_m;
		double tprice_m;
		double volume_m;

	};

	

	struct PositionForSymbol
	{
		PositionForSymbol();

		double getAvrOpenPrice() const;
		double getAvrClosePrice() const;
		double getFullOpenVolume() const;
		double getFullCloseVolume() const;
		double getOpenTime() const;
		double getCloseTime() const;

		CppUtils::String toString() const;
		
		void applyDecreaseOperation(
			BrokerSession* brksession, 
			PositionEntry const & entry, 
			double const& imbalanceVolume, 
			bool const history
		);

		// -------------------------------

		vector<PositionEntry> opens_m;
		vector<PositionEntry> closes_m;
		double imbalanceVolume_m;
		TradeDirection direction_m;
		bool history_m;
		BrokerSession* brkSession_m;

		CppUtils::Uid positionUid_m;

	};

	struct PositionsDataMap
	{
		map<CppUtils::Uid, PositionForSymbol> positionsMap_m;
	};

	// --------------------------------------------

	enum PositionTradeAction
	{
		PTA_Dummy =    0,
		PTA_Create	=  1,
		PTA_Increase = 2,
		PTA_Decrease = 3
	};


	enum OrderOperationType {
		OOT_Market_Buy,
		OOT_Market_Sell,
		OOT_Limit_Buy,
		OOT_Limit_Sell,
		OOT_Stop_Buy,
		OOT_Stop_Sell,
		OOT_Stop_Limit_Buy,
		OOT_Stop_Limit_Sell,
		OOT_Cancel,
		OOT_Move,
		OOT_Dummy
	};

	
	

	

	struct OrderOperation
	{
		OrderOperation();

		OrderOperation(
			OrderOperationType const& type, 
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			BrokerSession* brkSession,
			double const& volume,
			double const& price,
			double const& stopprice,
			SymbolTradingInfo const& tinfo,
			CppUtils::Uid const& cancelOrderId,
			CppUtils::Uid const& moveOrderId,
			CppUtils::Uid const& orderUid,
			CppUtils::Uid const& affectedPositionId,
			Order const& order
		);
				

		bool isStopOperration() const;

		CppUtils::String toString() const;
		

		OrderOperationType type_m;
		CppUtils::String provider_m;
		CppUtils::String symbol_m;
		BrokerSession* brkSession_m; // broker session
		double volume_m; // volume
		double price_m; // only for limit orders
		double stopPrice_m; // only for stop orders
		SymbolTradingInfo tinfo_m;
		CppUtils::Uid cancelOrderId_m; // on cancellations
		CppUtils::Uid moveOrderId_m; // for movements
		CppUtils::Uid orderUid_m; // this is the same as order_m.ID_m but in some cases, like stop_limit_... it is another unique ID
		CppUtils::Uid affectedPositionId_m; // in some cases when we want to perform operation against already existing position id -...
		Order order_m;

	};

	
	// ----------------------------

	struct OperationStructureBase
	{
		OperationStructureBase():
			brkSession_m(NULL)
		{
		}

		OperationStructureBase(
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			BrokerSession* brkSession
		):
		brkSession_m(brkSession),
		provider_m(provider),
		symbol_m(symbol)
		{
		}

		CppUtils::String toString() const
		{
			CppUtils::StringConcat r;
			r <<	STRINGCONTPAIR(provider_m) << STRINGCONTPAIR(symbol_m);
			return r.getString();
		}

		CppUtils::String provider_m;
		CppUtils::String symbol_m;
		BrokerSession* brkSession_m;
	};

	struct OperationStructureInstall: public OperationStructureBase
	{
		OperationStructureInstall(
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			double const& installTime,
			double const& installPrice,
			double const& volume,
			BrokerSession* brkSession,
			OrderOperationType const& operType
		):
			OperationStructureBase(provider, symbol, brkSession),
			installTime_m(installTime),
			installPrice_m(installPrice),
			volume_m(volume),
			operType_m(operType)
		{
		}

		CppUtils::String toString() const
		{
			CppUtils::StringConcat r;

			r << STRINGCONB << OperationStructureBase::toString() <<
				STRINGCONTPAIR_FUN(orderOperationType2String, operType_m) <<
				STRINGCONTPAIR_DATE(installTime_m) <<
				STRINGCONTPAIR(installPrice_m) <<
				STRINGCONTPAIR(volume_m) << STRINGCONE;


			return r.getString();
		};

		OrderOperationType operType_m;	// the type of operation that was installed or executed
		double installTime_m;
		double installPrice_m;
		double volume_m;
	};

	struct OperationStructureExecute: public OperationStructureBase
	{
		OperationStructureExecute(
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			double const& executeTime,
			double const& executePrice,
			double const& volume,
			BrokerSession* brkSession,
			CppUtils::Uid const& positionId,
			TradeDirection const& tradeDirection,
			PositionTradeAction const& ta
		):
			OperationStructureBase(provider, symbol, brkSession),
			executeTime_m(executeTime),
			executePrice_m(executePrice),
			volume_m(volume),
			positionId_m(positionId),
			tradeDirection_m(tradeDirection),
			ta_m(ta)
		{
		}

		CppUtils::String toString() const
		{
			CppUtils::StringConcat r;

			r << STRINGCONB << OperationStructureBase::toString() <<
				STRINGCONTPAIR_FUN(tradeDirection2String, tradeDirection_m) <<
				STRINGCONTPAIR_DATE(executeTime_m) <<
				STRINGCONTPAIR(executePrice_m) <<
				STRINGCONTPAIR(volume_m) << STRINGCONE ;


			return r.getString();
		};

		TradeDirection tradeDirection_m;	// as this is execution just need trade direction
		PositionTradeAction ta_m;
		double executeTime_m;
		double executePrice_m;
		double volume_m;
		CppUtils::Uid positionId_m;
	};

	struct OperationStructureCancel: public OperationStructureBase
	{
		OperationStructureCancel(
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			BrokerSession* brkSession,
			CppUtils::Uid const& cancelOrderId,
			double const& cancelTime
		):
			OperationStructureBase(provider, symbol, brkSession),
			cancelTime_m(cancelTime),
			cancelOrderUid_m(cancelOrderId)
		{
		}

		CppUtils::String toString() const
		{
			CppUtils::StringConcat r;

			r << STRINGCONB << OperationStructureBase::toString() <<
				STRINGCONTPAIR_DATE(cancelTime_m) <<
				STRINGCONTPAIR(cancelOrderUid_m) << STRINGCONE;

			return r.getString();
		};


		double cancelTime_m;
		CppUtils::Uid cancelOrderUid_m;
	};

	struct OperationStructureMove: public OperationStructureBase
	{
		OperationStructureMove(
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			BrokerSession* brkSession,
			CppUtils::Uid const& moveOrderId,
			double const& moveTime
		):
			OperationStructureBase(provider, symbol, brkSession),
			moveTime_m(moveTime),
			movedOrderUid_m(moveOrderId)
		{
		}

		CppUtils::String toString() const
		{
			CppUtils::StringConcat r;

			r << STRINGCONB << OperationStructureBase::toString() <<
				STRINGCONTPAIR(moveTime_m) <<
				STRINGCONTPAIR(movedOrderUid_m) << STRINGCONE;

			return r.getString();
		};

		double moveTime_m;
		CppUtils::Uid movedOrderUid_m;
	};

	//call back for operations
	class OperationProcessorCallBack
	{
		 public:
				virtual void onInstallationProcedure(OperationStructureInstall const& os, OrderOperation const& oper) = 0;

				virtual void onExecutionProcedure(OperationStructureExecute const& os, OrderOperation const& oper) = 0;

				virtual void onCancellationProcedure(OperationStructureCancel const& os, OrderOperation const& oper, OrderOperation const& removed_oper ) = 0;

				virtual void onMoveProcedure(OperationStructureMove const& os, OrderOperation const& oper, OrderOperation const& moved_oper ) = 0;

	};
	



}; // end of namespace

#endif
