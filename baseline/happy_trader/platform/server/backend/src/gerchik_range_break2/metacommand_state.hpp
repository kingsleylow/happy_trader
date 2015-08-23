#ifndef _GERCHIK_RANGE_BREAK2_METACOMMANDSTATE_INCLUDED
#define _GERCHIK_RANGE_BREAK2_METACOMMANDSTATE_INCLUDED

#include "../brklib/brklib.hpp"
#include "orderdescr.hpp"

namespace AlgLib {
// ------------------------------------
// metacommand states
// STATES

DEFINE_ENUM_ENTRY(MCS_Dummy,0 )
DEFINE_ENUM_ENTRY(MCS_Initial,1 )
DEFINE_ENUM_ENTRY(MCS_Order_Main_Issued,2 )
DEFINE_ENUM_ENTRY(MCS_Order_TP_Issued,3 )
DEFINE_ENUM_ENTRY(MCS_Order_SL_Issued,4 )
DEFINE_ENUM_ENTRY(MCS_Order_TP_SL_Pending,5 )
DEFINE_ENUM_ENTRY(MCS_Inconsistent,6);

class MetacommandState: public EnumerationHelperBase {

public:

	MetacommandState():
		EnumerationHelperBase(MCS_Initial)
	{
	}

	MetacommandState(CppUtils::EnumerationHelper const& enumMember):
		EnumerationHelperBase(enumMember)
	{
	}

	virtual void clear()
	{
		getMember() = MCS_Initial;
	}

	inline CppUtils::EnumerationHelper const& getMetacommandState() const
	{
		return getMember();
	}

	inline CppUtils::EnumerationHelper& getMetacommandState()
	{
		return getMember();
	}


};

};

#endif