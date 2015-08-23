#ifndef _GERCHIK_RANGE_BREAK2_METACOMMANDACTION_INCLUDED
#define _GERCHIK_RANGE_BREAK2_METACOMMANDACTION_INCLUDED

#include "../brklib/brklib.hpp"
#include "orderdescr.hpp"

namespace AlgLib {
	// ----------------------------------
	// ACTION

	// metacommand action
	DEFINE_ENUM_ENTRY(MAS_Dummy, 0 )
	DEFINE_ENUM_ENTRY(MAS_Issue_Order_Main, 1 )
	DEFINE_ENUM_ENTRY(MAS_Issue_Order_TP, 2 )
	DEFINE_ENUM_ENTRY(MAS_Issue_Order_SL, 3 )
	DEFINE_ENUM_ENTRY(MAS_Clear, 4 )

class MetacommandAction : public EnumerationHelperBase{
public:
	MetacommandAction():
	EnumerationHelperBase(MAS_Dummy)
	{
	}

	MetacommandAction(CppUtils::EnumerationHelper const& enumMember):
	EnumerationHelperBase(enumMember)
	{
	}

	virtual void clear()
	{
		getMember() = MAS_Dummy;
	}

	inline CppUtils::EnumerationHelper const& getMetacommandAction() const
	{
		return getMember();
	}

	};
};

#endif