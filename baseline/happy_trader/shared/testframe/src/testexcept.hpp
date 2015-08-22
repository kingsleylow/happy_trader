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
// File       : testexcept.hpp
// Package		: testframe
// Description: Header declaring simple exception class for test framework.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testexcept.hpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#ifndef TESTFRAME_TESTEXCEPT_INCLUDED
#define TESTFRAME_TESTEXCEPT_INCLUDED


#include <string>


// We are in namespace Testframe
namespace Testframe {


//=============================================================
// Class Exception
//=============================================================
/** Class Exception is an abstract base class for exceptions known
		to the test framework. Application being developed should derive
		their exceptions from this class. An exception holds a type
		and a message string.

		@author Olaf Lenzmann
*/
class Exception {

public:
	//
	// Functions
	//
	/** Pure virtual function to access the type of exception. */
	virtual std::string type (void) const = 0;

	/** Pure virtual function to access the exception message. */
	virtual std::string message (void) const = 0;

	/** Pure virtual function to return additionla info. */
	virtual std::string addInfo() const = 0;
};


// End of namespace
}



#endif // TESTFRAME_TESTEXCEPT_INCLUDED

