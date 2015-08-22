//#include "platform/server/backend/src/brklibmetatrader/brklibmetatrader.hpp"

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
//#include "wininetwrap.hpp"
//#include "DownloadDataRequest.hpp"





//
TESTCASE_BEGIN( test1 )

	

	
	TESTCASE_SETUP
	{
		
		
	}


	// =======================================================
	// Wrapup
	TESTCASE_WRAPUP
	{
		
	}


	// =======================================================
	// Test


	//DownloadDataRequest requestor;

	
	DEFINE_TEST( T1 ) 
	{
		/*
		WinHttpSession sess;
		HRESULT r = sess.Initialize();

		WinHttpConnection conn;
		r = conn.Initialize(L"127.0.0.1", 8082, sess);

		r = requestor.Initialize(
			L"/htWL4RTProviderService.jsp?alg_lib_pair=wl4_proxy&server_id=server%201&symbol=EURGBP&provider=RT%20Demo%20Provider%201&mult_factor=1&aggr_period=AP_Minute",
			L"GET",
			conn
		);

		LPCWSTR additionalHeaders = L"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727; MS-RTC LM 8; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; InfoPath.1)\r\n";
		DWORD hLen   = -1L;


		r = requestor.SendRequest(additionalHeaders, hLen, 0, 0, 0);
		*/
		CppUtils::sleep(30000);
		
	}
			
	DEFINE_TEST( T2 ) {
    
	
	}


	// =======================================================
	// Test Index

	TESTCASE_INDEX( test1 )
	{
		REGISTER_TEST( test1, &test1::T1 );
	
	}


TESTCASE_END;
