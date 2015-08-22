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
// File       : testmonitor.hpp
// Package		: testframe
// Description: Header declaring class for monitoring tests.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testmonitor.hpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#ifndef TEST_TSTFRWK_MONITOR_INCLUDED
#define TEST_TSTFRWK_MONITOR_INCLUDED


#include "testbase.hpp"
#include "testsysdep.hpp"
#include <list>



// We are in namespace Testframe
namespace Testframe {


//=============================================================
// Class TestMonitor
//=============================================================
/** Class TestMonitor serves to record a log of problems
		encountered during execution of tests. This class provides
		a static, global instance.

		@author Olaf Lenzmann
*/
class TESTFRAME_EXP  TestMonitor {

public:

	//
	// Ctors & Dtor
	//
	/** Ctor. */
	TestMonitor ();
	/** Dtor. */
	~TestMonitor ();

	//
	// Functions
	//
	/** Notifies the monitor that a new, named test section is
			entered. For each section entered, leaveTestSection()
			must be called. Section can be either an own allocation
			context - as indicated by the 2nd argument - or they
			can share the allocation context of the containing section.
			All malloc/new and matching free actions must be matched
			within the same allocation context. Default is that each
			section is an own allocation context.
	*/
	static void enterTestSection (const char *sectionName, bool allocationContext = true);
	/** Notifies the monitor that a test section has been left. */
	static void leaveTestSection (void);
	/** Record a failure described by a string with the type of problem,
			msg, line and file that it occurred at in the log. */
	static void recordProblem (const char *msg, const char *type, const char *file, int line);
	/** Returns the number of problems that occurred so far. */
	static int numberOfProblems (void);
	/** Record the allocation of heap memory with the monitor. */
	static void recordAllocation (void *ptr, size_t size);
	/** Record the deallocation of heap memory with the monitor. */
	static void recordDeallocation (void *ptr);

	/** method to switch on and off allocations reporting through the recordAllocation
		methods
		@param the new state of the record allocation switch
	*/
	static void setAllocationRecording(bool newState);

	/** Write a report and return as string. The string must be freed
			by the caller using free(). */
	static char *report (void);
	/** Write a report in HTML and return as string. The string must be freed
			by the caller using free(). */
	static char *reportHTML (void);



private:
	//
	// Typedef
	//
	/** Helper structure recording a reported problem. This does not
			provide a proper copy ctor, nor a dtor! */
	struct Problem {
		char *type;
		char *msg;
		char *line;
		char *file;
		char *section;
	};
	/** Helper structure to record allocations. */
	struct Allocation {
		size_t size_m;
		char *section_m;
	};
	/** Helper structure for internal bookkeeping. */
	struct Context {
		char *name_m;
		bool allocationContext_m;
		double startTime_m;
		map<void *, Allocation> allocated_m;
	};

	//
	// Functions
	//
	/** Helper to concatenate the list of test section names to
			a single string with slashes as delimiter. The caller
			must free the resulting string using free(). */
	char *TestMonitor::constructSectionName (void) const;

	static list<Context>::iterator findAllocationContect(void);

	/** Returns a reference to a static, global instance of this class. */
	static TestMonitor & staticInst (void);
	//
	// Variables
	//
	/** List holding the test section contexts. */
	list<Context> sectionList_m;
	/** Log tracking all reported problems. */
	list<Problem> log_m;
	/** member variable storing whether allocations reported through the recordAllocation
		methods should actually be counted */
	bool allocationRecording_m;


};



// End of namespace
}


#endif // TEST_TSTFRWK_MONITOR_INCLUDED

