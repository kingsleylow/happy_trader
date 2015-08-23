// Copyright Virtual Photonics Inc., 2000. All rights reserved.
// Carnotstrasse 6, 10587 Berlin, Germany
//
// The copyright to the computer program(s) herein is the
// property of VPI. The program(s) may be used and/or copied
// only with the written permission of VPI or in accordance
// with the terms and conditions stipulated in the agreement/
// contract under which the program(s) have been supplied.
// This copyright notice must not be removed.
//
// File       : testcase.cpp
// Package		: testframe
// Description: Implements base class for test cases.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testcase.cpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#include "testcase.hpp"
#include "testmonitor.hpp"
#include "testexcept.hpp"


// We are in namespace Testframe
namespace Testframe {


//=============================================================
// Class TestCase
//=============================================================
TestCase::~TestCase ()
{
	while( !testFuns_m.empty() ) {
		free( testFuns_m.begin()->name_m );
		delete testFuns_m.begin()->testFunction_m;
		testFuns_m.pop_front();
	}
}
//-------------------------------------------------------------
void TestCase::run (void)
{
	// Reset cancel flag
	cancel_m = false;

	// Inform the monitor
	TestMonitor::enterTestSection( name() );

	// Log memory action
	TestMonitor::setAllocationRecording(true);

	// Setup test
	setup();

	// Stop logging memory action
	TestMonitor::setAllocationRecording(false);

	// Run all those test functions
	for( list<TestFunction>::const_iterator test = testFuns_m.begin();
		test != testFuns_m.end(); ++test ) {

		// Inform the monitor
		TestMonitor::enterTestSection( test->name_m, false );

		// Log memory action
		TestMonitor::setAllocationRecording(true);

		// Run the test
		try {
			(*test->testFunction_m)();
		}
		catch( Testframe::Exception &x ) {
			TestMonitor::recordProblem( x.type().c_str(), x.message().c_str(), "n/a", -1 );
		}
		catch( ... ) {
			TestMonitor::recordProblem( "Exception", "n/a", "n/a", -1 );
		}

		// Stop logging memory action
		TestMonitor::setAllocationRecording(false);

		// Inform the monitor
		TestMonitor::leaveTestSection();

		// Cancel?
		if( cancel_m )
			break;
	}

	// Log memory action
	TestMonitor::setAllocationRecording(true);

	// Wrapup test
	wrapup();

	// Stop logging memory action
	TestMonitor::setAllocationRecording(false);

	// Inform the monitor
	TestMonitor::leaveTestSection();
}
//-------------------------------------------------------------
void TestCase::addTestFunction (const char *name, VoidFunction *testFun)
{
	// Append to the list
	TestFunction tf;
	tf.name_m = strdup(name ? name : "<unnamed>" );
	tf.testFunction_m = testFun;
	testFuns_m.push_back(tf);
}
//-------------------------------------------------------------
int TestCase::numberOfTests (void)
{
	return testFuns_m.size();
}


// End of namespace
}

