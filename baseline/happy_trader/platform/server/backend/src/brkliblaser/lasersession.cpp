#include "lasersession.hpp"
#include "laserbroker.hpp"

namespace BrkLib {


  
	LaserSession::LaserSession(CppUtils::String const& connectstring, LaserBroker &broker, WindowThreadProc& winthreadproc):
	BrokerSession(connectstring, broker),
	winThreadProc_m(winthreadproc)
	{
				
	}

	LaserSession::~LaserSession()
	{
		
	}
		


} // end of namespace