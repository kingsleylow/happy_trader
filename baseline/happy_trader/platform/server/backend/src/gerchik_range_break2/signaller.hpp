#ifndef _GERCHIK_RANGE_BREAK2_SIGNALLER_INCLUDED
#define _GERCHIK_RANGE_BREAK2_SIGNALLER_INCLUDED

#include "action.hpp"

namespace AlgLib {
	/*
	this class contains parameter for Action
	*/

	struct Signaller
	{
	public:
		Signaller();

		Signaller(CppUtils::String const& strdata);
		
		void fromString(CppUtils::String const& strdata);
		
		void clear();

	public:

		

		CppUtils::String symbol_m;

		// these are parameters for Actions
		double opIssuePrice_m;

		double volume_m;

		// if true we only register orders for execution otherwise issue to broker size
		bool orderIsDelayedLocally_m;

		CppUtils::String orderMethod_m;

		CppUtils::String orderPlace_m;

		CppUtils::String additionalOrderInfo_m;

		CppUtils::Uid orderUid_m;

		BrkLib::OrderType orderType_m;

		CppUtils::String description_m;
		
	};
};

#endif