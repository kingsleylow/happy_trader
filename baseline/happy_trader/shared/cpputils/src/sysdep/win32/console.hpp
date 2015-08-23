#ifndef _CPPUTILS_CONSOLE_INCLUDED
#define _CPPUTILS_CONSOLE_INCLUDED


#include "../../cpputilsdefs.hpp"
#include "../../mutex.hpp"
#include "../../thread.hpp"

#include <iostream>
#include "Wincon.h"

namespace CppUtils {

	class CPPUTILS_EXP Console
	{
		
	public:
	
		~Console();

		static Console& instance();

		void out(char const* str);

		void deinit();

		void init();

	private:
		
		Console();


		FILE* hf_out_m;
		int hCrt_m;

	};

	
};

#endif