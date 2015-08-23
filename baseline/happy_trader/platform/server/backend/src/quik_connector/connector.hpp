#ifndef _QUIK_CONNECTOR_CONNECTOR_INCLUDED
#define _QUIK_CONNECTOR_CONNECTOR_INCLUDED

#include "quik_connectordefs.hpp"
#include "quikddewrap.hpp"
#include "wraptrans2quik.hpp"


namespace QuikConnector {

struct ConnectorErrorInstance
{
	
	ConnectorErrorInstance();

	void setNoError();

	void init(QuikApiException& e);

	void init(CppUtils::Exception& e);

	void init(CppUtils::String const& message);

	// ------------------------------

	CppUtils::String type_m;

	CppUtils::String message_m;

	CppUtils::String context_m;

	CppUtils::String arg_m;

	CppUtils::String addInfo_m;

	// quik specific

	int quikCode_m;

	int quikExtCode_m;

	CppUtils::String quikError_m;

};

// --------------------------------

class ConnectorInstance: 
		public CppUtils::Mutex, 
		public QuikApiWrap, 
		public CppUtils::DDEServerWrapListener
{
public:
	
	// ctor & dtor
	ConnectorInstance();

	~ConnectorInstance();

	// create and release error structures
	void* newErrorInstance();

	void releaseErrorInstance(void* errP);

	// clear all error structures
	void clearAll();

	// starts DDE server
	void initialize(
		CppUtils::String const& ddeserviceName, 
		CppUtils::DDEItemList const& itemList, 
		PQCFUN callback,
		PQCCONNECTSTATUSFUN callbackConnectFun,
		PQCTRANSSTATUSFUN callbackTransactFun
	);

	// stop DDE server
	void deinitialize();

	// helper to convert to C++ item list
	CppUtils::DDEItemList convertToItemList(PDDEITEM ddeInitialItemListP);

private:


	virtual void onDDEDataPoke(CppUtils::DDEItem const& item, CppUtils::String const& value);

	virtual void onDDEXLTablePoke(CppUtils::DDEItem const& item, CppUtils::Byte const* p, size_t pDataLength);

	virtual void onDDEDataRequested(CppUtils::DDEItem const& item, CppUtils::String& value);

	virtual bool onUnknownDDEItem(CppUtils::DDEItem const& item);

	// -----------------------

	DDEQuikServerWrap* ddeWrap_m;

	set<ConnectorErrorInstance*> errorInstances_m;

	PQCFUN callback_m;

	

};



}; // end of namespace

#endif