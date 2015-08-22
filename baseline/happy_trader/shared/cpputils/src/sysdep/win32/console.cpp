#include "console.hpp"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "../../except.hpp"

namespace CppUtils {

	Console::Console():
		hf_out_m(0),
		hCrt_m(-1)
	{
		
	}

	Console::~Console()
	{
		deinit();
	}

	Console& Console::instance()
	{
		static Console instance;
		return instance;
	}

	void Console::deinit()
	{
		if (hCrt_m != -1) {
			_close(hCrt_m);
			hCrt_m = -1;
			hf_out_m = 0;

			FreeConsole();
		}
	}

	void Console::init()
	{
		if (hCrt_m == -1) {
			AllocConsole();

			HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
			hCrt_m = _open_osfhandle((long) handle_out, _O_TEXT);
			if (hCrt_m >=0) {
				hf_out_m = _fdopen(hCrt_m, "w");
				if (hf_out_m != 0) {
					setvbuf(hf_out_m, NULL, _IONBF, 1);
					*stdout = *hf_out_m;
				}
				else {
					deinit();
					THROW(CppUtils::OperationFailed, "exc_CannotOpenConsole", "ctx_Console", CppUtils::getOSError());
				}
			}
			else {
				deinit();
				THROW(CppUtils::OperationFailed, "exc_CannotOpenConsole", "ctx_Console", CppUtils::getOSError());
			}
		}
	}

	void Console::out(char const* str)
	{
		if (str) {
			if (hf_out_m) {
				printf( str );
			}
		}
	}


	

};