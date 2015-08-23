#ifndef _JAVA_DDE_SERVER_DDE_INCLUDED
#define _JAVA_DDE_SERVER_DDE_INCLUDED

#include "java_dde_serverdefs.hpp"
#include "HtDDEServer_C.h"


typedef jint (JNICALL *GetCreatedJavaVMs_t)( JavaVM**, jsize, jsize* );

// -------------------------------
// DDE helper class
class DDEJavaServerWrap: public CppUtils::DDEServerWrapListener {
public:
	DDEJavaServerWrap( CppUtils::String const& apps, CppUtils::DDEItemList const& itemlist, JavaVM* jvmptr);
	
	virtual ~DDEJavaServerWrap();

		
	void threadRun();

	// in DLL main function
	static void registerWindowClass(HINSTANCE hinstDll);

	
	inline JNIEnv* getJVMEnv()
	{
		return pEnv_m;
	}

	


	inline CppUtils::DDEServerWrap& getDDEServer()
	{
		return ddeServerInstance_m;
	}
			
private:

	//jint JNI_GetCreatedJavaVMs_Helper(CppUtils::String const& jvmDllPath);
	
	//
	virtual void onDDEDataPoke(CppUtils::DDEItem const& item, CppUtils::String const& value);

	// XLTable for further processing
	virtual void onDDEXLTablePoke(CppUtils::DDEItem const& item, CppUtils::Byte const* p, size_t pDataLength);

	// server gives the data upon client request
	virtual void onDDEDataRequested(CppUtils::DDEItem const& item, CppUtils::String& value);

	// self - register
	virtual bool onUnknownDDEItem(CppUtils::DDEItem const& item);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	// helpers
	

	//void switchDDEOn();

	//void switchDDEOff();

	bool stopThread();

	inline CppUtils::String const& getApps() const
	{
		return apps_m;
	}

	inline CppUtils::DDEItemList const& getItemList() const
	{
		return itemList_m;
	}

	
	
	void updateItemsForDDEServer(bool add);


	CppUtils::Thread* thread_m;

	// event showing that helper window is created
	CppUtils::Event wndCreated_m;

	//CppUtils::Event started_m;

	

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

	// status whether it is initialized (DDE is started)
  bool ddeStartedStatus_m;

	// the instance of DDE server
	CppUtils::DDEServerWrap ddeServerInstance_m;
	
	GetCreatedJavaVMs_t jvmDllFun_m;
};

#endif