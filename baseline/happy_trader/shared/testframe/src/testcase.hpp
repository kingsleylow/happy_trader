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
// File       : testcase.hpp
// Package		: testframe
// Description: Header declaring base class for test cases.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testcase.hpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#ifndef TESTFRAME_TESTCASE_INCLUDED
#define TESTFRAME_TESTCASE_INCLUDED


#include "testbase.hpp"
#include "testfun.hpp"
#include <list>



// We are in namespace Testframe
namespace Testframe {


//=============================================================
// Class TestCase
//=============================================================
/** Class TestCase is a base class for implementing tests. A
		derived class must implement test function as members with
		void (void) signature. In the constructor it must then record
		all its test functions by using the addTest() method. The
		whole set of test can then be run via the run() function.

		Test functions must use the ASSERT() macro to check for predicates.
		The TestCase class cooperates with the TestMonitor class to
		record a report.

		@author Olaf Lenzmann
*/
class TESTFRAME_EXP TestCase : public TestBase {

public:
	//
	// Ctors & Dtor
	//
	/** Ctor taking a name of the test as arg. */
	TestCase (const char *name) : TestBase(name), testFuns_m() {};
	/** Dtor. Deletes functors objects. */
	~TestCase ();

	//
	// Functions
	//
	/** Overrides inherited function to run all tests that have
			been added to the test case. Running is canceled, if an
			external entity has signalled to stop by calling cancel()
			from a different thread.
	*/
	void run (void);
	/** Adds a test function to the set of functions to be executed
			with a given name. Functions must be members of the instance that
			this is called on. The test case takes ownership of the functions
			and will delete them. The tests will be run in the order that
			they are added in. */
	void addTestFunction (const char *name, VoidFunction *testFun);
	/** Returns the number of test functions in the internal set. */
	int numberOfTests (void);


protected:

	//
	// Functions
	//
	/** Pure virtual function that derived classes must override to prepare
			running their tests. */
	virtual void setup (void) = 0;
	/** Pure virtual function that derived classes must override to clean
			up after running their tests. */
	virtual void wrapup (void) = 0;


private:
	/** Helper structure to record test functions with name. */
	struct TestFunction {
		char *name_m;
		VoidFunction *testFunction_m;
	};

	/** List holding the test functions to run by this test. */
	list<TestFunction> testFuns_m;
};



// End of namespace
}


#endif // TESTFRAME_TESTCASE_INCLUDED

