#ifndef _CPPUTILS_DDEWRAP_INCLUDED
#define _CPPUTILS_DDEWRAP_INCLUDED


#include "../../cpputilsdefs.hpp"
#include <ddeml.h>
#include "mfthunk.hpp"
#include "../../stdthunk.hpp"


#define INSTANCE_THIS 0x5A1DAED4


namespace CppUtils {

	// helper class providing functionality to DDE communications
	// connects to DDE server on constructor, outomatically disconnecst in destructor
	// provides callback for constant messages update
	struct CPPUTILS_EXP DDEItem {
		DDEItem() 
		{
		}

		DDEItem(CppUtils::String const& topic, CppUtils::String const& item):
		topic_m(topic),
		item_m(item)
		{
		}

		DDEItem(DDEItem const& src):
		topic_m(src.topic_m),
		item_m(src.item_m)
		{
		}

	
		String topic_m;
		String item_m;

	};
  
	typedef vector<DDEItem> DDEItemList;

	typedef struct {    // used to passinfo to/from TransactionDlgProc and
    DWORD ret;				// TextEntryDlgProc.
    DWORD Result;
    DWORD ulTimeout;
    DWORD wType;
    HCONV hConv;
    HDDEDATA hDdeData;
    DWORD wFmt;
    HSZ hszItem;
    DWORD fsOptions;
		char* item_m;
	} XACT;


  //----------------------------
	

	
	
	//
	class CPPUTILS_EXP DDEWrap {
	
	public:

		
		DDEWrap();

		void initializeDDE( String const& apps, DDEItemList const& itemlist, int toutmsec  = 1000 );
		
		virtual ~DDEWrap();
		
		virtual void onDataArrived(DDEItem const& item, CppUtils::String const& value);

		void adviseItems();

		void unadviseItems();

		void uninitializeDDE();

		// member function
		HDDEDATA UdprocDdeCallback(
			UINT wType, 
			UINT wFmt, 
			HCONV hConv, 
			HSZ hsz1,
			HSZ hsz2, 
			HDDEDATA hData, 
			DWORD lData1, 
			DWORD lData2
	 );
    
	private:

		
		// DDE related data
		CONVCONTEXT ccFilter_m;

		DWORD idInst_m;

		vector<XACT*> pxact_m;
	
		//stdcall_proxy callProxy_m;
 
		CppUtils::CallBackAdapter<INSTANCE_THIS, DDEWrap, HDDEDATA,	UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD>* callProxy_m;

	};

	// --------------------------





	
}; // end of namespace




#endif