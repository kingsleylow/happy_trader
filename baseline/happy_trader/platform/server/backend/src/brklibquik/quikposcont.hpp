#ifndef _BRKLIBQUIK_QUIKPOSCONT_INCLUDED
#define _BRKLIBQUIK_QUIKPOSCONT_INCLUDED

#include "brklibquikdefs.hpp"
#include "quikddewrap.hpp"
#include "quikstruct.hpp"



namespace BrkLib {
	// this is helper class holding all raw DDE data arrived from QUIK
	// it is responsible for DDE connection with QUIK instance
	// it must be connected to QUIK all the time
	// do not confuse with broker sessions

class QuikBroker;
class QuikPosHolder : public CppUtils::DDEServerWrapListener
{
public:

	// ctor & dtor
	QuikPosHolder();

	~QuikPosHolder();

	// start processing, start DDE server
	void initialize(CppUtils::String const& ddeserviceName, QuikBroker* quikBroker);

	
	// stop processing
	void deinitialize();

	// callback from table class upon changes mad in QuikTable...
	template <class T>
	void onQuikTableChanged(QuikTable<T>& qtable, int const rbeg, int const cbeg, int const rend, int const cend);

private:
 
	//
	virtual void onDDEDataPoke(CppUtils::DDEItem const& item, CppUtils::String const& value);

	// XLTable for further processing
	virtual void onDDEXLTablePoke(CppUtils::DDEItem const& item, CppUtils::Byte const* p, size_t pDataLength);

	// server gives the data upon client request
	virtual void onDDEDataRequested(CppUtils::DDEItem const& item, CppUtils::String& value);

	// self - register
	virtual bool onUnknownDDEItem(CppUtils::DDEItem const& item);

	//
	// -------------------------------
	// 
	
	// 
	QuikTable<QuikDeals> tQuikDeals_Tab_m;

	QuikTable<QuikExecutedAndCancelledOrders> tQuikExecutedAndCancelledOrders_Tab_m;

	QuikTable<QuikActiveOrders> tQuikActiveOrders_Tab_m;	

	QuikTable<QuikStopOrders> tQuikStopOrders_Tab_m;	

	QuikTable<QuikCancelledAndExecutedStopOrders> tQuikCancelledAndExecutedStopOrders_Tab_m;	

	//QuikTable<QuikLukoilLevel2> tLukoilLevel2_Tab_m;


	// dde wrap 
	DDEQuikServerWrap* ddeWrap_m;

	QuikBroker* pBroker_m;



};

};

#endif