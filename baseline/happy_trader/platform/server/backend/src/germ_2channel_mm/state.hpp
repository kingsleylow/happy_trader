#ifndef _GERM_2CHANNEL_MMSTATE_INCLUDED
#define _GERM_2CHANNEL_MMSTATE_INCLUDED

#include "germ_2channel_mmdefs.hpp"
#include "statemachinecontext.hpp"
#include "event.hpp"

namespace AlgLib {

	

	class TS_Initial: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_Initial);

		virtual bool entry_action(CppUtils::BaseEvent const& event); 
	};	

	class TS_Inconsistent: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_Inconsistent);
	};

	class TS_IssueBuy: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_IssueBuy);

		virtual bool entry_action(CppUtils::BaseEvent const& event); 
	};

	class TS_IssueSell: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_IssueSell);

		virtual bool entry_action(CppUtils::BaseEvent const& event); 
	};

	class TS_Long: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_Long);				
	};

	class TS_Short: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_Short);	
	};

	class TS_IssueCloseLong: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_IssueCloseLong);

		virtual bool entry_action(CppUtils::BaseEvent const& event); 
	};

	class TS_IssueCloseShort: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_IssueCloseShort);			

		virtual bool entry_action(CppUtils::BaseEvent const& event); 
	};

	class TS_If_Reverse_To_Short: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_If_Reverse_To_Short);
	};

	class TS_If_Reverse_To_Long: public CppUtils::BaseState<MachineContext>
	{
		public:
		CTOR(TS_If_Reverse_To_Long);			
	};


} // end of namespace

#endif