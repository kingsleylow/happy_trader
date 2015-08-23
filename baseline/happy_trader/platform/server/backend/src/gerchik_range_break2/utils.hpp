#ifndef _GERCHIK_RANGE_BREAK2_UTILS_INCLUDED
#define _GERCHIK_RANGE_BREAK2_UTILS_INCLUDED

#include "gerchik_range_break2defs.hpp"

namespace AlgLib {

	// command types
	DEFINE_ENUM_ENTRY(CT_BREAKTHROUGH_DUMMY,0 )
	DEFINE_ENUM_ENTRY(CT_BREAKTHROUGH_UP,1)
	DEFINE_ENUM_ENTRY(CT_BREAKTHROUGH_DOWN,2 )
	

	
	// this is the base class for states, actrions, etc...
	class EnumerationHelperBase {
	public:
		EnumerationHelperBase()
		{
		}

		
		EnumerationHelperBase(CppUtils::EnumerationHelper const& enumMember):
			enumMember_m(enumMember)
		{
		}

		
		~EnumerationHelperBase()
		{
		}

		

		// must be implemented
		virtual void clear() = 0;
		
		inline CppUtils::EnumerationHelper& getMember()
		{
			return enumMember_m;
		}

		inline  CppUtils::EnumerationHelper const& getMember() const
		{
			return enumMember_m;
		}

		virtual CppUtils::String toString() const
		{
			return enumMember_m.getName();
		}

	private:
			CppUtils::EnumerationHelper enumMember_m;
	};
			
};

#endif
