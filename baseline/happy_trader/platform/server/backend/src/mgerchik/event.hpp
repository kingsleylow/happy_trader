#ifndef _MGERCHIK_EVENT_INCLUDED
#define _MGERCHIK_EVENT_INCLUDED

#include "../brklib/brklib.hpp"

namespace AlgLib {

	// these are events
	struct TradingEvent {

		// enumeration of all events
		enum TE {
			TE_Dummy = 0,
			
			TE_SizeBidSizeAsk_Long =			1,
			TE_NotSizeBidSizeAsk_Long =		2,

			TE_SizeBidSizeAsk_Short =			3,
			TE_NotSizeBidSizeAsk_Short =	4,

			TE_IndexIsRising =				5,
			TE_NotIndexIsRising =			6,

			TE_IndexIsFalling =				7,
			TE_NotIndexIsFalling =		8,

			TE_PriceIsRising =				9,
			TE_NotPriceIsRising =			10,

			TE_PriceIsFalling =				11,
			TE_NotPriceIsFalling =		12,

			// exitt pos events
			// conditions to exit
			TE_Exit1FromLong =				13, 
			TE_Exit1FromShort =				14,

			// second conditions to exit
			TE_Exit2FromLong =				15,
			TE_Exit2FromShort =				16,

			// conditions to cancel
			TE_CancelFromLong	=				17,
			TE_CancelFromShort=				18,

			// broker events 
			// confirmation we opened pos
			// these are all for the first type of position !!!
			TE_BrokerConfirmTradeLong					=	19,
			TE_BrokerConfirmTradeShort				=	20,

			// confirm trade is rejected
			TE_BrokerRejectTradeLong					=	21,
			TE_BrokerRejectTradeShort					=	22,

			// confirm pending
			TE_BrokerConfirmPendingLong				=	23,
			TE_BrokerConfirmPendingShort			=	24,

			// confirm cancel
			TE_BrokerConfirmCancelLong				=	25,
			TE_BrokerConfirmCancelShort				=	26,

			// reject cancel
			TE_BrokerRejectCancelLong					=	27,
			TE_BrokerRejectCancelShort				=	28,

			// broker confirms normal close
			TE_BrokerConfirmCloseLong					=	29,
			TE_BrokerConfirmCloseShort				=	30,

			TE_BrokerRejectCloseLong					=	31,
			TE_BrokerRejectCloseShort					=	32,

			TE_BrokerConfirmClose2Long				= 33,
			TE_BrokerConfirmClose2Short				= 34,

			TE_BrokerRejectClose2Long					=	35,
			TE_BrokerRejectClose2Short				=	36,

			TE_Exit3FromLong									= 37, // signal to urgent market exit	
			TE_Exit3FromShort									= 38,

			TE_BrokerConfirmClose3Long				= 39,
			TE_BrokerConfirmClose3Short				= 40,

			TE_BrokerRejectClose3Long					=	41,
			TE_BrokerRejectClose3Short				=	42,

			TE_CalculatedRiskMoreThreashold		= 43,
			TE_NotCalculatedRiskMoreThreashold = 44
			
		};

		// -------------
		TradingEvent():
			event_m(TE_Dummy),
			eventLocalTime_m(-1),
			eventIssuerTime_m(-1)
		{
		}

		TradingEvent( TE const event, CppUtils::String const& symbol, double const& eventIssuerTime, bool insertToTarget):
			event_m(event),
			symbol_m(symbol),
			eventIssuerTime_m( eventIssuerTime ),
			eventLocalTime_m( CppUtils::getTime() )
		{
			uid_m.generate();

			if (insertToTarget) {
				targetSymbols_m.push_back(symbol);
			}
		}

		

		CppUtils::String toString() const {
			CppUtils::String result = "EVENT: " + uid_m.toString() + "\n";
			switch(event_m) {
				case TE_Dummy: result +=  " [ "+  symbol_m + " ] " + "TE_Dummy"; break;
				
				case TE_SizeBidSizeAsk_Long: result +=  " [ "+  symbol_m + " ] " + "TE_SizeBidSizeAsk_Long"; break;
				case TE_NotSizeBidSizeAsk_Long: result +=  " [ "+  symbol_m + " ] " + "TE_NotSizeBidSizeAsk_Long"; break;

				case TE_SizeBidSizeAsk_Short: result +=  " [ "+  symbol_m + " ] " + "TE_SizeBidSizeAsk_Short"; break;
				case TE_NotSizeBidSizeAsk_Short: result +=  " [ "+  symbol_m + " ] " + "TE_NotSizeBidSizeAsk_Short"; break;

				case TE_IndexIsRising: result +=  " [ "+  symbol_m + " ] " + "TE_IndexIsRising"; break;
				case TE_NotIndexIsRising: result +=  " [ "+  symbol_m + " ] " + "TE_NotIndexIsRising"; break;
				case TE_IndexIsFalling: result +=  " [ "+  symbol_m + " ] " + "TE_IndexIsFalling"; break;
				case TE_NotIndexIsFalling: result +=  " [ "+  symbol_m + " ] " + "TE_NotIndexIsFalling"; break;
				case TE_PriceIsRising: result +=  " [ "+  symbol_m + " ] " + "TE_PriceIsRising"; break;
				case TE_NotPriceIsRising: result +=  " [ "+  symbol_m + " ] " + "TE_NotPriceIsRising"; break;
				case TE_PriceIsFalling: result +=  " [ "+ symbol_m + " ] " + "TE_PriceIsFalling"; break;
				case TE_NotPriceIsFalling: result +=  " [ "+  symbol_m + " ] " + "TE_NotPriceIsFalling"; break; 
				
				case TE_Exit1FromLong: result +=  " [ "+  symbol_m + " ] " + "TE_Exit1FromLong"; break;
				case TE_Exit1FromShort: result +=  " [ "+  symbol_m + " ] " + "TE_Exit1FromShort"; break;

				case TE_Exit2FromLong: result +=  " [ "+  symbol_m + " ] " + "TE_Exit2FromLong"; break;
				case TE_Exit2FromShort: result +=  " [ "+  symbol_m + " ] " + "TE_Exit2FromShort"; break;
				
				case TE_CancelFromLong: result +=  " [ "+  symbol_m + " ] " + "TE_CancelFromLong"; break;
				case TE_CancelFromShort: result +=  " [ "+  symbol_m + " ] " + "TE_CancelFromShort"; break;
				
				// broker events
				case TE_BrokerConfirmTradeLong: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmTradeLong"; break;
				case TE_BrokerConfirmTradeShort: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmTradeShort"; break;

				case TE_BrokerRejectTradeLong: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectTradeLong"; break;
				case TE_BrokerRejectTradeShort: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectTradeShort"; break;

				case TE_BrokerConfirmPendingLong: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmPendingLong"; break;
				case TE_BrokerConfirmPendingShort: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmPendingShort"; break;

				case TE_BrokerConfirmCancelLong: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmCancelLong"; break;
				case TE_BrokerConfirmCancelShort: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmCancelShort"; break;

				case TE_BrokerRejectCancelLong: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectCancelLong"; break;
				case TE_BrokerRejectCancelShort: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectCancelShort"; break;

				case TE_BrokerConfirmCloseLong: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmCloseLong"; break;
				case TE_BrokerConfirmCloseShort: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmCloseShort"; break;

				case TE_BrokerRejectCloseLong: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectCloseLong"; break;
				case TE_BrokerRejectCloseShort: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectCloseShort"; break;

				case TE_BrokerConfirmClose2Long: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmClose2Long"; break;
				case TE_BrokerConfirmClose2Short: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmClose2Short"; break;

				case TE_BrokerRejectClose2Long: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectClose2Long"; break;
				case TE_BrokerRejectClose2Short: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectClose2Short"; break;

				case TE_Exit3FromLong: result +=  " [ "+  symbol_m + " ] " + "TE_Exit3FromLong"; break;
				case TE_Exit3FromShort: result +=  " [ "+  symbol_m + " ] " + "TE_Exit3FromShort"; break;

				case TE_BrokerConfirmClose3Long: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmClose3Long"; break;
				case TE_BrokerConfirmClose3Short: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerConfirmClose3Short"; break;

				case TE_BrokerRejectClose3Long: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectClose3Long"; break;
				case TE_BrokerRejectClose3Short: result +=  " [ "+  symbol_m + " ] " + "TE_BrokerRejectClose3Short"; break;

				case TE_CalculatedRiskMoreThreashold: result +=  " [ "+  symbol_m + " ] " + "TE_CalculatedRiskMoreThreashold"; break;
				case TE_NotCalculatedRiskMoreThreashold: result +=  " [ "+  symbol_m + " ] " + "TE_NotCalculatedRiskMoreThreashold"; break;
		
				default:
					result +=  " [ "+ symbol_m + " ] " + "N/A"; 
			}

			result += "\n local time: ( " + CppUtils::getGmtTime(eventLocalTime_m) + " )\n";
			result += " issuer time: ( " + CppUtils::getGmtTime(eventIssuerTime_m) + " )\n";
			result += " target symbol(s) size: " + CppUtils::long2String(targetSymbols_m.size()) + "\n";
			result += "EVENT END\n";

			return result;
		}

	
		// members
public:

		TE event_m;

		// event time (according stock)
		// this is local time
		double eventLocalTime_m;

		// this is the time of who raised that event
		double eventIssuerTime_m;

		// symbol generated the event
		CppUtils::String symbol_m;

		// target symbol
		CppUtils::StringList targetSymbols_m;

		CppUtils::Uid uid_m;



	};

	typedef vector<TradingEvent> TradingEventList;



};

#endif