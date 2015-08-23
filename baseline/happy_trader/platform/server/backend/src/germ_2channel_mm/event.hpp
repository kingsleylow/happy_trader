#ifndef _GERM_2CHANNEL_MM_EVENT_INCLUDED
#define _GERM_2CHANNEL_MM_EVENT_INCLUDED

#include "germ_2channel_mmdefs.hpp"
#include "statemachinecontext.hpp"



namespace AlgLib {


	
	class TE_Enter_Long_Signal: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Enter_Long_Signal);

		TE_Enter_Long_Signal(CppUtils::String const& symbol, double const& volume):
			symbol_m(symbol),
			volume_m(volume)
		{
			
		}

		// parameters
		CppUtils::String symbol_m;

		double volume_m;

	};

	class TE_Enter_Short_Signal: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Enter_Short_Signal);

		TE_Enter_Short_Signal(CppUtils::String const& symbol, double const& volume):
			symbol_m(symbol),
			volume_m(volume)
		{
			
		}

		// parameters
		CppUtils::String symbol_m;

		double volume_m;
	};

	class TE_Broker_Confirm_Buy: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Broker_Confirm_Buy);

		TE_Broker_Confirm_Buy(CppUtils::String const& brokerOrderID):
			brokerOrderID_m(brokerOrderID)
		{
		}

		CppUtils::String brokerOrderID_m;

		
	};

	class TE_Broker_Confirm_Sell: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Broker_Confirm_Sell);

		TE_Broker_Confirm_Sell(CppUtils::String const& brokerOrderID):
			brokerOrderID_m(brokerOrderID)
		{
		}

		CppUtils::String brokerOrderID_m;

	};

	class TE_Close_Long_Signal: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Close_Long_Signal);

		TE_Close_Long_Signal(CppUtils::String const& symbol):
			symbol_m(symbol),
			volume_m(-1)
		{
		}

		virtual bool mailed_event_arrived(BaseEvent const& event_arrived)
		{
			// from broker confirmation
			if (event_arrived.getClassId() == TE_Broker_Confirm_Buy::CLASS_ID) {
				brokerOrderID_m = ((TE_Broker_Confirm_Buy const&)event_arrived).brokerOrderID_m;
			}

			// from issuing events
			if (event_arrived.getClassId() == TE_Enter_Long_Signal::CLASS_ID) {
				volume_m = ((TE_Enter_Long_Signal const&)event_arrived).volume_m;
			}

			return true;
		}

		CppUtils::String symbol_m;

		CppUtils::String brokerOrderID_m;

		double volume_m;
	};

	class TE_Close_Short_Signal: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Close_Short_Signal);

		TE_Close_Short_Signal(CppUtils::String const& symbol):
			symbol_m(symbol),
			volume_m(-1)
		{
		}

		virtual bool mailed_event_arrived(BaseEvent const& event_arrived)
		{
			
			if (event_arrived.getClassId() == TE_Broker_Confirm_Sell::CLASS_ID) {
				brokerOrderID_m = ((TE_Broker_Confirm_Sell const&)event_arrived).brokerOrderID_m;
			}

			if (event_arrived.getClassId() == TE_Enter_Short_Signal::CLASS_ID) {
				volume_m = ((TE_Enter_Short_Signal const&)event_arrived).volume_m;
			}

			return true;
		}


		CppUtils::String symbol_m;

		CppUtils::String brokerOrderID_m;

		double volume_m;
	};

	class TE_Broker_Confirm_Close_Long: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Broker_Confirm_Close_Long);

		
	};

	class TE_Broker_Confirm_Close_Short: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Broker_Confirm_Close_Short);

		

	};

	class TE_Broker_Error_Buy: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Broker_Error_Buy);
	};

	class TE_Broker_Error_Sell: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Broker_Error_Sell);
	};

	class TE_Broker_Error_Close_Long: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Broker_Error_Close_Long);
	};

	class TE_Broker_Error_Close_Short: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Broker_Error_Close_Short);
	};

	class TE_Manual_Fix: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Manual_Fix);
	};

	

	
	

};

#endif