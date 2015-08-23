#ifndef _JAVA_POWER_LISTENER_DDE_INCLUDED
#define _JAVA_POWER_LISTENER_DDE_INCLUDED

#include "java_power_listenerdefs.hpp"
#include "HtPowerEventDetector_C.h"


// -------------------------------
// DDE helper class
class DDEJavaWrap: public CppUtils::DDEWrap {
public:
	DDEJavaWrap( CppUtils::String const& apps, CppUtils::DDEItemList const& itemlist, JavaVM *vmpp );
	virtual ~DDEJavaWrap();

	virtual void onDataArrived(CppUtils::DDEItem const& item, CppUtils::String const& value);
	
	void threadRun();

	// in DLL main function
	static void registerWindowClass(HINSTANCE hinstDll);

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

	

	inline JNIEnv* getJVMEnv()
	{
		return pEnv_m;
	}

	inline void setDDEConnectStatus(bool status)
	{
		ddeAdviseStatus_m = status;
	}

private:

	CppUtils::Thread* thread_m;

	// event showing that helper window is created
	CppUtils::Event wndCreated_m;


	// application and item list
	CppUtils::String apps_m;

	CppUtils::DDEItemList itemList_m;

	HWND hWnd_m;

	static HINSTANCE hInstance_m;

	// java
	// cache the JavaVM* pointer
	JavaVM* pJvm_m;

	// cache the JNIEnv* pointer
	JNIEnv* pEnv_m;

	// current thread id
	DWORD threadId_m;

  bool ddeAdviseStatus_m;
};

#endif