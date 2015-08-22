#ifndef _GERCHIK_RANGE_BREAK_SIGNALLER_INCLUDED
#define _GERCHIK_RANGE_BREAK_SIGNALLER_INCLUDED

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
	

	public:

		CppUtils::String symbol_m;

		// these are parameters for Actions
		double opSLInstallPrice_LongTrade_m;

		double opTPInstallPrice_LongTrade_m;

		double opSLInstallPrice_ShortTrade_m;

		double opTPInstallPrice_ShortTrade_m;

		double opBothSideLongInstallPrice_m;

		double opBothSideShortInstallPrice_m;

		double volume_m;
	};
};

#endif