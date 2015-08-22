#include "helperfun.hpp"

namespace BrkLib {

	
CppUtils::String  posTypeToString(BrkLib::PosType const& posType)
{
	switch (posType) {
		case	BrkLib::POS_BUY:	return "POS_BUY";
		case	BrkLib::POS_LIMIT_BUY: return "POS_LIMIT_BUY";
		case	BrkLib::POS_STOP_BUY:	return "POS_STOP_BUY";
		case	BrkLib::POS_STOP_LIMIT_BUY: return "POS_STOP_LIMIT_BUY";
		case	BrkLib::POS_CLOSE_LONG: return "POS_CLOSE_LONG";
		case	BrkLib::POS_CLOSE_LIMIT_LONG: return "POS_CLOSE_LIMIT_LONG";
		case	BrkLib::POS_CLOSE_STOP_LONG: return "POS_CLOSE_STOP_LONG";
		case	BrkLib::POS_CLOSE_STOP_LIMIT_LONG: return "POS_CLOSE_STOP_LIMIT_LONG";

		case	BrkLib::POS_SELL:	return "POS_SELL";
		case	BrkLib::POS_LIMIT_SELL: return "POS_LIMIT_SELL";
		case	BrkLib::POS_STOP_SELL:	return "POS_STOP_SELL";
		case	BrkLib::POS_STOP_LIMIT_SELL: return "POS_STOP_LIMIT_SELL";
		case	BrkLib::POS_CLOSE_SHORT: return "POS_CLOSE_SHORT";
		case	BrkLib::POS_CLOSE_LIMIT_SHORT: return "POS_CLOSE_LIMIT_SHORT";
		case	BrkLib::POS_CLOSE_STOP_SHORT: return "POS_CLOSE_STOP_SHORT";
		case	BrkLib::POS_CLOSE_STOP_LIMIT_SHORT: return "POS_CLOSE_STOP_LIMIT_SHORT";

		default:
			return "N/A";

	}
}

TradeDirection posTypeToDirection(BrkLib::PosType const& posType)
{
	switch (posType) {
		case	BrkLib::POS_BUY:	return TD_LONG;
		case	BrkLib::POS_LIMIT_BUY: return TD_LONG;
		case	BrkLib::POS_STOP_BUY:	return TD_LONG;
		case	BrkLib::POS_STOP_LIMIT_BUY: return TD_LONG;
		case	BrkLib::POS_CLOSE_LONG: return TD_LONG;
		case	BrkLib::POS_CLOSE_LIMIT_LONG: return TD_LONG;
		case	BrkLib::POS_CLOSE_STOP_LONG: return TD_LONG;
		case	BrkLib::POS_CLOSE_STOP_LIMIT_LONG: return TD_LONG;

		case	BrkLib::POS_SELL:	return TD_SHORT;
		case	BrkLib::POS_LIMIT_SELL: return TD_SHORT;
		case	BrkLib::POS_STOP_SELL:	return TD_SHORT;
		case	BrkLib::POS_STOP_LIMIT_SELL: return TD_SHORT;
		case	BrkLib::POS_CLOSE_SHORT: return TD_SHORT;
		case	BrkLib::POS_CLOSE_LIMIT_SHORT: return TD_SHORT;
		case	BrkLib::POS_CLOSE_STOP_SHORT: return TD_SHORT;
		case	BrkLib::POS_CLOSE_STOP_LIMIT_SHORT: return TD_SHORT;

		default:
			return TD_NONE;

	}
}

TradeDirection orderTypeToDirection(BrkLib::OrderType const& orderType)
{
	switch (orderType) {
		case	BrkLib::OP_BUY:	return TD_LONG;
		case	BrkLib::OP_BUY_LIMIT: return TD_LONG;
		case	BrkLib::OP_BUY_STOP:	return TD_LONG;
		case	BrkLib::OP_BUY_STOP_LIMIT: return TD_LONG;
		case	BrkLib::OP_CLOSE_LONG: return TD_LONG;
		case	BrkLib::OP_CLOSE_LONG_LIMIT: return TD_LONG;
		case	BrkLib::OP_CLOSE_LONG_STOP: return TD_LONG;
		case	BrkLib::OP_CLOSE_LONG_STOP_LIMIT: return TD_LONG;

		case	BrkLib::OP_SELL:	return TD_SHORT;
		case	BrkLib::OP_SELL_LIMIT: return TD_SHORT;
		case	BrkLib::OP_SELL_STOP:	return TD_SHORT;
		case	BrkLib::OP_SELL_STOP_LIMIT: return TD_SHORT;
		case	BrkLib::OP_CLOSE_SHORT: return TD_SHORT;
		case	BrkLib::OP_CLOSE_SHORT_LIMIT: return TD_SHORT;
		case	BrkLib::OP_CLOSE_SHORT_STOP: return TD_SHORT;
		case	BrkLib::OP_CLOSE_SHORT_STOP_LIMIT: return TD_SHORT;

		default:
			return TD_NONE;

	}
}

CppUtils::String tradeDirection2String(TradeDirection const td)
{
	switch (td) {
		case	TD_NONE:	return "TD_NONE";
		case	TD_LONG:	return "TD_LONG";
		case	TD_SHORT:	return "TD_SHORT";
		case	TD_BOTH:	return "TD_BOTH";
		default:
			return "N/A";
	}
}

CppUtils::String tradeDirection2String(int const td)
{
	return tradeDirection2String(TradeDirection(td));
}

CppUtils::String orderTypeBitwise2String(int const ot)
{
	CppUtils::String r;
	if (ot & OP_DUMMY) r.append("OP_DUMMY").append(" | ");
	if (ot & OP_BUY) r.append("OP_BUY").append(" | ");
	if (ot & OP_SELL) r.append("OP_SELL").append(" | ");
	if (ot & OP_BUY_LIMIT) r.append("OP_BUY_LIMIT").append(" | ");
	if (ot & OP_SELL_LIMIT) r.append("OP_SELL_LIMIT").append(" | ");
	if (ot & OP_BUY_STOP) r.append("OP_BUY_STOP").append(" | ");
	if (ot & OP_SELL_STOP) r.append("OP_SELL_STOP").append(" | ");
	if (ot & OP_TAKEPROFIT_SET) r.append("OP_TAKEPROFIT_SET").append(" | ");
	if (ot & OP_STOPLOSS_SET) r.append("OP_STOPLOSS_SET").append(" | ");
	if (ot & OP_ORDER_CANCEL) r.append("OP_ORDER_CANCEL").append(" | ");
	if (ot & OP_HANDSHAKE) r.append("OP_HANDSHAKE").append(" | ");
	if (ot & OP_RELATIVE_OPEN_PRICE) r.append("OP_RELATIVE_OPEN_PRICE").append(" | ");
	if (ot & OP_GET_SYMBOL_METAINFO) r.append("OP_GET_SYMBOL_METAINFO").append(" | ");
	if (ot & OP_EXEC_PRICE_SET) r.append("OP_EXEC_PRICE_SET").append(" | ");
	if (ot & OP_EXPIR_SET) r.append("OP_EXPIR_SET").append(" | ");
	if (ot & OP_BUY_STOP_LIMIT) r.append("OP_BUY_STOP_LIMIT").append(" | ");
	if (ot & OP_SELL_STOP_LIMIT) r.append("OP_SELL_STOP_LIMIT").append(" | ");
	if (ot & OP_CLOSE_SHORT) r.append("OP_CLOSE_SHORT").append(" | ");
	if (ot & OP_CLOSE_LONG) r.append("OP_CLOSE_LONG").append(" | ");
	if (ot & OP_CLOSE_SHORT_LIMIT) r.append("OP_CLOSE_SHORT_LIMIT").append(" | ");
	if (ot & OP_CLOSE_LONG_LIMIT) r.append("OP_CLOSE_LONG_LIMIT").append(" | ");
	if (ot & OP_CLOSE_SHORT_STOP) r.append("OP_CLOSE_SHORT_STOP").append(" | ");
	if (ot & OP_CLOSE_LONG_STOP) r.append("OP_CLOSE_LONG_STOP").append(" | ");
	if (ot & OP_CLOSE_SHORT_STOP_LIMIT) r.append("OP_CLOSE_SHORT_STOP_LIMIT").append(" | ");
	if (ot & OP_CLOSE_LONG_STOP_LIMIT) r.append("OP_CLOSE_LONG_STOP_LIMIT").append(" | ");

	return r;
}

OrderType orderTypeClearance(int const ot)
{
	if (ot & OP_DUMMY) return OP_DUMMY;
	if (ot & OP_BUY) return OP_BUY;
	if (ot & OP_SELL) return OP_SELL;
	if (ot & OP_BUY_LIMIT) return OP_BUY_LIMIT;
	if (ot & OP_SELL_LIMIT) return OP_SELL_LIMIT;
	if (ot & OP_BUY_STOP) return OP_BUY_STOP;
	if (ot & OP_SELL_STOP) return OP_SELL_STOP;
	if (ot & OP_TAKEPROFIT_SET) return OP_TAKEPROFIT_SET;
	if (ot & OP_STOPLOSS_SET) return OP_STOPLOSS_SET;
	if (ot & OP_ORDER_CANCEL) return OP_ORDER_CANCEL;
	if (ot & OP_HANDSHAKE) return	OP_HANDSHAKE;
	if (ot & OP_RELATIVE_OPEN_PRICE) return	OP_RELATIVE_OPEN_PRICE;
	if (ot & OP_GET_SYMBOL_METAINFO) return	OP_GET_SYMBOL_METAINFO;
	if (ot & OP_EXEC_PRICE_SET) return OP_EXEC_PRICE_SET;
	if (ot & OP_EXPIR_SET) return	OP_EXPIR_SET;
	if (ot & OP_BUY_STOP_LIMIT) return OP_BUY_STOP_LIMIT;
	if (ot & OP_SELL_STOP_LIMIT) return OP_SELL_STOP_LIMIT;
	if (ot & OP_CLOSE_SHORT) return OP_CLOSE_SHORT;
	if (ot & OP_CLOSE_LONG) return OP_CLOSE_LONG;
	if (ot & OP_CLOSE_SHORT_LIMIT) return	OP_CLOSE_SHORT_LIMIT;
	if (ot & OP_CLOSE_LONG_LIMIT) return OP_CLOSE_LONG_LIMIT;
	if (ot & OP_CLOSE_SHORT_STOP) return OP_CLOSE_SHORT_STOP;
	if (ot & OP_CLOSE_LONG_STOP) return	OP_CLOSE_LONG_STOP;
	if (ot & OP_CLOSE_SHORT_STOP_LIMIT) return OP_CLOSE_SHORT_STOP_LIMIT;
	if (ot & OP_CLOSE_LONG_STOP_LIMIT) return	OP_CLOSE_LONG_STOP_LIMIT;

	return OP_DUMMY;
}

CppUtils::String orderValidity2String(OrderValidity const ov)
{
	switch(ov) {
		case OV_DUMMY: return "OV_DUMMY";
		case OV_GTC: return "OV_GTC";
		case OV_DAY: return "OV_DAY";
		default: return "N/A";

	}

}



CppUtils::String getOrderSmallDescription(BrkLib::Order const& order)
{
	CppUtils::StringConcat r;

	r << "\n[" << STRINGCONTPAIR(order.ID_m) << STRINGCONTPAIR(order.symbol_m) << STRINGCONTPAIR(order.provider_m) << STRINGCONTPAIR(order.orPrice_m) << 
		STRINGCONTPAIR(order.orStopPrice_m) << 
		STRINGCONTPAIR(order.orVolume_m) <<	STRINGCONTPAIR_DATE(order.orTime_m) << STRINGCONTPAIR_FUN(orderValidity2String, order.orderValidity_m) << 
		STRINGCONTPAIR_FUN(orderTypeBitwise2String, order.orderType_m) <<
		"order.context_m=" << order.context_m.getDescriptionString() << ";\n"	<< 
		"]\n";

	

	return r.getString();
}

}; // end of namespace
