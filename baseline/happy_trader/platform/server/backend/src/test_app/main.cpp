#include "shared/cpputils/src/cpputils.hpp"
#include "shared/testframe/src/testframe.hpp"
//#include "platform/server/backend/src/quik_connector/connector_api.h"


#include "test1.hpp"


// =============================================================
// This is the test application fro communication with MT4 pipe
//

// =======================================================
// Test Suite


TESTSUITE_BEGIN( QUIK_CONNECTOR )

ADD_TEST( test1 );
  
TESTSUITE_END


// =======================================================
// Main

CONSOLE_RUN( QUIK_CONNECTOR );

