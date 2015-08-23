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
// File       : testsuite.cpp
// Package		: testframe
// Description: Implements class for test suites.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testsuite.cpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#include "testsuite.hpp"
#include "testcase.hpp"
#include "testmonitor.hpp"
#include "testexcept.hpp"


// We are in namespace Testframe
namespace Testframe {


//=============================================================
// Class TestSuite
//=============================================================
TestSuite::~TestSuite ()
{
	// Delete all tests
	while( !tests_m.empty() ) {
		delete *tests_m.begin();
		tests_m.erase( tests_m.begin() );
	}
}
//-------------------------------------------------------------
void TestSuite::run (void)
{
	// Reset cancel flag
	cancel_m = false;

	// Inform the monitor
	TestMonitor::enterTestSection( name() );

	// Run all those tests functions
	for( list<TestBase *>::const_iterator test = tests_m.begin();
		test != tests_m.end(); ++test ) {

		// Run the test
		(*test)->run( );

		// Cancel?
		if( cancel_m )
			break;
	}

	// Inform the monitor
	TestMonitor::leaveTestSection();
}
//-------------------------------------------------------------
void TestSuite::cancel (void)
{
	// Inherited
	TestBase::cancel();

	// Cancel all contained tests
	for( list<TestBase *>::const_iterator test = tests_m.begin();
		test != tests_m.end(); ++test )
		(*test)->cancel();
}
//-------------------------------------------------------------
void TestSuite::addTest (TestBase *test)
{
	// FIXME: this is not thread-safe
	tests_m.push_back( test );
}
//-------------------------------------------------------------
int TestSuite::numberOfTests (void)
{
	int number = 0;

	for( list<TestBase *>::const_iterator test = tests_m.begin();
		test != tests_m.end(); ++test )
		number += (*test)->numberOfTests();

	return number;
}


// End of namespace
}
