#include "state.hpp"
#include "implement.hpp"

namespace AlgLib {

	bool TS_Initial::entry_action(CppUtils::BaseEvent const& event)
	{
		return true;
	}

	bool TS_IssueBuy::entry_action(CppUtils::BaseEvent const& event)
	{
		Grm2ChannelMM& ctx = context().getBase();
		TE_Enter_Long_Signal const* event_i = dynamic_cast<TE_Enter_Long_Signal const*>(&event);
		if (event_i != NULL) {
			ctx.issueBuyOrder( event_i->symbol_m, event_i->volume_m );
		}

		return true;
	}

	bool TS_IssueSell::entry_action(CppUtils::BaseEvent const& event)
	{
		Grm2ChannelMM& ctx = context().getBase();
		TE_Enter_Short_Signal const* event_i = dynamic_cast<TE_Enter_Short_Signal const*>(&event);
		if (event_i != NULL) {
			ctx.issueSellOrder( event_i->symbol_m, event_i->volume_m );
		}

		return true;
	}

	bool TS_IssueCloseLong::entry_action(CppUtils::BaseEvent const& event)
	{
		Grm2ChannelMM& ctx = context().getBase();
		TE_Close_Long_Signal const* event_i = dynamic_cast<TE_Close_Long_Signal const*>(&event);
		if (event_i != NULL) {
			ctx.issueCloseLongOrder( event_i->symbol_m, event_i->volume_m, event_i->brokerOrderID_m  );
		}

		return true;
	}

	bool TS_IssueCloseShort::entry_action(CppUtils::BaseEvent const& event)
	{
		Grm2ChannelMM& ctx = context().getBase();
		TE_Close_Short_Signal const* event_i = dynamic_cast<TE_Close_Short_Signal const*>(&event);
		if (event_i != NULL) {
			ctx.issueCloseShortOrder( event_i->symbol_m, event_i->volume_m, event_i->brokerOrderID_m );
		}

		return true;
	}

};

