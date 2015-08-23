#ifndef _GERCHIK_RANGE_BREAK2_METACOMMANDEVENT_INCLUDED
#define _GERCHIK_RANGE_BREAK2_METACOMMANDEVENT_INCLUDED

#include "../brklib/brklib.hpp"
#include "orderdescr.hpp"
#include "utils.hpp"

namespace AlgLib {

	// --------------------------------------
	// EVENTS

		DEFINE_ENUM_ENTRY(MAE_Dummy, 0 )
		DEFINE_ENUM_ENTRY(MAE_IssueOrder_Main, 1 )
		DEFINE_ENUM_ENTRY(MAE_Order_Main_Open, 2 )
		DEFINE_ENUM_ENTRY(MAE_Order_Main_Cancel, 3 )
		DEFINE_ENUM_ENTRY(MAE_Order_Main_Error, 4 )

		DEFINE_ENUM_ENTRY(MAE_Order_TP_Pending, 5 )
		DEFINE_ENUM_ENTRY(MAE_Order_TP_Cancel, 6 )
		DEFINE_ENUM_ENTRY(MAE_Order_TP_Error, 7 )
		
		DEFINE_ENUM_ENTRY(MAE_Order_SL_Pending, 8 )
		DEFINE_ENUM_ENTRY(MAE_Order_SL_Cancel, 9 )
		DEFINE_ENUM_ENTRY(MAE_Order_SL_Error, 10 )


class MetacommandEvent : public EnumerationHelperBase{

public:
	struct Context
	{
		CppUtils::EnumerationHelper commandType_m;
		CppUtils::String orderSignStr_m;
		CppUtils::String orderTPSignStr_m;
		CppUtils::String orderSLSignStr_m;

		Context():
			commandType_m(CT_BREAKTHROUGH_DUMMY)
		{
		}

		void clear()
		{
			commandType_m = CT_BREAKTHROUGH_DUMMY;
			orderSignStr_m = "";
			orderTPSignStr_m = "";
			orderSLSignStr_m = "";
		}

	};

	MetacommandEvent():
	EnumerationHelperBase(MAE_Dummy)
	{
	}

	
	MetacommandEvent(CppUtils::EnumerationHelper const& enumMember):
	EnumerationHelperBase(enumMember)
	{
	}

	MetacommandEvent( CppUtils::Uid const &ruid, CppUtils::EnumerationHelper const& event_id)
	{
		getMember() = event_id;
		recipientUid_m = ruid;
	}

	virtual void clear()
	{
		getMember() = MAE_Dummy;
	}

	inline CppUtils::EnumerationHelper const& getMetacommandEvent() const
	{
		return getMember();
	}

	inline CppUtils::Uid const& getRecipientUid() const
	{
		return recipientUid_m;
	}

	inline CppUtils::Uid& getRecipientUid()
	{
		return recipientUid_m;
	}

	void initialize( CppUtils::Uid const &ruid, CppUtils::EnumerationHelper const& event_id)
	{
		getMember() = event_id;
		recipientUid_m = ruid;
	}

	inline Context& context()
	{
			return context_m;
	};

	inline Context const& context() const
	{
			return context_m;
	};


	private:
		CppUtils::Uid recipientUid_m;

		Context context_m;

	};

};

#endif