#ifndef _BRKLIBQUIK_QUIKDDEWRAP_INCLUDED
#define _BRKLIBQUIK_QUIKDDEWRAP_INCLUDED

#include "brklibquikdefs.hpp"

namespace BrkLib {

	// -------------------------------
// DDE helper class
class DDEQuikServerWrap {
public:
	DDEQuikServerWrap( CppUtils::DDEServerWrapListener& listener, CppUtils::String const& apps, CppUtils::DDEItemList const& itemlist );
	
	virtual ~DDEQuikServerWrap();

		
	void threadRun();

	// in DLL main function
	static void registerWindowClass(HINSTANCE hinstDll);
	

	inline CppUtils::DDEServerWrap& getDDEServer()
	{
		return ddeServerInstance_m;
	}
			
	inline CppUtils::DDEServerWrapListener& getListener()
	{
		return listener_m;
	}

private:

	

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	// helpers
	void setReadyToOperate();

	void switchDDEOn();

	void switchDDEOff();

	bool stopThread();

	inline CppUtils::String const& getApps() const
	{
		return apps_m;
	}

	inline CppUtils::DDEItemList const& getItemList() const
	{
		return itemList_m;
	}

	
	inline void setDDEConnectStatus(bool status)
	{
		ddeStartedStatus_m = status;
	}


	void updateItemsForDDEServer(bool add);

	CppUtils::Thread* thread_m;

	// event showing that helper window is created
	CppUtils::Event wndCreated_m;


	// application and item list
	CppUtils::String apps_m;

	CppUtils::DDEItemList itemList_m;

	HWND hWnd_m;

	static HINSTANCE hInstance_m;

	// DDE thread id
	DWORD threadId_m;

	// status whether it is initialized (DDE is started)
  bool ddeStartedStatus_m;

	// the instance of DDE server
	CppUtils::DDEServerWrap ddeServerInstance_m;

	// ref to listener
	CppUtils::DDEServerWrapListener& listener_m;
};


}; //end of namespace

#endif