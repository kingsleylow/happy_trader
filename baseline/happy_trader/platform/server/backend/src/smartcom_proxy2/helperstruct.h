#ifndef _SMARTCOM_PROXY2HELPERSTRUCT_INCLUDED
#define _SMARTCOM_PROXY2HELPERSTRUCT_INCLUDED

#include "smartcom_proxy2defs.h"

namespace SmartComHelperLib {

struct Candle
{
	Candle(double const& datetime, double const& open, double const& high, double const& low, double const& close, double const& volume, double const& open_int):
		datetime_m(datetime),
		open_m(open),
		high_m(high),
		low_m(low),
		close_m(close),
		volume_m(volume),
		open_int_m(open_int)
	{
	}

	Candle()
	{
		memset(this, '\0', sizeof(Candle));
	}

	CppUtils::String toString() const
	{
		CppUtils::StringConcat r;
				r << STRINGCONB << STRINGCONTPAIR_DATE(datetime_m) << 
				STRINGCONTPAIR(open_m) << 
				STRINGCONTPAIR(high_m) << 
				STRINGCONTPAIR(low_m) << 
				STRINGCONTPAIR(close_m) << 
				STRINGCONTPAIR(volume_m) <<	
				STRINGCONTPAIR(open_int_m)  
				STRINGCONE;

			return r.getString();			
	};

	double datetime_m;
	double open_m;
	double high_m;
	double low_m;
	double close_m;
	double volume_m;
	double open_int_m;
};

// ------------------------------------

struct InternalTrade
{

	InternalTrade():
		price_m(-1),
		amount_m(0),
		datetime_m(-1),
		isSent_m(false)
	{
	}

	InternalTrade(
			BSTR portfolio, 
			BSTR symbol, 
			BSTR orderNo, 
			double const& price, 
			double const& amount,  
			DATE datetime, 
			BSTR tradeNo ):
		price_m(price),
		amount_m(amount),
		datetime_m(datetime),
		symbol_m(symbol),
		portfolio_m(portfolio),
		tradeNo_m(tradeNo),
		orderNo_m(orderNo),
		isSent_m(false)
	{
	}

	 CComBSTR symbol_m;

	 CComBSTR portfolio_m;

	 CComBSTR orderNo_m;

	 double price_m;

	 double amount_m;

	 DATE datetime_m;

	 CComBSTR tradeNo_m;

	 // flag showing the order was sent
	bool isSent_m;

};

// -------------------------------

struct InternalOrder
{

	InternalOrder():
		state_m(-1),
		action_m(-1),
		type_m(-1),
		validity_m(-1),
		price_m(-1),
		amount_m(-1),
		stop_m(-1),
		filled_m(-1),
		datetime_m(-1),
		isSent_m(false)
	{

	};

	InternalOrder(
		BSTR symbol, 
		long const& state, 
		long const& action, 
		long const& type, 
		long const& validity, 
		double const& price, 
		double const& amount, 
		double const& stop, 
		double const& filled, 
		DATE datetime, 
		BSTR orderid, 
		BSTR orderno
	):
		symbol_m(symbol),
		state_m(state),
		action_m(action),
		type_m(type),
		validity_m(validity),
		price_m(price),
		amount_m(amount),
		stop_m(stop),
		filled_m(filled),
		datetime_m(datetime),
		orderid_m(orderid),
		orderno_m(orderno),
		isSent_m(false)
	{

	};


	CComBSTR symbol_m;

	long state_m;

	long action_m;

	long type_m;

	long validity_m;

	double price_m;

	double amount_m;

	double stop_m; 

	double filled_m; 

	DATE datetime_m;

	CComBSTR orderid_m;

	CComBSTR orderno_m;

	// flag showing the order was sent
	bool isSent_m;


};

typedef vector<InternalOrder> InternalOrderList;

typedef vector<InternalTrade> InternalTradesList;

// ----------------------


}; // end of namespace

#endif