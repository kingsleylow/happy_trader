#include "udf_mysql_quik.hpp"
#include "implement.hpp"
#include "strsafe.h"

#pragma warning(disable : 4995)


namespace AlgLib {


	// ---------------------
	
	   
 
	#define MYSQL_QUIK_SENDER "MYSQL_QUIK_SENDER"
	#define	SETSTRLEN(szString,iNum)	((szString)[(iNum)]='\0')

   
	// ---------------------------
	
	static const char* resultOK_scm = "OK";

	// ---------------------------

	


	void init_returnString(char const* str, char *result, unsigned long *length,char *is_null)
	{
		if (str == NULL) {
			*is_null = 1;
			return;
		}
		
		unsigned long bufl = strlen(str);
		if (bufl > 255)
			bufl = 255;

		if (bufl==0) {
			*is_null = 1;
			return;
		}

		*is_null = 0;
		memcpy(result, str, strlen(str) );
		*length = bufl;
	}

	// helpers
	BOOL WINAPI TransferData(HWND hServer, DWORD dwMsg, LPWM_DATASTRUCTURE dataToSend) 
	{
		BOOL bSend;
		COPYDATASTRUCT cpStructData;
	
		cpStructData.dwData = 0;          // function identifier
		cpStructData.cbData = sizeof( WM_DATASTRUCTURE );  // size of data
		cpStructData.lpData = dataToSend;           // data structure
		//
				

		bSend = ::SendMessage(hServer,WM_COPYDATA, (WPARAM)hServer,(LPARAM)&cpStructData);
		return(bSend);
	}  


	// 
	// new.Symbol, new.Price, new.Vol, minute, hour, second, new.Operation
	my_bool send_quik_data_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
	{
		if (args->arg_count != 7)
		{
			strcpy(message,"send_quik_data() requires: symbol, price, volume, minute, hour, second, operation");
			return 1;
		}

		// types
		args->arg_type[0] = STRING_RESULT; // symbol
		args->arg_type[1] = REAL_RESULT; // price
		args->arg_type[2] = REAL_RESULT; // volume
		args->arg_type[3] = INT_RESULT; // hour 
		args->arg_type[4] = INT_RESULT; // minute
		args->arg_type[5] = INT_RESULT; // sec
		args->arg_type[6] = STRING_RESULT; // operation


		return 0;
	}

	void send_quik_data_deinit(UDF_INIT *initid)
	{
		

	}

	char* send_quik_data(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
	{
		
		WM_DATASTRUCTURE dataToSend;
		memset(&dataToSend, '\0', sizeof(WM_DATASTRUCTURE));

		dataToSend.price_m = *((double*) args->args[1]);
		dataToSend.volume_m = *((double*) args->args[2]);
		dataToSend.hour_m = int( *((longlong*)args->args[3]));
		dataToSend.minute_m =  int(*((longlong*)args->args[4]));
		dataToSend.second_m =  int(*((longlong*)args->args[5]));
		 
		memcpy(dataToSend.symbol_m, args->args[0], args->lengths[0] > 32 ? 32: args->lengths[0] );
		memcpy(dataToSend.operation_m, args->args[6], args->lengths[6] > 32 ? 32: args->lengths[6] );
		

		HWND winh = ::FindWindow( TARGET_WINDOW_CLASS, NULL);
		if (winh == NULL) {
			init_returnString("CANNOT FIND RECIPIENT HANDLE", result, length, is_null);
			return result;
		}
		

		
		try {

			
				
			// need to prepare data and send it
			BOOL rs = TransferData(winh, 0, &dataToSend);

			if (rs == FALSE) {
				init_returnString("CANNOT SEND DATA", result, length, is_null);
				return result;
			}

		}
		catch(... )
		{		init_returnString("EXCEPTION", result, length, is_null);
				return result;
		}
		
  
		//return 0;

		init_returnString(resultOK_scm, result, length, is_null);
		return result;
	}


} // end of namespace
