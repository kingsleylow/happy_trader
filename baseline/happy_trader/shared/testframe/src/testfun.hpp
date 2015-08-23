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
// File       : testfun.hpp
// Package		: testframe
// Description: Header declaring functor class to wrap test functions.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testfun.hpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#ifndef TESTFRAME_TESTFUN_INCLUDED
#define TESTFRAME_TESTFUN_INCLUDED


#include "testbase.hpp"


// We are in namespace Testframe
namespace Testframe {


//=============================================================
// Class VoidFunction
//=============================================================
/** Class VoidFunction is an abstract base class for functors
		wrapping a void(void) function into an object.

		@author Olaf Lenzmann
*/
class VoidFunction {

public:
	/** Pure virtual function operator. */
	virtual void operator () (void) = 0;
};




//=============================================================
// Class TestFunction
//=============================================================
/** Class TestFunction is a template class that wraps a member test
		function into a functor object. Is derived from VoidFunction.

		@author Olaf Lenzmann
*/
template <class Test> class TestFunction : public VoidFunction {

	public:
		/** Ctor taking a reference to the object that the function is a
				member of, as well as the pointer to the function itself. */
		TestFunction( Test &obj, void (Test::*fun) (void) ) : obj_m(obj), fun_m(fun) {};

		/** Function operator executing the function provided in the ctor. */
		void operator () (void) { (&obj_m->*fun_m)(); };

	private:
		/** Pointer to the function. */
		void (Test::*fun_m) (void);
		/** Reference to the object that the function is bound to. */
		Test &obj_m;
};


// End of namespace
}


#endif // TESTFRAME_TESTFUN_INCLUDED

