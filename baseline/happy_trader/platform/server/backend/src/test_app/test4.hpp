//

int CALLBACK qcFun(
	void* envP,  
	PDDEITEM ddeItem, 
	char*** xTableDataPPP,
	int xTableRows,
	int xTableCols)
{

	return 0;
}


TESTCASE_BEGIN( test4 )

	// =======================================================
	// Setup
	

	TESTCASE_SETUP
	{
		
		
	}


	// =======================================================
	// Wrapup
	TESTCASE_WRAPUP
	{
		
	}

	DEFINE_TEST( T1 ) 
	{
		void* envP = 0;
		int r = qc_create_enviroment(&envP);
		r = qc_release_enviroment(envP);

		int i =0;
	}

	DEFINE_TEST( T2 ) 
	{
		void* envP = 0;
		void* errP = 0;
		int r = qc_create_enviroment(&envP);
		r = qc_get_create_error_handle(envP, &errP);
		
		DDEITEM ddeInitialItemListP[] = {"[book1]sheet1", "R1C1:R1C1", "[book1]sheet1", "R1C1:R2C2",0, 0};
		r = qc_init(envP, errP, "happy_trader", ddeInitialItemListP, qcFun, NULL, NULL );

		r = qc_deinit(envP, errP);
		
		r = qc_get_release_error_handle(envP, errP);
		r = qc_release_enviroment(envP);

		int i =0;
	}

	// =======================================================
	// Test Index

	TESTCASE_INDEX( test4 )
	{
		REGISTER_TEST( test4, T1 );
		REGISTER_TEST( test4, T2 );
	}


TESTCASE_END;