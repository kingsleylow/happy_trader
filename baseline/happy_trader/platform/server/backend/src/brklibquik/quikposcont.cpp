#include "quikposcont.hpp"
#include "quikbroker.hpp"
#include "quikstruct.hpp"

#define QUIKPOSHOLDER "QUIKPOSHOLDER"

namespace BrkLib {
	QuikPosHolder::QuikPosHolder():
		ddeWrap_m(NULL),
		pBroker_m(NULL)
{
}

QuikPosHolder::~QuikPosHolder()
{
}

	// ---------------------------

void QuikPosHolder::onDDEDataPoke(CppUtils::DDEItem const& item, CppUtils::String const& value)
{
		// todo
}

// XLTable for further processing
void QuikPosHolder::onDDEXLTablePoke(CppUtils::DDEItem const& item, CppUtils::Byte const* p, size_t pDataLength)
{

	vector< vector <CppUtils::String> > out;

	try {
		CppUtils::crackXtableData(out, p, pDataLength);

		// call this functions for all registered QuikTable...
		tQuikDeals_Tab_m.onNewDataArrivedExt(item, out);
		tQuikExecutedAndCancelledOrders_Tab_m.onNewDataArrivedExt(item, out);
		tQuikActiveOrders_Tab_m.onNewDataArrivedExt(item, out);
		tQuikStopOrders_Tab_m.onNewDataArrivedExt(item, out);
		tQuikCancelledAndExecutedStopOrders_Tab_m.onNewDataArrivedExt(item, out);

		//tLukoilLevel2_Tab_m.onNewDataArrivedExt(item, out );
		
	}
	catch(CppUtils::Exception& e)
	{
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, QUIKPOSHOLDER, "Cannot process XTABLE data",  e.message() << e.arg() << e.context() << e.addInfo() );

	}
	catch(...)
	{
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, QUIKPOSHOLDER, "Cannot process XTABLE data for unknown reason");

	}
	// todo
	
}

// server gives the data upon client request
void QuikPosHolder::onDDEDataRequested(CppUtils::DDEItem const& item, CppUtils::String& value)
{
		// N/A
}

bool QuikPosHolder::onUnknownDDEItem(CppUtils::DDEItem const& item)
{
	// self-register
	return true;
}

// -----------------------

void QuikPosHolder::initialize(CppUtils::String const& ddeserviceName, QuikBroker* quikBroker)
{
	if (!ddeWrap_m) {
		pBroker_m = quikBroker;
		CppUtils::DDEItemList itemlist;
		
		// subscribe for that items
		tQuikDeals_Tab_m.initialize(*this, itemlist );
		tQuikExecutedAndCancelledOrders_Tab_m.initialize(*this, itemlist);
		tQuikActiveOrders_Tab_m.initialize(*this, itemlist);
		tQuikStopOrders_Tab_m.initialize(*this,itemlist);
		tQuikCancelledAndExecutedStopOrders_Tab_m.initialize(*this, itemlist);

		//tLukoilLevel2_Tab_m.initialize(*this, itemlist );
			

		// create item list - these 
		ddeWrap_m = new DDEQuikServerWrap(*this, ddeserviceName, itemlist );

		// now we are waiting until QUIK user will establish DDE propagation
	}

}



void QuikPosHolder::deinitialize()
{
	if (ddeWrap_m) {
		
		tQuikDeals_Tab_m.deinitialize();
		tQuikExecutedAndCancelledOrders_Tab_m.deinitialize();
		tQuikActiveOrders_Tab_m.deinitialize();
		tQuikStopOrders_Tab_m.deinitialize();
		tQuikCancelledAndExecutedStopOrders_Tab_m.deinitialize();

		//tLukoilLevel2_Tab_m.deinitialize();
		

		delete ddeWrap_m;
		pBroker_m = NULL;
		ddeWrap_m = NULL;
	}
}

template <class T>
void QuikPosHolder::onQuikTableChanged(QuikTable<T>& qtable, int const rbeg, int const cbeg, int const rend, int const cend)
{
	
	RespGenerator<T> respGen;


	if (pBroker_m) {
		pBroker_m->delegateResponseToSessions( respGen(qtable.getDataVector(), rbeg, cbeg, rend, cend) );
	}
	
};

}; // end of namespace