#ifndef _BRKLIB_BROKERSTRUCT_INCLUDED
#define _BRKLIB_BROKERSTRUCT_INCLUDED

#include "brklibdefs.hpp"
#include "brokersession.hpp"


/**
 helper structures to broker
*/
namespace BrkLib {

	enum BrokerMode
	{
		BM_NOTHING =  0x0000,
		BM_USE_CLOSE_POS_COMMANDS = 0x0001, // use only open commands notation
		BM_ONE_POSITION_PER_SYMBOL = 0x0002 // only one position per symbol is allowed, so brokerPositionId is inored in most cases as mapped by symbol
	};

	// this is in addition to expiration time - it shows when order is expired
	enum OrderValidity
	{
		OV_DUMMY =	0,
		OV_GTC =		1,
		OV_DAY =		2
	};

	enum DelayedOrderType
	{
		DO_DUMMY =						0,
		DO_EXEC_PRICE_MORE =	1,
		DO_EXEC_PRICE_LESS =	2
	};

	enum DelayedOrderResponseType
	{
		DORT_DUMMY =						0,
		DORT_REGISTER_ORDER		=	1,
		DORT_UNREGISTER_ORDER =	2,
		DORT_ISSUE_ORDER			=	3,
	};

	// this is price constants
	// which are passed in price field
	// all negative
	enum PriceConst
	{
		PC_ZERO =			0, // price is zero
		PC_BEST_BID = 1, // best bid
		PC_BEST_ASK = 2,  // best ask
		PC_INPUT =		3  // use price as it is in the input field
	};

	// this is alert priorities
	enum AlertPriorities {
		AP_LOW =			3,
		AP_MEDIUM =		2,
		AP_HIGH =			1,
		AP_CRITICAL =	0
	};

	// these are types of operation on Orders against Broker server
	// can be joined via or if there is a sence
	enum OrderType { 
		OP_DUMMY							=	0x0800,
		OP_BUY								=	0x0001,
    OP_SELL								= 0x0002,
    OP_BUY_LIMIT					=	0x0004,
    OP_SELL_LIMIT					=	0x0008,
    OP_BUY_STOP						=	0x0010,
    OP_SELL_STOP					=	0x0020,
		OP_TAKEPROFIT_SET			=	0x0040,
		OP_STOPLOSS_SET				=	0x0080,
		//OP_CLOSE							=	0x0100,// close executed order
		OP_ORDER_CANCEL				=	0x0200,// cancel still not executed order
		OP_HANDSHAKE					=	0x0400, // this is just handshake to get any specific response from broker
		// flag shoing that the price is relative to the open price
		// usefull when you wnat to set up the price relatevely to the open price
		// applicable when you modify order and change SL or TP prices
		// thus used only with OP_TAKEPROFIT_SET or OP_STOPLOSS_SET
		OP_RELATIVE_OPEN_PRICE	=	0x1000,
		// retreives symbol metainfo
		OP_GET_SYMBOL_METAINFO = 0x2000,
		OP_EXEC_PRICE_SET			 = 0x4000, //set exec price for pending orders
		OP_EXPIR_SET					 = 0x8000, // set expiration,

		OP_BUY_STOP_LIMIT				=	0x10000, // buy stop limit
    OP_SELL_STOP_LIMIT			=	0x20000,  // sell stop limit

		OP_CLOSE_SHORT					= 0x40000, // this closes short as market
		OP_CLOSE_LONG						= 0x80000,  // this closes long as market

		OP_CLOSE_SHORT_LIMIT					= 0x100000, // this closes short as limit
		OP_CLOSE_LONG_LIMIT						= 0x200000,  // this closes long as limit

		OP_CLOSE_SHORT_STOP						= 0x400000, // this closes short as stop
		OP_CLOSE_LONG_STOP						= 0x800000,  // this closes long as stop

		OP_CLOSE_SHORT_STOP_LIMIT			= 0x1000000, // this closes short as stop
		OP_CLOSE_LONG_STOP_LIMIT			= 0x2000000, // this closes long as stop,

		OP_ORDER_MOVE									= 0x4000000
   };

	

	// what broker can return to client
	// it can happen asynchroniously without client request
	// can be joined via or
	enum OperationResult {
		PT_DUMMY																					= 0,
		
		// whethere order was installed or executed
		PT_ORDER_ESTABLISHED															=	1,
		PT_CANNOT_ESTABLISH_ORDER													= 2,

		// whether order was modifies
		PT_ORDER_MODIFIED																	=	3,
		PT_CANNOT_MODIFY_ORDER														=	4,
	

		// whether position was closed
		//PT_ORDER_CLOSED																		=	5,
		//PT_CANNOT_CLOSE_ORDER															=	6,
		
		// whether position was cancelled
		PT_ORDER_CANCELLED																=	7,
		PT_CANNOT_CANCEL_ORDER														=	8,

		// this happens on overnight and other situations when position is extra-charged
		// or commission is charged - whatever when some money are added or substructed to/from your account
		PT_POSITION_CHARGED																=	9,

		// any kind of information message from the broker not changing current position
		PT_GENERAL_INFO																		=	10, // general info
		PT_CURRENT_QUOTES																	=	11, // current bid/ask price
		
		PT_DISCONNECTED																		=	12, // broker layer disconnected the client
		PT_GENERAL_ERROR_HAPPENED													=	13, // some general error happened
		PT_NEWS_ARRIVED																		=	14,	// news arrived
		PT_HANDSHAKEARRIVED																= 15, // handshake arrived

		PT_REGISTERED_DELAYED_ORDER												= 16,
		PT_UNREGISTERED_DELAYED_ORDER											= 17,
		PT_DELAYED_ORDER_GOING_TO_EXECUTE									= 18,

		
		PT_POSITION_CLOSED																= 19,
		PT_CANNOT_CLOSE_POSITION													= 20,

		PT_POSITION_MODIFIED															= 21,
		PT_CANNOT_MODIFY_POSITION													= 22
		
	};

	// response types
	// to which calss we will cast
	enum ResponseTypes {
		RT_BrokerResponseDummy		=		0,
		RT_BrokerResponseOrder		=		1,
		RT_BrokerResponseEquity		=		2,
		RT_BrokerResponseTick			=		3,
		RT_BrokerResponseInfo			=		4,
		RT_BrokerResponseHistory	=		5,
		RT_BrokerErrorInfo				=		6,
		RT_BrokerResponseMetaInfo	=		7,
		RT_BrokerResponseDelayedOrder = 8,
		RT_BrokerResponseClosePos = 9,
		RT_BrokerResponsePositionOperation = 10
	};

	/**
	 This is helper structure containing detailed reasons what happened
	 with operation. can be Or
	*/
	enum OperationDetailResult {
			ODR_OK												= 0x00100000,
			ODR_INVALID_PRICE							=	0x00000001,
			ODR_INVALID_VOLUME						=	0x00000002,
			ODR_INVALID_STOPLOSS_PRICE		=	0x00000004,
			ODR_INVALID_TAKEPROFIT_PRICE	=	0x00000008,
			ODR_INVALID_EXPIRATION				=	0x00000010,
			ODR_STOPLOSS_CHANGED_OK				=	0x00000020,
			ODR_TAKEPROFIT_CHANGED_OK			=	0x00000040,
			ODR_EXPIRATION_CHANGED_OK			=	0x00000080,
			ODR_MARGINE_CALL							=	0x00000100,
			ODR_ORDER_INSTALLED						=	0x00000200,
			ODR_ORDER_EXECUTED						=	0x00000400,
			ODR_GENERAL_ERROR							= 0x00000800,
			ODR_DUMMY											= 0x00000000, 
			ODR_ERROR_REQUOTE							= 0x00002000,
			ODR_INVALID_ACCOUNT						= 0x00004000, 
			ODR_SHORT_ONLY								= 0x00008000, 
			ODR_LONG_ONLY									= 0x00010000, 
			ODR_CANNOT_MODIFY_ORDER				= 0x00020000, 
			ODR_SERVER_BUSY								= 0x00040000,
			ODR_MARKET_CLOSED							= 0x00080000,
			ORD_NOT_SUPPORTED							=	0x00200000,
			ORD_INVALID_ORDER							=	0x00400000,
			ORD_EXECUTE_PRICE_CHANGED_OK	=	0x00800000,
			
			ODR_INVALID_STOP_PRICE				=	0x01000000,  // stop price was not accepted
			ODR_STOP_PRICE_CHANGED_OK			= 0x02000000,  // when installing stop orders this shows that stop was accepted
			ODR_ORDER_PENDING							= 0x04000000,
			ODR_ORDER_SUBMITTED						= 0x08000000
			
			
	};
	
	/**
		Position (order) type
		used in responses to ientify what we position we opened, modified, cancelled or closed
	*/
	enum PosType {
		POS_DUMMY = 0,

		// open 
		POS_BUY =											1, 
		POS_SELL =										2, 
		POS_LIMIT_BUY =								3, 
		POS_LIMIT_SELL =							4, 
		POS_STOP_BUY =								5, 
		POS_STOP_SELL =								6, 
		POS_STOP_LIMIT_BUY =					7, 
		POS_STOP_LIMIT_SELL =					8, 

		// close
		POS_CLOSE_LONG =							9, 
		POS_CLOSE_SHORT =							10, 
		POS_CLOSE_LIMIT_LONG =				11, 
		POS_CLOSE_LIMIT_SHORT =				12, 
		POS_CLOSE_STOP_LONG =					13, 
		POS_CLOSE_STOP_SHORT =				14, 
		POS_CLOSE_STOP_LIMIT_LONG =		15, 
		POS_CLOSE_STOP_LIMIT_SHORT =	16,
		POS_CANCELLED								= 17
		
	};

	enum TradeDirection
	{
		TD_NONE = 0,
		TD_LONG = 1,
		TD_SHORT =2,
		TD_BOTH = 3
	};

	
	

	/**
	 Position state. 
	*/
	enum PosState {
		STATE_DUMMY =		0,
		STATE_OPEN =		2,
		STATE_HISTORY = 3
	};

	// getBrokerPositionList - can be orred
	enum PosStateQuery {
		QUERY_OPEN =		0x0002,
		QUERY_HISTORY = 0x0004
	};

	enum CloseReason {
		REASON_DUMMY	=				0,
		REASON_NORMAL =				1, // normal close of position because of command
		REASON_SL			=				2,
		REASON_TP			=				3,
		REASON_CANCEL	=				4,
		REASON_MARGINE_CALL	= 5,
		REASON_ERROR				= 6,
		REASON_EXPIRATION		= 7
	};

	
	/**
	* Context
	*/

/** 
	 Order structure - this is request to broker
	*/

	// must be overriden
	class BRKLIB_EXP BrokerContext
	{
	public:
		
		BrokerContext()
		{
		}

		virtual ~BrokerContext()
		{
		}

		virtual BrokerContext* clone() const = 0; 

		virtual void deallocate() = 0;

		virtual CppUtils::String toString()	const
		{
			return "N/A";
		}

			
	};


	
	
	class BRKLIB_EXP BrokerResponseContext
	{
	public:
		BrokerResponseContext():
			data_m(NULL)
		 {
		 }

		~BrokerResponseContext()
		{
			clearContext();
		}

		 BrokerResponseContext& operator = (BrokerResponseContext const &src)
		 {
				clearContext();

				if (src.data_m) {
					data_m = src.data_m->clone();
				}

				return *this;
		 }

		 BrokerResponseContext(BrokerResponseContext const& src)
		 {
				if (src.data_m) {
					data_m = src.data_m->clone();
				}	else {
					data_m= NULL;
				}
		 }

		 BrokerContext* getContext()
		 {
			 return data_m;
		 }

		 BrokerContext const* getContext() const
		 {
			 return data_m;
		 }

		 void setContext(BrokerContext const& brkCtx)
		 {
				clearContext();
				data_m = brkCtx.clone();
			 
		 }

		 void clearContext()
		 {
			 if (data_m) {
				 data_m->deallocate();
				 data_m = NULL;

			 }
		 }

		 CppUtils::String getDescriptionString() const
		 {
			 if (data_m == NULL) {
				 return "[ NULL ]";
			 }
			 else {
				 return data_m->toString();
			 }
		 }

		 

	private:											   

		 BrokerContext* data_m;
	};

	
	// this is only for serialization/deseralization
	class BRKLIB_EXP BrokerContext_String: public BrokerContext
	{
	public: 

		BrokerContext_String();
		
		virtual ~BrokerContext_String();
		
		virtual BrokerContext* clone() const;
		
		virtual void deallocate();

		virtual CppUtils::String toString()	const
		{
			return strData_m;
		}

		CppUtils::String& getStringData()
		{
			return strData_m;
		}

		CppUtils::String const& getStringData() const
		{
			return strData_m;
		}

	private:

		CppUtils::String strData_m;

	};


	class Order: public HlpStruct::SerialImpl<Order> {
		public:

		Order():
			orderType_m(OP_DUMMY),
			orTime_m(-1),
			orPrice_m(-1),
			orStopPrice_m(-1),
			orSLprice_m(-1),
			orTPprice_m(-1),
			orVolume_m(-1),
			orExpirationTime_m(-1),
			orderValidity_m(OV_DUMMY),
			orderIssueTime_m(-1),
			//cookie_symb_prov_m(-1),
			orPriceType_m(PC_ZERO),
			orStopPriceType_m(PC_ZERO)
		
		{
			// set default time value
			orderIssueTime_m = CppUtils::getTime();
			
			ID_m.generate();
		}
	
	public:

		

		// native ID
		CppUtils::Uid ID_m;

		// broker order ID - the reference when cancelling or closing this order
		CppUtils::String brokerOrderID_m;
		
		// when we want to change position we reference this field
		CppUtils::String brokerPositionID_m;

		// type of order
		OrderType orderType_m;

		// provider that we are going to use
		CppUtils::String provider_m;

		CppUtils::String symbol_m;
				
		// comment
		CppUtils::String comment_m;
		
		// order issue time
		mutable double orderIssueTime_m;

		// order time
		double orTime_m;

		// price for market orders
		double orPrice_m;

		// price for STOP orders - STOP price
		double orStopPrice_m;

		// initial stop loss price
		double orSLprice_m;

		// initial take profit price
		double orTPprice_m;

		// order volume
		// here volume has the sence of the direct amount
		// it must be re-calculated to lots if ANY
		int orVolume_m;

		// expiration time for non-executed orders
		double  orExpirationTime_m;

		// oredr validity const
		OrderValidity orderValidity_m;

		// price type (how to interpret orPrice_m)
		PriceConst orPriceType_m;

		PriceConst orStopPriceType_m;

		// --------------------
		// below is specially for stocks
		CppUtils::String place_m;

		CppUtils::String method_m;

		CppUtils::String additionalInfo_m;


		
		// serailization map
		BEGIN_SERIALIZE_MAP()
			UID_ENTRY( ID_m )
			STRING_ENTRY(brokerOrderID_m)
			STRING_ENTRY(brokerPositionID_m)
			INT_ENTRY2(orderType_m, OrderType)
			STRING_ENTRY(provider_m)
			//STRING_ENTRY(symbolNum_m)
			//STRING_ENTRY(symbolDenom_m)

			STRING_ENTRY(symbol_m);
			STRING_ENTRY(comment_m)
			DOUBLE_ENTRY(orderIssueTime_m)
			DOUBLE_ENTRY(orTime_m)
			DOUBLE_ENTRY(orPrice_m )
			DOUBLE_ENTRY(orStopPrice_m)
			DOUBLE_ENTRY( orSLprice_m )
			DOUBLE_ENTRY( orTPprice_m )
			INT_ENTRY( orVolume_m )
			//INT_ENTRY( signDigits_m )
			DOUBLE_ENTRY(  orExpirationTime_m )
			INT_ENTRY2(orderValidity_m, OrderValidity )
			

			// for stocks only
			STRING_ENTRY(place_m)
			STRING_ENTRY(method_m)
			STRING_ENTRY(additionalInfo_m)
			INT_ENTRY2(orPriceType_m, PriceConst)
			INT_ENTRY2(orStopPriceType_m, PriceConst)

			// only specified type
			STRINGCONTEXT_ENTRY(context_m)
		END_SERIALIZE_MAP()


	
		// this is internal id resolving provider/symbol
		// non serializable
		//mutable int cookie_symb_prov_m;

		BrokerResponseContext context_m;

	public:


		/**
		* Helper functions to processs symbol numerator & denominator
		*/

		void clear()
		{
			orderType_m = OP_DUMMY;
			orTime_m = -1;
			orPrice_m = -1;
			orStopPrice_m = -1;
			orSLprice_m = -1;
			orTPprice_m = -1;
			orVolume_m = -1;
			orExpirationTime_m = -1;
			orderIssueTime_m = -1;
			orPriceType_m = PC_ZERO;
			orStopPriceType_m = PC_ZERO;
			orderValidity_m = OV_DUMMY;

			place_m.clear();
			method_m.clear();
			additionalInfo_m.clear();
			brokerPositionID_m.clear();
			brokerOrderID_m.clear();
			comment_m.clear();

			provider_m.clear();
			//symbolNum_m.clear();
			//symbolDenom_m.clear();

			symbol_m.clear();

			ID_m.generate();

		}
		
	};


	// This is the base class for all the broker responses
	// contains general information
	

	class BrokerResponseBase: private HlpStruct::SerialImpl<BrokerResponseBase> {
	public:
		BrokerResponseBase():
				resultCode_m(PT_DUMMY),
				resultDetailCode_m(ODR_DUMMY),
				brokerInternalCode_m(-1),
				type_m(RT_BrokerResponseDummy),
				brokerIssueTime_m(-1)
		{
			
		}
				
					
		// native ID
		CppUtils::Uid  ID_m;

		// this is parent id - e.i. if a response was a result of order execution this is 
		CppUtils::Uid  parentID_m;

		
		// broker string response
		CppUtils::String  brokerComment_m;

		// internal broker code 
		int  brokerInternalCode_m;

		// result of broker operation
		OperationResult resultCode_m;

		// this is detailed operation result
		int resultDetailCode_m;

		// response type
		ResponseTypes  type_m;

		// broker resposne issue time
		double brokerIssueTime_m;


		
		// serailization map
		BEGIN_SERIALIZE_MAP()
			UID_ENTRY( ID_m )
			UID_ENTRY( parentID_m )
			STRING_ENTRY(brokerComment_m)
			INT_ENTRY(brokerInternalCode_m)
			INT_ENTRY2(resultCode_m, OperationResult)
			INT_ENTRY2(resultDetailCode_m, OperationDetailResult)
			INT_ENTRY2(type_m, ResponseTypes)
			DOUBLE_ENTRY(brokerIssueTime_m);

			// only specified type
			STRINGCONTEXT_ENTRY(context_m)
		END_SERIALIZE_MAP()

	
		// this generally has references to orders which are alive 
		BrokerResponseContext context_m;

		// this generally contains context for orders which issued current response, like order to cancel
		// can be NULL
		BrokerResponseContext causeContext_m;
		
	};

	// this is delayed orders notification
	class BrokerResponseDelayedOrder: public BrokerResponseBase {
	public:
		BrokerResponseDelayedOrder():
			delayedOrderType_m(DO_DUMMY),
			execPrice_m(-1),
			doType_m(DORT_DUMMY)
		{
			type_m = RT_BrokerResponseDelayedOrder;
		}

		BrokerResponseDelayedOrder(
			DelayedOrderResponseType const doType, 
			DelayedOrderType const delayedOrderType,
			Order const& order,
			double const& execPrice,
			CppUtils::String const& comment
		):
			delayedOrderType_m(delayedOrderType),
			execPrice_m(execPrice),
			doType_m(doType),
			order_m(order),
			comment_m(comment)
		{
			type_m = RT_BrokerResponseDelayedOrder;
			ID_m.generate();
		}

		DelayedOrderType delayedOrderType_m;
		double execPrice_m;
		Order order_m;
		DelayedOrderResponseType doType_m;

		// for any type of comments
		CppUtils::String comment_m;


	};

	// this is the response containing order information
	// that is usually arrive 
	class BrokerResponseOrder: public HlpStruct::SerialDeriveImpl<BrokerResponseBase> {
	public:
		BrokerResponseOrder():
			opPrice_m(-1),
			opStopPrice_m(-1),
			opTPprice_m(-1),
			opSLprice_m(-1),
			opVolume_m(-1),
			opRemainingVolume_m(-1),
			opTime_m(-1),
			opComission_m(-1),
			opExpirationTime_m(-1),
			opOrderValidity_m(OV_DUMMY),
			opOrderType_m(OP_DUMMY)
		
		{
			type_m = RT_BrokerResponseOrder;
		}

		// broker position ID - if applicable
		CppUtils::String brokerPositionID_m;

		// broker order ID - if new order is created or modified
		CppUtils::String brokerOrderID_m;

				
		// this is just the price- either install or open or close- depending on the response
		double  opPrice_m;

		// this is for stop orders - STOP price
		double opStopPrice_m;

		// if broker set up TP for position- this is broker TP price
		double  opTPprice_m;

		// if broker set up SL  for position - this is broker SL price
		double  opSLprice_m;

		// broker volume if broker made operation
		// here volume has the sence of the direct amount
		// it must be re-calculated to lots if ANY
		int  opVolume_m;

		// sometimes it returns remaining volume to execute
		int opRemainingVolume_m;

		
		// data provider
		CppUtils::String provider_m;

		CppUtils::String symbol_m;
		
		// if broker change expiration time  - this is reflected here
		double  opExpirationTime_m;

		// if broker changes order validity
		OrderValidity opOrderValidity_m;

		OrderType opOrderType_m;

		// order open time or execute time
		double  opTime_m;
		
		// comission
		double  opComission_m;

	
		// serailization map
		BEGIN_SERIALIZE_MAP()
			PROCESS_BASE();

		
			STRING_ENTRY( brokerPositionID_m );
			STRING_ENTRY( brokerOrderID_m );
			DOUBLE_ENTRY(  opPrice_m );
			DOUBLE_ENTRY(  opStopPrice_m );
			DOUBLE_ENTRY(  opTPprice_m );
			DOUBLE_ENTRY(  opSLprice_m );
			INT_ENTRY(  opVolume_m );
			INT_ENTRY(	 opRemainingVolume_m );
		
			STRING_ENTRY(provider_m)
			//STRING_ENTRY( symbolNum_m );
			//STRING_ENTRY( symbolDenom_m );
			STRING_ENTRY( symbol_m);

			DOUBLE_ENTRY(  opExpirationTime_m );
			INT_ENTRY2(opOrderValidity_m, OrderValidity );
			INT_ENTRY2(opOrderType_m, OrderType );
			DOUBLE_ENTRY(  opTime_m );
			DOUBLE_ENTRY(  opComission_m );
		END_SERIALIZE_MAP()


	
					
	};

	
	// Broker response telling that position was charged or other current state of equity
	class BrokerResponseEquity: public HlpStruct::SerialDeriveImpl<BrokerResponseBase>
	{
		public:
		BrokerResponseEquity():
			curSmbNumSmbRepExchangeRate_m(-1),
			curPositionDenomProfit_m(-1),
			curPositionReportProfit_m(-1),
			opChargeDenomCurrency_m(-1),
			opChargeOperationPct_m(-1),
			signDigits_m(-1)
		{
			type_m = RT_BrokerResponseEquity;
		}
		// position broker ID
		CppUtils::String  brokerPositionID_m;

		// this is exchange rate to calculate profit in report currency
		double  curSmbNumSmbRepExchangeRate_m;

		// current profit (equity) in denominator currency
		double  curPositionDenomProfit_m;

		// current position profit (equity) in report currency
		double  curPositionReportProfit_m;

		// comission / charges in Denominator currency
		double  opChargeDenomCurrency_m;

		// commisions charges in % from position
		double  opChargeOperationPct_m;

		// number of significant digits when perfroming calculations
		int signDigits_m;

		BEGIN_SERIALIZE_MAP()
			PROCESS_BASE();
			
			STRING_ENTRY(brokerPositionID_m)
			DOUBLE_ENTRY(curSmbNumSmbRepExchangeRate_m)
			DOUBLE_ENTRY(curPositionDenomProfit_m)
			DOUBLE_ENTRY(curPositionReportProfit_m)
			DOUBLE_ENTRY(opChargeDenomCurrency_m)
			DOUBLE_ENTRY(opChargeOperationPct_m)
			INT_ENTRY(signDigits_m)
		END_SERIALIZE_MAP()
				
	};

	// Broker response for symbol metainfo
	class BrokerResponseMetaInfo: public HlpStruct::SerialDeriveImpl<BrokerResponseBase> {
	public:
		BrokerResponseMetaInfo():
			signDigits_m(-1),
			pointValue_m(-1),
			spread_m(-1)
		{
			type_m = RT_BrokerResponseMetaInfo;
		}

		// symbol that is unique for the defined broker - in numenator
		//CppUtils::String  symbolNum_m;

		// the symbol in denuminator 
		//CppUtils::String  symbolDenom_m;

		CppUtils::String  symbol_m;


		// the value of one point for the pair
		double pointValue_m;

		// spread
		double spread_m;

		// the number of significant dogits
		int signDigits_m;

		BEGIN_SERIALIZE_MAP()
			PROCESS_BASE();
			
			//STRING_ENTRY(symbolNum_m)
			//STRING_ENTRY(symbolDenom_m)
			STRING_ENTRY(symbol_m);
			DOUBLE_ENTRY(pointValue_m)
			DOUBLE_ENTRY(spread_m)
			INT_ENTRY(signDigits_m)
		END_SERIALIZE_MAP()

	};
	

	// Broker response for current tick prices
	class BrokerResponseTick: public HlpStruct::SerialDeriveImpl<BrokerResponseBase> {
		public:
		BrokerResponseTick():
			opBid_m(-1),
			opAsk_m(-1),
			opOpInt_m(-1),
			signDigits_m(-1)
			{
				type_m = RT_BrokerResponseTick;
			};

		// symbol that is unique for the defined broker - in numenator
		//CppUtils::String  symbolNum_m;

		// the symbol in denuminator 
		//CppUtils::String  symbolDenom_m;

		CppUtils::String  symbol_m;

		// current prices & open interest
		double  opBid_m;

		double  opAsk_m;
		
		double  opOpInt_m;

		// the significan digits number
		int signDigits_m;

		BEGIN_SERIALIZE_MAP()
			PROCESS_BASE();
			
			//STRING_ENTRY(symbolNum_m)
			//STRING_ENTRY(symbolDenom_m)

			STRING_ENTRY(symbol_m);
			DOUBLE_ENTRY(opBid_m)
			DOUBLE_ENTRY(opAsk_m)
			DOUBLE_ENTRY(opOpInt_m)
			INT_ENTRY(signDigits_m)
		END_SERIALIZE_MAP()


	};

	// this if we wants to get the history directly from broker
	class BrokerResponseHistory: public HlpStruct::SerialDeriveImpl<BrokerResponseBase> {
		public:
			BrokerResponseHistory():
				signDigits_m(-1)
			{
				type_m = RT_BrokerResponseHistory;
			}
		// symbol that is unique for the defined broker - in numenator
		//CppUtils::String  symbolNum_m;

		// the symbol in denuminator 
		//CppUtils::String  symbolDenom_m;

		CppUtils::String  symbol_m;

		// actual history
		HlpStruct::RtDataList history_m;

		// the significan digits number
		int signDigits_m;

		BEGIN_SERIALIZE_MAP()
			PROCESS_BASE();
			
			//STRING_ENTRY(symbolNum_m)
			//STRING_ENTRY(symbolDenom_m)
			
			STRING_ENTRY(symbol_m);

			INT_ENTRY(signDigits_m)
		END_SERIALIZE_MAP()
		

	};

	// any kinds of information messages from broker
	class BrokerResponseInfo: public HlpStruct::SerialDeriveImpl<BrokerResponseBase> {
		BrokerResponseInfo():
			newsDate_m(-1)
		{
			type_m = RT_BrokerResponseInfo;
		}

		// string information
		CppUtils::String  news_m;

		// the date relevant to information
		double  newsDate_m;

		BEGIN_SERIALIZE_MAP()
			PROCESS_BASE();
			
			STRING_ENTRY(news_m)
			DOUBLE_ENTRY(newsDate_m)
		END_SERIALIZE_MAP()

	
	};

	// any kind of error messages from broker
	class BrokerErrorInfo: public HlpStruct::SerialDeriveImpl<BrokerResponseBase>
	{
		public:
		BrokerErrorInfo():
			errorDate_m(-1),
			errorCode_m(-1)
		{
			type_m = RT_BrokerErrorInfo;
		};

		// string error info
		CppUtils::String  error_m;

		// releval error time
		double  errorDate_m;

		// error code
		int errorCode_m;

		// if relevant to some kind of order
		CppUtils::String  brokerPositionID_m;

		BEGIN_SERIALIZE_MAP()
			PROCESS_BASE();
			
			STRING_ENTRY(error_m)
			DOUBLE_ENTRY(errorDate_m)
			INT_ENTRY(errorCode_m)
		END_SERIALIZE_MAP()


	};


		class BrokerResponseClosePos: public HlpStruct::SerialDeriveImpl<BrokerResponseBase> {
		public:

			BrokerResponseClosePos():
				volume_m(-1),
				priceOpen_m(-1),
				priceClose_m(-1),
				tradeDirection_m(TD_NONE),
				comissionAbs_m(-1),
			  comissionPct_m(-1),
				posOpenTime_m(-1),
				posCloseTime_m(-1)

			{
				type_m = RT_BrokerResponseClosePos;
			}
		
			CppUtils::String provider_m;

			CppUtils::String symbol_m;

			int volume_m;

			double priceOpen_m;

			double priceClose_m;

			double comissionAbs_m;

			double comissionPct_m;

			double posOpenTime_m;

			double posCloseTime_m;

			CppUtils::String brokerPositionID_m;

			TradeDirection tradeDirection_m;

			BEGIN_SERIALIZE_MAP()
				PROCESS_BASE();
				STRING_ENTRY(provider_m)
				STRING_ENTRY(symbol_m)
				STRING_ENTRY(brokerPositionID_m)
				DOUBLE_ENTRY(priceOpen_m)
				DOUBLE_ENTRY(priceClose_m)
				DOUBLE_ENTRY(comissionAbs_m)
				DOUBLE_ENTRY(comissionPct_m)
				DOUBLE_ENTRY(posOpenTime_m)
				DOUBLE_ENTRY(posCloseTime_m)
				INT_ENTRY(volume_m)
				INT_ENTRY2(tradeDirection_m, TradeDirection)

			END_SERIALIZE_MAP()

		};


		// this happens when broker wants to give an update about operation - buy or sell occured  
		class BrokerResponsePositionOperation: public HlpStruct::SerialDeriveImpl<BrokerResponseBase> {
		public:

			BrokerResponsePositionOperation():
				volume_m(0),
				direction_m(TD_NONE)
				{
					type_m = RT_BrokerResponsePositionOperation; 
				}

				CppUtils::String brokerPositionID_m;

				CppUtils::String brokerOrderID_m;

				CppUtils::String provider_m;

				CppUtils::String symbol_m;

				double volume_m;

				double price_m;

				TradeDirection direction_m;

				BEGIN_SERIALIZE_MAP()
					PROCESS_BASE();
					STRING_ENTRY(brokerPositionID_m)
					STRING_ENTRY(brokerOrderID_m)
					STRING_ENTRY(provider_m)
					STRING_ENTRY(symbol_m)
					DOUBLE_ENTRY(volume_m)
					DOUBLE_ENTRY(price_m)
					INT_ENTRY2(direction_m, TradeDirection)

				END_SERIALIZE_MAP()

		};


	/**
	 Position structure
	 it contains all necessary information on position,
	 Position structure can be uniquely calculated via processing sequence of Operations.
	 But in practice broker can do something without notification.
	 Thus generally the list of of positions is returned by broker as a report
	*/

	class Position : HlpStruct::SerialImpl<Position>{
	public:
		Position():
			timeOpen_m(-1),
			timeClose_m(-1),
			avrOpenPrice_m(-1),
			avrClosePrice_m(-1),
			volumeImbalance_m(-1),
			volume_m(-1),
			direction_m(TD_NONE),
			posState_m(STATE_DUMMY)
		{
		}


		public:

			
		// internal ID
		CppUtils::Uid ID_m;
				
		// position broker ID
		CppUtils::String brokerPositionID_m;

		// the time we successfully opened position
		double timeOpen_m;

		// the time when we closed the position
		double timeClose_m;

		// the average open price
		double avrOpenPrice_m;

		// the average close Price
		double avrClosePrice_m;

		// if it is not closed it is non-zero
		double volumeImbalance_m;

		// this is actual volume position was opened
		double volume_m;

		// trade direction
		TradeDirection direction_m;

		// RT provider
		CppUtils::String provider_m;

		// symbol that is unique for the defined broker - in numenator
		CppUtils::String symbol_m;

		// position state
		// whether it is alive, history or closed
		PosState posState_m;
		

		BEGIN_SERIALIZE_MAP()
			UID_ENTRY(ID_m)
			STRING_ENTRY(brokerPositionID_m)
			DOUBLE_ENTRY(timeOpen_m)
			DOUBLE_ENTRY(timeClose_m)
			DOUBLE_ENTRY( avrOpenPrice_m );
			DOUBLE_ENTRY( avrClosePrice_m );
			DOUBLE_ENTRY( volumeImbalance_m );
			DOUBLE_ENTRY( volume_m );
			STRING_ENTRY( provider_m );
			STRING_ENTRY( symbol_m );
			INT_ENTRY2(direction_m, TradeDirection );
			INT_ENTRY2(posState_m, PosState);
		
		END_SERIALIZE_MAP()
	};

	typedef vector<Position> PositionList;

	/**
	* Helper straucture containing info for backtesting. Real brokers must get this info as they can
	*/
	struct SymbolTradingInfo
	{
		SymbolTradingInfo():
			spread_m(0),
			slippage_m(0),
			minStopLossDistance_m(0),
			minTakeProfitDistance_m(0),
			comission_m(0),
			comissionPct_m(0),
			minLimitOrderDistance_m(0),
			minStopOrderDistance_m(0),
			executePosExactPrices_m(true),
			signDigits_m(-1),
			pointValue_m(0)
		{
			
		}
			
		CppUtils::String toString()	const
		{
			CppUtils::String result;

			result += "spread_m=" + CppUtils::simpleFloat2String(spread_m) + ";";
			result += "slippage_m=" + CppUtils::simpleFloat2String(slippage_m) + ";";
			result += "minStopLossDistance_m=" + CppUtils::simpleFloat2String(minStopLossDistance_m) + ";";
			result += "minTakeProfitDistance_m=" + CppUtils::simpleFloat2String(minTakeProfitDistance_m) + ";";
			result += "minLimitOrderDistance_m=" + CppUtils::simpleFloat2String(minLimitOrderDistance_m) + ";";
			result += "minStopOrderDistance_m=" + CppUtils::simpleFloat2String(minStopOrderDistance_m) + ";";
			result += "comission_m=" + CppUtils::simpleFloat2String(comission_m) + ";";
			result += "comissionPct_m=" + CppUtils::simpleFloat2String(comissionPct_m) + ";";
			result += "executePosExactPrices_m=" + CppUtils::String(executePosExactPrices_m?"true":"false") + ";";
			result += "pointValue_m=" + CppUtils::simpleFloat2String(pointValue_m) + ";";
			result += "signDigits_m=" + CppUtils::long2String(signDigits_m) + ";";


			return result;
		}

		// spread
		double spread_m;

		// slippage
		double slippage_m;

		// minimum stop loss distance to the current price
		double minStopLossDistance_m;

		// minimum take profit distance to the current price
		double minTakeProfitDistance_m;

		// minimum distance for limit orders
		double minLimitOrderDistance_m;

		// minimum distance for stop orders
		double minStopOrderDistance_m;

		// comission
		// assuming this is calculated just before position is closed
		double comission_m;

		double comissionPct_m;

		// if this set to true when position must be opened, open price is set to what we expected (deteriorate by slippage)
		// if set to false the open/close price is determined by the last bid/ask price that leaded to execute the order
		bool executePosExactPrices_m;

		// point value
		double pointValue_m;

		// significant digits
		int signDigits_m;
		

	};

	// -------------------------------------------

	
	struct SimulationProfileData
	{
	public:

		SimulationProfileData():
			genTickMethod_m(HlpStruct::TG_Dummy),
			aggrPeriod_m(HlpStruct::AP_Dummy),
			multFactor_m(-1),
			tickSplitNumber_m(-1),
			tickUsage_m(HlpStruct::PC_USE_DUMMY),
			simCandlePeriod_m(-1)
		{
		}
		
		void printDebugInfo() const
		{
			LOG(MSG_INFO, "BROKERSTRUCT", "Simulation profile data dump as follows:");
			for(map<CppUtils::String, map<CppUtils::String, SymbolTradingInfo > >::const_iterator it = symbolsTradingInfo_m.begin(); it != symbolsTradingInfo_m.end(); it++) {
				map<CppUtils::String, BrkLib::SymbolTradingInfo > const& map2 = it->second;
				CppUtils::String const& provider_i = it->first;

				for(map<CppUtils::String, BrkLib::SymbolTradingInfo >::const_iterator it2 = map2.begin(); it2 != map2.end(); it2++) {
					CppUtils::String const& symbol_i = it2->first;

					LOG(MSG_INFO, "BROKERSTRUCT", "Simulation profile data passed for: " << provider_i << " - " << symbol_i );
				}

				
			}

		}

		 // generate tick method
		HlpStruct::TickGenerator genTickMethod_m;
    
    // aggregation period
		HlpStruct::AggregationPeriods aggrPeriod_m;
    
    // multiplication factor
    int multFactor_m; 
    
    // tick split number
    int tickSplitNumber_m;
    
    // either Bid or Ask
		HlpStruct::TickUsage tickUsage_m;

		// provider symbol list with trading info
		map<CppUtils::String, map<CppUtils::String, BrkLib::SymbolTradingInfo > > symbolsTradingInfo_m;

		// this is special flag calculating later
		int simCandlePeriod_m;
	};

	
	
}; // end of namespace
#endif