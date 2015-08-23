#ifndef _GERCHIK_RANGE_BREAK_EVENT_INCLUDED
#define _GERCHIK_RANGE_BREAK_EVENT_INCLUDED

#include "gerchik_range_breakdefs.hpp"
//#include "../brklib/brklib.hpp"
#include "utils.hpp"
#include "brokercontext.hpp"

namespace AlgLib {

	DEFINE_ENUM_ENTRY(TE_Dummy, 0);
	DEFINE_ENUM_ENTRY(TE_BollingerTradeSignal, 1);
	DEFINE_ENUM_ENTRY(TE_BollingerNotTradeSignal, 2);

	DEFINE_ENUM_ENTRY(TE_BrokerConfirm_EnterBoth, 3);
	DEFINE_ENUM_ENTRY(TE_BrokerReject_EnterBoth, 4);

	DEFINE_ENUM_ENTRY(TE_BrokerConfirmEnterOpened, 5);
	DEFINE_ENUM_ENTRY(TE_BrokerRejectEnterOpened, 6);

	DEFINE_ENUM_ENTRY(TE_BrokerConfirmOtherSideCancelled, 7);
	DEFINE_ENUM_ENTRY(TE_BrokerRejectOtherSideCancelled, 8);

	DEFINE_ENUM_ENTRY(TE_BrokerConfirmTPSLPending, 9);
	DEFINE_ENUM_ENTRY(TE_BrokerRejectTPSLPending, 10);

	DEFINE_ENUM_ENTRY(TE_BrokerConfirmTPOpened, 11);
	DEFINE_ENUM_ENTRY(TE_BrokerRejectTPOpened, 12);

	DEFINE_ENUM_ENTRY(TE_BrokerConfirmSLOpened, 13);
	DEFINE_ENUM_ENTRY(TE_BrokerRejectSLOpened, 14);

	DEFINE_ENUM_ENTRY(TE_BrokerConfirmTPCancelled, 15);
	DEFINE_ENUM_ENTRY(TE_BrokerRejectTPCancelled, 16);

	DEFINE_ENUM_ENTRY(TE_BrokerConfirmSLCancelled, 17);
	DEFINE_ENUM_ENTRY(TE_BrokerRejectSLCancelled, 18);

	DEFINE_ENUM_ENTRY(TE_BrokerConfirmBothCancelled, 19);
	DEFINE_ENUM_ENTRY(TE_BrokerRejectBothCancelled, 20);

	DEFINE_ENUM_ENTRY(TE_Manually_Fixed, 21);


	// these are events
	struct TradingEvent {

		// -------------
		TradingEvent():
			event_m(TE_Dummy),
			eventLocalTime_m(-1),
			eventIssuerTime_m(-1),
			isLong_m(DIRECTION_DUMMY)
		{
		}

		TradingEvent( CppUtils::EnumerationHelper const& event, CppUtils::String const& symbol, CppUtils::EnumerationHelper const& isLong, double const& eventIssuerTime, bool insertToTarget):
			event_m(event),
			symbol_m(symbol),
			eventIssuerTime_m( eventIssuerTime ),
			eventLocalTime_m( CppUtils::getTime() ),
			isLong_m(isLong)
		{
			uid_m.generate();

			if (insertToTarget) {
				targetSymbols_m.push_back(symbol);
			}
		}

		// when long or shot is not relevant
		TradingEvent( CppUtils::EnumerationHelper const& event, CppUtils::String const& symbol, double const& eventIssuerTime, bool insertToTarget):
			event_m(event),
			symbol_m(symbol),
			eventIssuerTime_m( eventIssuerTime ),
			eventLocalTime_m( CppUtils::getTime() ),
			isLong_m(DIRECTION_DUMMY)
		{
			uid_m.generate();

			if (insertToTarget) {
				targetSymbols_m.push_back(symbol);
			}
		}

		

		CppUtils::String toString() const {
			CppUtils::String result = "EVENT: " + uid_m.toString() + "\n";
	
			result +=  " [ "+ symbol_m + " ] " + event_m.getName(); 
			
			result += "\nEVENT VARIABLES [ :\n";
			result += "local time: ( " + CppUtils::getGmtTime(eventLocalTime_m) + " )\n";
			result += "issuer time: ( " + CppUtils::getGmtTime(eventIssuerTime_m) + " )\n";
			result += "target symbol(s) size: " + CppUtils::long2String(targetSymbols_m.size()) + "\n";
			result += CppUtils::String("isLong_m=") + isLong_m.getName() + "\n";
			result += " ] EVENT END\n";

			return result;
		}

	
		// members
public:

		CppUtils::EnumerationHelper event_m;

		// event time (according stock)
		// this is local time
		double eventLocalTime_m;

		// this is the time of who raised that event
		double eventIssuerTime_m;

		// symbol generated the event
		CppUtils::String symbol_m;

		// if event relates to long (position)
		CppUtils::EnumerationHelper isLong_m;

		// target symbol - when one symbol can enforce events for othr symbols
		CppUtils::StringList targetSymbols_m;

		// UID
		CppUtils::Uid uid_m;



	};

	typedef vector<TradingEvent> TradingEventList;



};

#endif