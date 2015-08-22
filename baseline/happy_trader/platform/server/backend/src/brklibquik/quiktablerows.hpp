#ifndef _BRKLIBQUIK_QUIKTABLEROWS_INCLUDED
#define _BRKLIBQUIK_QUIKTABLEROWS_INCLUDED

#include "brklibquikdefs.hpp"
#include "deserial.hpp"


namespace BrkLib {

// these are the structures to be imported from Quik DDE broadcast
// field names must match the columns names
// -----------------------------------

// this is the QUIK table:
// Сделки
class QuikDeals : public QuikSerialImpl<QuikDeals>
{
public:
	CppUtils::String TRADEDATE;
	CppUtils::String TRADETIME;
	CppUtils::String SEC_SHORT_NAME;
	CppUtils::String BUYSELL;
	double PRICE;
	double QTY;
	double VALUE;
	CppUtils::String TRADENUM;
	CppUtils::String ORDERNUM;

	double trade_time_m;

	QUIK_BEGIN_SERIALIZE_MAP()
		QUIK_STRING_ENTRY(TRADEDATE)
		QUIK_STRING_ENTRY(TRADETIME)
		QUIK_STRING_ENTRY(SEC_SHORT_NAME)
		QUIK_STRING_ENTRY(BUYSELL)
		
		QUIK_DOUBLE_ENTRY(PRICE)
		QUIK_DOUBLE_ENTRY(QTY)
		QUIK_DOUBLE_ENTRY(PRICE)

		QUIK_STRING_ENTRY(TRADENUM)
		QUIK_STRING_ENTRY(ORDERNUM)

		QUIK_TRANSLATE_FROM_DATE_TIME_COLS_ENTRY2(trade_time_m, TRADEDATE, TRADETIME, "d.M.Y", "h:m:s" )

	QUIK_END_SERIALIZE_MAP()

	DECLARE_DDE_TOPIC( QUIKPOS, SHEET1  )
	DECLARE_QUIK_TABLE_NAME("Сделки");


};

// Исполненные и снятые заявки
class QuikExecutedAndCancelledOrders : public QuikSerialImpl<QuikExecutedAndCancelledOrders>
{
public:
	CppUtils::String ORDERDATE;
	CppUtils::String ORDERTIME;
	CppUtils::String SECSHORTNAME;
	CppUtils::String BUYSELL;
	double PRICE;
	double QTY;
	CppUtils::String STATUS;
	double BALANCE;
	CppUtils::String ORDERNUM;

	double order_time_m;

	// ------------------------

	QUIK_BEGIN_SERIALIZE_MAP()
		QUIK_STRING_ENTRY(ORDERDATE)
		QUIK_STRING_ENTRY(ORDERTIME)
		QUIK_STRING_ENTRY(SECSHORTNAME)
		QUIK_STRING_ENTRY(BUYSELL)
		QUIK_DOUBLE_ENTRY(PRICE)
		QUIK_DOUBLE_ENTRY(QTY)
		QUIK_STRING_ENTRY(STATUS)
		QUIK_DOUBLE_ENTRY(BALANCE)
		QUIK_STRING_ENTRY(ORDERNUM)

		QUIK_TRANSLATE_FROM_DATE_TIME_COLS_ENTRY2(order_time_m, ORDERDATE, ORDERTIME, "d.M.Y", "h:m:s" )
	QUIK_END_SERIALIZE_MAP()

	DECLARE_DDE_TOPIC( QUIKPOS, SHEET2  )
	DECLARE_QUIK_TABLE_NAME("Исполненные и снятые заявки");


};

// Активные заявки
class QuikActiveOrders : public QuikSerialImpl<QuikActiveOrders>
{
public:
	CppUtils::String ORDERDATE;
	CppUtils::String ORDERTIME;
	CppUtils::String SECSHORTNAME;
	CppUtils::String BUYSELL;
	double PRICE;
	double QTY;
	CppUtils::String STATUS;
	double BALANCE;
	CppUtils::String ORDERNUM;

	double order_time_m;

	// ------------------------

	QUIK_BEGIN_SERIALIZE_MAP()
		QUIK_STRING_ENTRY(ORDERDATE)
		QUIK_STRING_ENTRY(ORDERTIME)
		QUIK_STRING_ENTRY(SECSHORTNAME)
		QUIK_STRING_ENTRY(BUYSELL)
		QUIK_DOUBLE_ENTRY(PRICE)
		QUIK_DOUBLE_ENTRY(QTY)
		QUIK_STRING_ENTRY(STATUS)
		QUIK_DOUBLE_ENTRY(BALANCE)
		QUIK_STRING_ENTRY(ORDERNUM)

		QUIK_TRANSLATE_FROM_DATE_TIME_COLS_ENTRY2(order_time_m, ORDERDATE, ORDERTIME, "d.M.Y", "h:m:s" )
	QUIK_END_SERIALIZE_MAP()

	DECLARE_DDE_TOPIC( QUIKPOS, SHEET3  )
	DECLARE_QUIK_TABLE_NAME("Активные заявки");



};

// Таблица стоп заявок
class QuikStopOrders : public QuikSerialImpl<QuikStopOrders>
{
	public:
		CppUtils::String STOP_ORDERDATE;
		CppUtils::String STOP_ORDERNUM;
		CppUtils::String STOP_ORDERTIME;
		CppUtils::String STOP_ORDERKIND;	
		CppUtils::String STOP_ORDERKINDDESC;
		CppUtils::String SEC_SHORT_NAME;	
		CppUtils::String BUYSELL;
		CppUtils::String ACCOUNT;
		double CONDITION_PRICE;
		double PRICE;
		double QTY;
		double BALANCE;
		double FILLED_QTY;
		
		CppUtils::String BROKERREF;
		CppUtils::String LINKED_ORDER;
		CppUtils::String STATUS;
		CppUtils::String STATUS_DESC;

		double stop_order_time_m;

	// ------------------------

	QUIK_BEGIN_SERIALIZE_MAP()
		QUIK_STRING_ENTRY(STOP_ORDERDATE)
		QUIK_STRING_ENTRY(STOP_ORDERNUM)
		QUIK_STRING_ENTRY(STOP_ORDERTIME)
		QUIK_STRING_ENTRY(STOP_ORDERKIND)
		QUIK_STRING_ENTRY(STOP_ORDERKINDDESC)
		QUIK_STRING_ENTRY(SEC_SHORT_NAME)
		QUIK_STRING_ENTRY(BUYSELL)
		QUIK_STRING_ENTRY(ACCOUNT)
		QUIK_DOUBLE_ENTRY(CONDITION_PRICE)
		QUIK_DOUBLE_ENTRY(PRICE)
		QUIK_DOUBLE_ENTRY(QTY)
		QUIK_DOUBLE_ENTRY(BALANCE)
		QUIK_DOUBLE_ENTRY(FILLED_QTY)
		QUIK_STRING_ENTRY(BROKERREF)
		QUIK_STRING_ENTRY(LINKED_ORDER)
		QUIK_STRING_ENTRY(STATUS)
		QUIK_STRING_ENTRY(STATUS_DESC)

		QUIK_TRANSLATE_FROM_DATE_TIME_COLS_ENTRY2(stop_order_time_m, STOP_ORDERDATE, STOP_ORDERTIME, "d.M.Y", "h:m:s" )
	QUIK_END_SERIALIZE_MAP()

	DECLARE_DDE_TOPIC( QUIKPOS, SHEET4  )
	DECLARE_QUIK_TABLE_NAME("Таблица стоп-заявок");



};

// СНЯТЫЕ и ИСПОЛНЕННЫЕ стоп заявки
class QuikCancelledAndExecutedStopOrders : public QuikSerialImpl<QuikCancelledAndExecutedStopOrders>
{
	public:
		CppUtils::String STOP_ORDERDATE;
		CppUtils::String STOP_ORDERTIME;
		CppUtils::String SEC_SHORT_NAME;
		CppUtils::String BUYSELL;
		double CONDITION_PRICE;
		double PRICE;
		double QTY;
		CppUtils::String STOP_ORDERKIND;
		CppUtils::String STATUS;

		double stop_order_time_m;

	// ------------------------

	QUIK_BEGIN_SERIALIZE_MAP()
		QUIK_STRING_ENTRY(STOP_ORDERDATE)
		QUIK_STRING_ENTRY(STOP_ORDERTIME)
		QUIK_STRING_ENTRY(SEC_SHORT_NAME)
		QUIK_STRING_ENTRY(BUYSELL)
		
		QUIK_DOUBLE_ENTRY(CONDITION_PRICE)
		QUIK_DOUBLE_ENTRY(PRICE)
		QUIK_DOUBLE_ENTRY(QTY)
		
		QUIK_STRING_ENTRY(STOP_ORDERKIND)
		QUIK_STRING_ENTRY(STATUS)

		QUIK_TRANSLATE_FROM_DATE_TIME_COLS_ENTRY2(stop_order_time_m, STOP_ORDERDATE, STOP_ORDERTIME, "d.M.Y", "h:m:s" )
	QUIK_END_SERIALIZE_MAP()

	DECLARE_DDE_TOPIC( QUIKPOS, SHEET5  )
	DECLARE_QUIK_TABLE_NAME("СНЯТЫЕ и ИСПОЛНЕНЫЕ стоп заявки");

};


// ЛУКОЙЛ [А1-Акции] Котировки
/*
class QuikLukoilLevel2 : public QuikSerialImpl<QuikLukoilLevel2>
{
	double BUY_VOLUME;
	double PRICE;
	double SELL_VOLUME;
	double OWN_SELL_VOLUME;
	double OWN_BUY_VOLUME;

	QUIK_BEGIN_SERIALIZE_MAP()
		
		QUIK_DOUBLE_ENTRY(BUY_VOLUME)
		QUIK_DOUBLE_ENTRY(PRICE)
		QUIK_DOUBLE_ENTRY(SELL_VOLUME)
		QUIK_DOUBLE_ENTRY(OWN_SELL_VOLUME)
		QUIK_DOUBLE_ENTRY(OWN_BUY_VOLUME)

	QUIK_END_SERIALIZE_MAP()




	DECLARE_DDE_TOPIC( QUIKSECURITIES, LUKOIL  )
	DECLARE_QUIK_TABLE_NAME("ЛУКОЙЛ [А1-Акции] Котировки");
};
*/


#include "quiktablerows.inc"


}; // end of namespace

#endif