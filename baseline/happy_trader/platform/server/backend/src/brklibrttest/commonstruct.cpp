#include "commonstruct.hpp"
namespace BrkLib {

	// --------------------------------------
		// helper functions

	CppUtils::String orderOperationType2String(OrderOperationType const oper_type)
	{
		switch(oper_type) {
			case OOT_Market_Buy: return "OOT_Market_Buy";
			case OOT_Market_Sell: return "OOT_Market_Sell";
			case OOT_Limit_Buy: return "OOT_Limit_Buy";
			case OOT_Limit_Sell: return "OOT_Limit_Sell";
			case OOT_Stop_Buy: return "OOT_Stop_Buy";
			case OOT_Stop_Sell: return "OOT_Stop_Sell";
			case OOT_Stop_Limit_Buy: return "OOT_Stop_Limit_Buy";
			case OOT_Stop_Limit_Sell: return "OOT_Stop_Limit_Sell";
			case OOT_Cancel: return "OOT_Cancel";
			case OOT_Move: return "OOT_Move";
			case OOT_Dummy: return "OOT_Dummy";
			default: return "N/A";
		};
	}

	CppUtils::String positionTradeAction2String(PositionTradeAction const pt)
	{
		switch(pt) {
			case PTA_Dummy: return "PTA_Dummy";
			case PTA_Create: return "PTA_Create";
			case PTA_Increase: return "PTA_Increase";
			case PTA_Decrease: return "PTA_Decrease"; 
			default: return "N/A";
		}
	}


	CppUtils::String getCommonOperationDescription(OrderOperation const& oper,Price const& p)
	{
		CppUtils::StringConcat r;

		r << "[\noper:\n[" << oper.toString() << "\n]\nprice:\n[" << p.toString() << "]\n]\n";

		return r.getString();
	}


	// -------------------------------------

	Price::Price(double const& bid, double const ask, double const ttime):
			bid_m(bid),
			ask_m(ask),
			time_m(ttime)
		{
		};

		Price::Price():
			bid_m(-1),
			ask_m(-1),
			time_m(-1)
		{
		}

		Price& Price::operator = (Price const& src)
		{
			bid_m = src.bid_m;
			ask_m = src.ask_m;
			time_m = src.time_m;

			return *this;
		}

		void Price::create(double const& bid, double const ask, double const ttime)
		{
			bid_m = bid;
			ask_m = ask;
			time_m = ttime;
		};

		void Price::clear()
		{
			bid_m = -1;
			ask_m = -1;
			time_m = -1;
		}

		bool Price::isValid() const
		{
			return bid_m > 0 && ask_m > 0 && time_m > 0;
		}

		CppUtils::String Price::toString() const
		{
			CppUtils::StringConcat r;
			r << "[\n"
				STRINGCONTPAIR(bid_m) << 
				STRINGCONTPAIR(ask_m) << 
				STRINGCONTPAIR_DATE(time_m) << "]\n";

			return r.getString();
		}

		// ---------------------------

		PositionForSymbol::PositionForSymbol():
			direction_m(TD_NONE),
			imbalanceVolume_m(-1),
			history_m(false),
			brkSession_m(NULL)
		{
			positionUid_m.generate();
		};

		double PositionForSymbol::getAvrOpenPrice() const
		{
			if (opens_m.size() == 0)
				return -1;

			double res = 0;
			for(int i = 0; i < opens_m.size(); i++) 
				res += opens_m[i].tprice_m;

			res /= opens_m.size();
			return res;
		}

		double PositionForSymbol::getAvrClosePrice() const
		{
			if (closes_m.size() == 0)
				return -1;

			double res = 0;
			for(int i = 0; i < closes_m.size(); i++) 
				res += closes_m[i].tprice_m;

			res /= closes_m.size();
			return res;
		}

		double PositionForSymbol::getFullOpenVolume() const
		{
			double res = 0;
			for(int i = 0; i < opens_m.size(); i++) 
				res += opens_m[i].volume_m;

			return res;
		}

		double PositionForSymbol::getFullCloseVolume() const
		{
			double res = 0;
			for(int i = 0; i < closes_m.size(); i++) 
				res += closes_m[i].volume_m;

			return res;	
		}

		double PositionForSymbol::getOpenTime() const
		{
			if (opens_m.size() > 0)
			 return opens_m[opens_m.size()-1].ttime_m;

			return -1;
		}

		double PositionForSymbol::getCloseTime() const
		{
			if (closes_m.size() > 0)
			 return closes_m[closes_m.size()-1].ttime_m;

			return -1;
		}

		CppUtils::String PositionForSymbol::toString() const
		{
			CppUtils::StringConcat r;

			r << STRINGCONB << 
				STRINGCONTPAIR(positionUid_m) <<
				STRINGCONTPAIR_FUN(tradeDirection2String, direction_m) <<
				STRINGCONTPAIR(imbalanceVolume_m) <<
				STRINGCONTPAIR(history_m) << 
				"open size: " << opens_m.size() << ";\n" <<
				"close size: " << closes_m.size() << ";\n" <<

				STRINGCONE;

			return r.getString();
		}

		void PositionForSymbol::applyDecreaseOperation(
			BrokerSession* brksession, 
			PositionEntry const & entry, 
			double const& imbalanceVolume, 
			bool const history
		)
		{
			brkSession_m = brksession;
			closes_m.push_back( entry );
			imbalanceVolume_m = imbalanceVolume;
			history_m = history;
		}


		// -----------------------------

		
		
		

		// -----------------------------

		OrderOperation::OrderOperation():
			brkSession_m(NULL),
			volume_m(-1),
			price_m(-1),
			stopPrice_m(-1)
		{
		};

		OrderOperation::OrderOperation(
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
	):
			type_m(type),
			provider_m(provider),
			symbol_m(symbol),
			brkSession_m(brkSession),
			volume_m(volume),
			price_m(price),
			stopPrice_m(stopprice),
			tinfo_m(tinfo),
			cancelOrderId_m(cancelOrderId),
			moveOrderId_m(moveOrderId),
			orderUid_m(orderUid),
			affectedPositionId_m(affectedPositionId),
			order_m(order)

		{
		}

		

		bool OrderOperation::isStopOperration() const
		{
			return (
				type_m == OOT_Stop_Buy || 
				type_m == OOT_Stop_Sell || 
				type_m == OOT_Stop_Limit_Buy ||  
				type_m == OOT_Stop_Limit_Sell
			);
			
		}


		CppUtils::String OrderOperation::toString() const
		{
			

				CppUtils::StringConcat r;
				r << STRINGCONB << STRINGCONTPAIR_FUN(orderOperationType2String,type_m) << 
				STRINGCONTPAIR(provider_m) << 
				STRINGCONTPAIR(symbol_m) << 
				STRINGCONTPAIR(volume_m) << 
				STRINGCONTPAIR(price_m) << 
				STRINGCONTPAIR(stopPrice_m) <<	
				STRINGCONTPAIR(cancelOrderId_m) <<
				STRINGCONTPAIR(moveOrderId_m) <<
				STRINGCONTPAIR(orderUid_m) <<
				STRINGCONTPAIR(affectedPositionId_m) <<
				STRINGCONTPAIR_FUN(getOrderSmallDescription, order_m)	<< 
				STRINGCONE;

				return r.getString();			
		}

		

};