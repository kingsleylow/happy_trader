#ifndef _GERCHIK_RANGE_BREAK2_EVENT_INCLUDED
#define _GERCHIK_RANGE_BREAK2_EVENT_INCLUDED

#include "gerchik_range_break2defs.hpp"
//#include "../brklib/brklib.hpp"
#include "utils.hpp"
#include "signaller.hpp"


namespace AlgLib {

	DEFINE_ENUM_ENTRY(TE_Dummy, 0);
	DEFINE_ENUM_ENTRY(TE_InstallOrder, 1);
	DEFINE_ENUM_ENTRY(TE_BrokerConfirm_Pending, 2);
	DEFINE_ENUM_ENTRY(TE_BrokerConfirm_Open,3);
	DEFINE_ENUM_ENTRY(TE_BrokerReturnError, 4);
	DEFINE_ENUM_ENTRY(TE_UserCancel, 5);
	DEFINE_ENUM_ENTRY(TE_BrokerConfirm_Cancelled, 6);
	DEFINE_ENUM_ENTRY(TE_UserConfirmFix, 7);
	DEFINE_ENUM_ENTRY(TE_BrokerConfirm_Delayed, 8);



	// these are events
	class TradingEvent : public EnumerationHelperBase{

	public:

		// this is a context
		struct TradingEventContext
		{
			TradingEventContext():
				installTime_m(-1),
				executeTime_m(-1),
				executePrice_m(-1),
				installPrice_m(-1),
				cancelTime_m(-1),
				cancelPrice_m(-1),
				errorTime_m(-1)
			{
			}

			void clear()
			{
				installTime_m  =-1;
				executeTime_m = -1;
				executePrice_m = -1;
				installPrice_m  =-1;
				cancelTime_m = -1;
				cancelPrice_m = -1;
				errorTime_m = -1;

				signaller_m.clear();
			}

			CppUtils::String errorReason_m;

			double errorTime_m;

			CppUtils::String brokerPositionId_m;

			double installTime_m;

			double installPrice_m;

			double cancelTime_m;

			double cancelPrice_m;

			double executeTime_m;

			double executePrice_m;

			// this is order context when we issue order
			Signaller signaller_m;
		};

		// -------------
		TradingEvent():
			EnumerationHelperBase(TE_Dummy),
			eventLocalTime_m(-1),
			eventIssuerTime_m(-1)
		{
		}

		TradingEvent( CppUtils::Uid const &ruid, CppUtils::EnumerationHelper const& eventd,  double const& eventIssuerTime = -1):
			EnumerationHelperBase(eventd),
			eventIssuerTime_m( eventIssuerTime ),
			eventLocalTime_m( CppUtils::getTime() ),
			recipientOrderUid_m(ruid)
		{
		}
		
		void initialize(CppUtils::Uid const &ruid, CppUtils::EnumerationHelper const& eventd,  double const& eventIssuerTime = -1)
		{
			eventLocalTime_m = CppUtils::getTime();
			eventIssuerTime_m = eventIssuerTime;
			getEvent() = eventd;
			recipientOrderUid_m = ruid;
		}

    
		inline TradingEventContext& context()
		{
			return context_m;
		};

		inline TradingEventContext const& context() const
		{
			return context_m;
		};
		

		virtual void clear()
		{
			getEvent() = TE_Dummy;
			context().clear();
		}

		virtual CppUtils::String toString() const {
			CppUtils::String result = CppUtils::String("EVENT: [ ") + getEvent().getName() + "\n";
		
			result += " recipient UID: " + recipientOrderUid_m.toString(); 
			result += " local time: ( " + CppUtils::getGmtTime(eventLocalTime_m) + " )\n";
			result += " issuer time: ( " + CppUtils::getGmtTime(eventIssuerTime_m) + " )\n";
			result += " ] EVENT END\n";

			return result;
		}

		inline CppUtils::EnumerationHelper const& getEvent() const
		{
			return getMember();
		}

		inline CppUtils::EnumerationHelper& getEvent()
		{
			return getMember();
		}

		inline CppUtils::Uid const& getRecipientUid() const
		{
			return recipientOrderUid_m;
		}

	
		// members
private:

		
		// UID - this is a recepient UID - e.i. order UID
		CppUtils::Uid recipientOrderUid_m;

		// event time (according stock)
		// this is local time
		double eventLocalTime_m;

		// this is the time of who raised that event
		double eventIssuerTime_m;

		TradingEventContext context_m;

		
	};

	typedef vector<TradingEvent> TradingEventList;



};

#endif