#ifndef _GERCHIK_RANGE_BREAK2_METACOMMADDESCRIPTOR_INCLUDED
#define _GERCHIK_RANGE_BREAK2_METACOMMADDESCRIPTOR_INCLUDED

#include "utils.hpp"
#include "signaller.hpp"


namespace AlgLib {

	class MetacommadDescriptor {

	public:

		MetacommadDescriptor()
		{
			clear();
		}

		void clear()
		{
		
			orderSignStr_m = "";
			orderTPSignStr_m = "";
			orderSLSignStr_m = "";
			commandType_m = CT_BREAKTHROUGH_DUMMY;

			orderSign_m.clear();
			orderTPSign_m.clear();
			orderSLSign_m.clear();
		};

		void parse()
		{
			orderSign_m.fromString(orderSignStr_m);
			orderTPSign_m.fromString(orderTPSignStr_m);
			orderSLSign_m.fromString(orderSLSignStr_m);
		}

		// ---------------
		
		CppUtils::EnumerationHelper commandType_m;
		CppUtils::String orderSignStr_m;
		CppUtils::String orderTPSignStr_m;
		CppUtils::String orderSLSignStr_m;

		Signaller orderSign_m;
		Signaller orderTPSign_m;
		Signaller orderSLSign_m;

	};

};

#endif