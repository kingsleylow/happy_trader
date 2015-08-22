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
// File       : testbase.hpp
// Package		: testframe
// Description: Header declaring base class for tests.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testbase.hpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#ifndef TESTFRAME_TESTBASE_INCLUDED
#define TESTFRAME_TESTBASE_INCLUDED


#include "testsysdep.hpp"


// We are in namespace Testframe
namespace Testframe {


// Forward declarations
//
class TestMonitor;


//=============================================================
// Class TestBase
//=============================================================
/** Abstract base class declaring the interface for tests.

		@author Olaf Lenzmann
*/
class TESTFRAME_EXP TestBase {

public:

	//
	// Ctors & Dtor
	//
	/** Ctor taking a name of the test. */
	TestBase (const char *name) : cancel_m(false), name_m(strdup(name?name:"<unnamed>")) {};
	/** Virtual dtor. */
	virtual ~TestBase () { free(name_m); };

	//
	// Functions
	//
	/** Returns the name of the test. */
	const char *name (void) const { return name_m; };
	/** Pure virtual function that derived classes must implement
			to perform the actual test. */
	virtual void run (void) = 0;
	/** Pure virtual function that derived classes must implement
			to return the number of tests that they hold. */
	virtual int numberOfTests (void) = 0;
	/** Can be called from any thread to indicate that running
			the test should be cancelled. Derived implementations
			must poll for this flag. */
	virtual void cancel (void) { cancel_m = true; };


protected:

	//
	// Variables
	//
	/** Indicates if to cancel execution. */
	bool cancel_m;


private:

	//
	// Ctor
	//
	/** Private, unimplemented default ctor. */
	TestBase ();

	//
	// Variables
	//
	/** Name of the test. */
	char *name_m;
};



// End of namespace
}


#endif // TESTFRAME_TESTBASE_INCLUDED

