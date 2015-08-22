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
// File       : testmonitor.cpp
// Package		: testframe
// Description: Implements class for monitoring tests.
//
// Responsible: Olaf Lenzmann
// Authors    :
//
// $Id: testmonitor.cpp,v 1.1.1.1 2001/11/05 15:49:29 lenzmann Exp $
//


#include "testmonitor.hpp"
#include <iostream>
#include <time.h>
#include <stdio.h>	/* pick up sprintf() */

// For debug outputs
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define DEBUG_OUTPUT( msg ) OutputDebugString( msg );
#else
#define DEBUG_OUTPUT( msg )
#endif



// We are in namespace Testframe
namespace Testframe {


//=============================================================
// Class TestMonitor
//=============================================================
TestMonitor::TestMonitor () :
	sectionList_m(),
	log_m()
{
	// Enter the root section
	enterTestSection( "/" );
}
//-------------------------------------------------------------
TestMonitor::~TestMonitor ()
{
	// Empty log
	while( !log_m.empty() ) {
		free( log_m.begin()->type );
		free( log_m.begin()->msg );
		free( log_m.begin()->line );
		free( log_m.begin()->file );
		free( log_m.begin()->section );

		log_m.pop_front();
	}

	// Empty section list
	while( !sectionList_m.empty() )
		leaveTestSection();
}
//-------------------------------------------------------------
void TestMonitor::enterTestSection (const char *sectionName, bool allocationContext)
{
	// Construct context
	Context context;
	context.name_m = strdup( sectionName ? sectionName : "<unnamed>" );
	context.allocationContext_m = allocationContext;
	context.startTime_m = getDoubleTime();

	// Record context
	staticInst().sectionList_m.push_back( context );

	// Console message
	cout << "Entering test section \"" << sectionName << "\"..." << endl;
}
//-------------------------------------------------------------
void TestMonitor::leaveTestSection (void)
{
	char buf2[256];

	// Check for memory leaks; there must be no remaining
	// entries in the context's allocation map.
	Context &context = staticInst().sectionList_m.back();
	for( map<void *, Allocation>::const_iterator mem = context.allocated_m.begin();
		mem != context.allocated_m.end(); ++mem ) {
		// Record problem
		sprintf( buf2, "%d bytes allocated in \"%s\" not deallocated @ %p when leaving section",
			mem->second.size_m, mem->second.section_m, mem->first );
		staticInst().recordProblem( "Memory issue", buf2, "n/a", -1 );

		// Free memory of allocation record
		free(mem->second.section_m);
	}

	// Compute time it took
	const double now = getDoubleTime();

	// Console message
	char buf[80];
	const double timediff = now - context.startTime_m;
	sprintf( buf, "%.3f", timediff );
	cout << "Leaving test section \"" << context.name_m << "\"; completion took " << buf << " seconds" << endl;

	// Free context memory
	free( staticInst().sectionList_m.back().name_m );
	staticInst().sectionList_m.pop_back();
}
//-------------------------------------------------------------
void TestMonitor::recordAllocation (void *ptr, size_t size)
{
	if ( staticInst().allocationRecording_m ) {
		// Get allocation context
		list<Context>::iterator context = findAllocationContect();

		// Allocation recording
		Allocation alloc;
		alloc.size_m = size;
		alloc.section_m = staticInst().constructSectionName();

		context->allocated_m[ ptr ] = alloc;
	}
}
//-------------------------------------------------------------
void TestMonitor::recordDeallocation (void *ptr)
{

	if ( staticInst().allocationRecording_m ) {
		// Get allocation context
		list<Context>::iterator context = staticInst().findAllocationContect();

		map<void *, Allocation>::const_iterator mem = context->allocated_m.find( ptr );

		// Check for bogus free calls
		if( mem == context->allocated_m.end() ) {
			char buf2[256];
			// This memory was never allocated
			sprintf( buf2, "Deallocating unallocated memory @ %p", ptr );
			recordProblem("Memory issue ", buf2, "n/a", -1);
			return;
		}

		// Free & remove allocation entry
		free(mem->second.section_m);
		context->allocated_m.erase( ptr );
	}
}
//-------------------------------------------------------------
void TestMonitor::recordProblem (const char *type, const char *msg, char const *file, int line)
{
	// Do not record mallocs & frees now
	bool oldState = staticInst().allocationRecording_m;
	staticInst().allocationRecording_m = false;

	// Generate the problem entry
	Problem problem;

	char buf[16];
	sprintf( buf, "%d", line );
	problem.type = strdup(type ? type : "n/a" );
	problem.msg = strdup(msg ? msg : "n/a" );;
	problem.line = strdup(line < 0 ? "n/a" : buf);
	problem.file = strdup(file ? file : "n/a" );
	problem.section = staticInst().constructSectionName();

	staticInst().log_m.push_back( problem );

	// Dump
	static char buffer[512];
	sprintf( buffer, "%s(%s): \"%s\": %s\n", problem.file, problem.line, problem.type, problem.msg );
	DEBUG_OUTPUT( buffer );

	// Revert to previous malloc recording
	staticInst().allocationRecording_m = oldState;
}
//-------------------------------------------------------------
int TestMonitor::numberOfProblems (void)
{
	return staticInst().log_m.size();
}
//-------------------------------------------------------------
char *TestMonitor::report (void)
{
	// Return an empty string if there were no problems
	if( staticInst().log_m.empty() ) {
		return NULL;
	}

	char buf[1024];
	char *rep = (char *) malloc( sizeof(char) * 250 * (staticInst().log_m.size() + 2) );

	// Create header
	sprintf( rep, "%-20s | %-40s | %-10s | %-30s | %-50s\n", "Problem type", "Message", "Line", "File", "Test section" );

	// List all problems
	for(list<Problem>::const_iterator problem = staticInst().log_m.begin();
		problem != staticInst().log_m.end(); ++problem ) {
		// Format output
		sprintf( buf, "%-20s | %-40s | %-10s | %-30s | %-50s\n", problem->type, problem->msg,
			problem->line, problem->file, problem->section );

		// Append to report
		strcat(rep, buf);
	}

	// Done
	return rep;
}
//-------------------------------------------------------------
char *TestMonitor::reportHTML (void)
{
	// Return an empty string if there were no problems
	if( staticInst().log_m.empty() ) {
		return NULL;
	}


	// Get time
	time_t now = time(0);
	char * timeline = ctime(& now);

	char buf[1024];
	sprintf( buf, "<center><b>Report of test finished at %.19s</b></center>\n<br>", timeline );

	// Create header
	char *rep = (char *) malloc( sizeof(char) * 1024 * (staticInst().log_m.size() + 2) );
	sprintf( rep, "	<!doctype html public \"-//w3c//dtd html 4.0 transitional//en\">\n<html>\n<head>\n" );
	strcat( rep, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n<title>Report</title>\n</head>\n<body>" );
	strcat( rep, buf );
	strcat( rep, "<table BORDER WIDTH=\"100%\" >\n<tr><td><b>Problem</b></td>\n\n<td><b>Information</b></td>\n\n<td><b>Line</b></td>\n\n");
	strcat( rep, "<td><b>File</b></td>\n\n<td><b>Test section</b></td>\n</tr>\n\n");


	// List all problems
	for(list<Problem>::const_iterator problem = staticInst().log_m.begin();
		problem != staticInst().log_m.end(); ++problem ) {
		// Format output
		sprintf( buf, "<tr>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n</tr>\n\n", problem->type, problem->msg,
			problem->line, problem->file, problem->section );

		// Append to report
		strcat(rep, buf);
	}

	// Remains
	strcat( rep, "</table></body></html>\n" );

	// Done
	return rep;
}
//-------------------------------------------------------------
char *TestMonitor::constructSectionName (void) const
{
	// Calculate length of string
	int len = 0;
	list<Context>::const_iterator section;
	for(section = sectionList_m.begin();
		section != sectionList_m.end(); ++section )
		len += strlen( section->name_m );

	// Allocate string
	char *sectionName = (char *) malloc( sizeof(char) * (len + sectionList_m.size()) );
	*sectionName = 0;

	// Compose string
	for(section = sectionList_m.begin();
		section != sectionList_m.end(); ++section ) {
		// Prepend delimiter if required
		if( strlen(sectionName) )
			strcat(sectionName, "/");

		// Append section
		strcat(sectionName, section->name_m );
	}

	// Done
	return sectionName;
}
//-------------------------------------------------------------
TestMonitor & TestMonitor::staticInst (void)
{
	static TestMonitor inst;
	return inst;
}

//-------------------------------------------------------------
list<TestMonitor::Context>::iterator TestMonitor::findAllocationContect(void)
{
	// Get correct context; this is the last context
	// that is an allocation context.
	list<Context>::iterator context = staticInst().sectionList_m.end();
	do {
		--context;
	}	while( !context->allocationContext_m );

	return context;
}

void TestMonitor::setAllocationRecording(bool newState) {
	staticInst().allocationRecording_m = newState;
}

// End of namespace
}
