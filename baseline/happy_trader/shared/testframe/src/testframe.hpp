// Copyright Virtual Photonics Inc., 2000. All rights reserved.
// Carnotstrasse 6, 10587 Berlin, Germany
//
// The copyright to the computer program(s) herein is the
// property of VPI. The program(s) may be used and/or copied
// only with the written permission of VPI or in accordanTestframe
// with the terms and conditions stipulated in the agreement/
// contract under which the program(s) have been supplied.
// This copyright notiTestframe must not be removed.
//
// File       : tfw.hpp
// Package		: testframe
// Description: Header pulling in all test framework definitions.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testframe.hpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#ifndef TESTFRAME_TSTFRWK_INCLUDED
#define TESTFRAME_TSTFRWK_INCLUDED


#include "testsysdep.hpp"

#include "testbase.hpp"
#include "testcase.hpp"
#include "testsuite.hpp"
#include "testmonitor.hpp"
#include "testexcept.hpp"

//=============================================================
// Global Variables
//=============================================================

// export the argc und argv command line arguments
extern int _argcd;
extern char** _argvd;

//=============================================================
// Macros
//=============================================================



// Application macro
//
#define CONSOLE_RUN(Suite) \
	int _argcd; \
	char** _argvd; \
	int main(int argc, char* argv[]) { \
		_argcd = argc; \
		_argvd = argv; \
		Testframe::init(); \
		Suite suite; \
		fprintf(stdout, "\nRunning %d tests\n", suite.numberOfTests() ); \
		suite.run(); \
		if( Testframe::TestMonitor::numberOfProblems() == 0 ) { \
			fprintf(stdout, "Success\n\n" ); \
			return 0; \
		} \
		\
		fprintf(stdout, "%d problems occurred\n", Testframe::TestMonitor::numberOfProblems() ); \
		char *rep = Testframe::TestMonitor::report(); \
		fprintf( stderr, "%s\n", rep ); \
		free( rep ); \
	}

#define CONSOLE_RUN_HTML(Suite) \
	int _argcd; \
	char** _argvd; \
	int main(int argc, char* argv[]) { \
		_argcd = argc; \
		_argvd = argv; \
		Testframe::init(); \
		Suite suite; \
		fprintf(stdout, "\nRunning %d tests\n", suite.numberOfTests() ); \
		suite.run(); \
		if( Testframe::TestMonitor::numberOfProblems() == 0 ) { \
			fprintf(stdout, "Success\n\n" ); \
			return 0; \
		} \
		\
		fprintf(stdout, "%d problems occurred\n", Testframe::TestMonitor::numberOfProblems() ); \
		char *rep = Testframe::TestMonitor::reportHTML(); \
		CppUtils::saveContentInFile(CppUtils::String(_argvd[0]) + ".html", rep != NULL ? rep : "" );	\
		free( rep ); \
	}

// Test suite declaration macros
//
#define TESTSUITE_BEGIN(Suite) class Suite : public Testframe::TestSuite { \
	public: \
		Suite() : Testframe::TestSuite( #Suite ) {

#define ADD_TEST(Test) addTest( new Test() );

#define TESTSUITE_END } };



// Test case declaration macros
//
#define TESTCASE_BEGIN(Test) class Test : public Testframe::TestCase { public:

#define TESTCASE_INDEX(Test) Test() : Testframe::TestCase(#Test)

#define REGISTER_TEST(Test,Fun) addTestFunction(#Fun, new Testframe::TestFunction<Test>( *this, Fun ) );

#define DEFINE_TEST(Fun) void Fun (void)

#define TESTCASE_SETUP void setup (void)

#define TESTCASE_WRAPUP void wrapup (void)

#define TESTCASE_END };



// Macro to use to test predicated within test functions.
// Turns malloc/free recording temporarily off to avoid
// inconsistent logs.
#define ASSERT(X) if( !(X) ) { \
	Testframe::TestMonitor::recordProblem( "Assertion failed", #X, __FILE__, __LINE__ ); \
}


// Macro to perform an operation that is expected to produce a given exception.
#define ASSERT_EXCEPTION(X,EXC) \
	{ \
		bool _success = false; \
		try { \
			X; \
		} \
		catch (EXC) { \
			_success = true; \
		} \
		if( !_success ) \
			Testframe::TestMonitor::recordProblem( "Expected exception not raised", #X, __FILE__, __LINE__ ); \
	}


#endif // TESTFRAME_TSTFRWK_INCLUDED

