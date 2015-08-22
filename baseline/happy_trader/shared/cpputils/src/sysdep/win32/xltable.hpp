#ifndef _CPPUTILS_XLTABLE_INCLUDED
#define _CPPUTILS_XLTABLE_INCLUDED


#include "../../cpputilsdefs.hpp"


namespace CppUtils {

	// --------------------------

	enum DataBlockTypes {
		tdtTable	= 0x0010,
		tdtFloat	= 0x0001,
		tdtString	= 0x0002,
		tdtBool		= 0x0003,
		tdtError	= 0x0004,
		tdtBlank	= 0x0005,
		tdtInt		= 0x0006,
		tdtSkip		= 0x0007
	};

	enum ErrorCodes {
		ERR_NULL  = 0,
		ERR_DIV_0 = 7,
		ERR_VALUE = 15, 
		ERR_REF   = 23,
		ERR_NAME  = 29,
		ERR_NUM   = 36,
		ERR_NA    = 42
	};

	// --------------------------
	CPPUTILS_EXP void crackXtableData(vector< vector <String> >& out, CppUtils::Byte const* pData, size_t pDataLength);

	// helpers
	void addConsequentElement(
		int& currow, 
		int& curcol, 
		vector< vector <String> >& out, 
		int const maxrows, 
		int const maxcols,
		CppUtils::String const& value,
		CppUtils::String& prevVal
	);

};

#endif
