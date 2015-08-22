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
// File       : testsuite.hpp
// Package		: testframe
// Description: Header declaring class for test suites.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testsuite.hpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#ifndef TESTFRAME_TESTSUITE_INCLUDED
#define TESTFRAME_TESTSUITE_INCLUDED


#include "testbase.hpp"
#include <set>



// We are in namespace Testframe
namespace Testframe {


// Forward declarations
//
class TestCase;


//=============================================================
// Class TestSuite
//=============================================================
/** Class TestSuite can aggregate a number of tests, i.e. objects
		that are derived from TestBase. It is itself derived from
		TestBase, allowing nested containment. When run, the suite
		runs all tests it contains.

		@author Olaf Lenzmann
*/
class TESTFRAME_EXP TestSuite : public TestBase {

public:

	//
	// Ctors & Dtor
	//
	/** Ctor taking a name of the test as arg. */
	TestSuite (const char *name) : TestBase(name) {};
	/** Dtor. Deletes all tests. */
	~TestSuite ();

	//
	// Functions
	//
	/** Overrides inherited function to run all tests that have
			been added to the test suite. Running is canceled, if an
			external entity has signalled to stop by calling cancel()
			from a different thread.
	*/
	void run (void);
	/** Overloads the inherited function to set the cancel flag
			for all test contained. */
	void cancel (void);
	/** Adds a test to the suite. The suite takes ownership of the
			test and will delete it in the dtor. */
	void addTest (TestBase *test);
	/** Returns the sum of the numbers of test functions that the
			tests contained return. */
	int numberOfTests (void);


private:
	/** List holding the tests. */
	list<TestBase *> tests_m;
};



// End of namespace
}


#endif // TESTFRAME_TESTSUITE_INCLUDED

