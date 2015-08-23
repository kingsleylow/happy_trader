
#include "ddeserver.hpp"

#include <tchar.h>
#include "core.hpp"

#define DDEJAVASERVERWRAP "DDEJAVASERVERWRAP" 

HINSTANCE DDEJavaServerWrap::hInstance_m = 0;

// ------------------------------
// messages 
//#define UWM_SUBSCRIBE_DDE_MSG _T("UWM_SUBSCRIBE_DDE_MSG-{EA8C1366-A837-4d25-858D-34C358B92F20}")

//#define UWM_UNSUBSCRIBE_DDE_MSG _T("UWM_UNSUBSCRIBE_DDE_MSG-{5934D98F-0A71-43d5-B494-58159764FE3C}")


//UINT UWM_SUBSCRIBE_DDE;

//UINT UWM_UNSUBSCRIBE_DDE;

// ------------------------------

int bootThread (void *ptr)
	{
		// handle run-time exceptions

		DDEJavaServerWrap* pThis = (DDEJavaServerWrap*)ptr; 

		try {  
			
			
			pThis->threadRun();  
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_ERROR, DDEJAVASERVERWRAP, "Fatal exception: " << e.message());
		}
		catch(...) {
			LOG(MSG_ERROR, DDEJAVASERVERWRAP, "Unknown fatal exception");
		}

		// uninitialize DDE if any
		try {
			pThis->getDDEServer().uninitializeServer();
		}
		catch(...) {
			// ignore
		}

		return 42;

	};


// -----------------------------
//helper class
	DDEJavaServerWrap::DDEJavaServerWrap( CppUtils::String const& apps, CppUtils::DDEItemList const& itemlist, JavaVM* jvmptr ):
	threadId_m(0),
	pJvm_m(jvmptr)
{
	apps_m = apps;
	itemList_m = itemlist;
	

	// initialize thread
	thread_m = new CppUtils::Thread(bootThread, this);

	LOG( MSG_DEBUG, DDEJAVASERVERWRAP, "DDE thread created" );

	

		
}

DDEJavaServerWrap::~DDEJavaServerWrap()
{
	
	if (!stopThread()) {
		LOG( MSG_ERROR, DDEJAVASERVERWRAP, "Cannot stop thread in destructor" );
	}

	
	thread_m->waitForThread();

	

	// Delete thread
	delete thread_m;
	thread_m = NULL;
	LOG( MSG_INFO, DDEJAVASERVERWRAP, "DDE thread terminated, DDE processor destroyed" );
}

void DDEJavaServerWrap::onDDEDataPoke(CppUtils::DDEItem const& item, CppUtils::String const& value) 
{
	callback_onDDEDataArrived(item, value );
}

// XLTable for further processing
void DDEJavaServerWrap::onDDEXLTablePoke(CppUtils::DDEItem const& item, CppUtils::Byte const* p, size_t pDataLength) 
{

	vector< vector <CppUtils::String> > out;
	try {
		CppUtils::crackXtableData(out, p, pDataLength);
	}
	catch(CppUtils::Exception& e)
	{
		LOG( MSG_ERROR, DDEJAVASERVERWRAP, "Exception on poke data: " << e.message() );
	}
	catch(...) 
	{
		LOG( MSG_ERROR, DDEJAVASERVERWRAP, "Unknown exception on poke data" );
	}

	callback_onDDEXLTableDataArrived(item, out);
}

// server gives the data upon client request
void DDEJavaServerWrap::onDDEDataRequested(CppUtils::DDEItem const& item, CppUtils::String& value) 
{
	
}

bool DDEJavaServerWrap::onUnknownDDEItem(CppUtils::DDEItem const& item) 
{
	return true;
}

//-------------------------------

void DDEJavaServerWrap::threadRun()
{
	hWnd_m = 0;

	// force to create message queue
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	// get thread id
	threadId_m = ::GetCurrentThreadId();

	
	pEnv_m = NULL;
	
	//jint nStatus = JNI_GetCreatedJavaVMs_Helper("jvm.dll");
	//if (nStatus != 0)
	//	THROW(CppUtils::OperationFailed, "exc_CannotCreateJVMJava", "ctx_threadRun", "");

	jint nStatus = pJvm_m->AttachCurrentThread(reinterpret_cast<void**>(&pEnv_m), NULL);
	if(nStatus != 0) { 
			THROW(CppUtils::OperationFailed, "exc_CannotAttachThreadJava", "ctx_threadRun", "");
	}
  

	

	// this is a thread for DDE

	hWnd_m = CreateWindow("dummy window", 
                      NULL, WS_OVERLAPPEDWINDOW, 
                      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                      CW_USEDEFAULT, NULL, NULL, hInstance_m,
											this  // create param
											);


	  

	if(hWnd_m == NULL) {
		THROW(CppUtils::OperationFailed, "exc_CannotCreateWindow", "ctx_threadRun", "");
  }
				
	getDDEServer().registerListener( *this );
	// BOOLSHIT
	getDDEServer().initServer(this->getApps());
	// add necessary topics
	updateItemsForDDEServer( true );
				   

	BOOL bRet;
	while( (bRet = ::GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
    if (bRet == -1)
    {
        // handle the error and possibly exit
    }
    else  {
			//::TranslateMessage(&msg); 
			::DispatchMessage(&msg); 
    }
	}

	// remove items
	updateItemsForDDEServer( false );

	// uninitialize
	getDDEServer().uninitializeServer();
	
		// destroy window
	BOOL dwin = ::DestroyWindow( hWnd_m );

	if (!dwin) {
		LOG( MSG_DEBUG, DDEJAVASERVERWRAP, "Cannot destroy window" );
	}

	LOG( MSG_DEBUG, DDEJAVASERVERWRAP, "Finished message loop" );


}





bool DDEJavaServerWrap::stopThread()
{
		
	//SendMessage(hWnd_m, WM_QUIT, 0, (LPARAM)this);
	BOOL result = PostThreadMessage(threadId_m, WM_QUIT, 0, (LPARAM)this);

	return result;
}


// -------------------------------------

LRESULT CALLBACK DDEJavaServerWrap::WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
   
	return ::DefWindowProc(hWnd, Msg, wParam, lParam);
}

// ------------------------------------

void DDEJavaServerWrap::registerWindowClass(HINSTANCE hinstDll)
{
	hInstance_m = hinstDll;


	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance_m;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= "dummy window";
	wcex.hIconSm		= 0;

	RegisterClassEx(&wcex);
}

// ----------------------------------

void DDEJavaServerWrap::updateItemsForDDEServer( bool add)
{
	for(int i = 0; i < itemList_m.size(); i++) {
		if (add)
			ddeServerInstance_m.addDDEItem(itemList_m[i]);
		else {
			try {
				ddeServerInstance_m.removeDDEItem(itemList_m[i]);
			}
			catch(CppUtils::OperationFailed& e)
			{
				// ignore
				//LOG(MSG_ERROR, DDEJAVASERVERWRAP, "Exception when removing DDE item: " << e.message());
			}
			catch(...)
			{
				// ignore
				//LOG(MSG_ERROR, DDEJAVASERVERWRAP, "Unknown exception when removing DDE item");
			}
		}
	}


	  
}

/*
jint DDEJavaServerWrap::JNI_GetCreatedJavaVMs_Helper(CppUtils::String const& jvmDllPath)
	{
		jint nSize = 1;
		jint nVms = 0;
		jint nStatus = 0;


		
		// try to resolve function address
		HINSTANCE hVM = ::LoadLibrary(jvmDllPath.c_str());
		if (hVM == NULL) 
				THROW(CppUtils::OperationFailed, "exc_CannotResolveJVMLib", "ctx_DDEJavaWrapCtr", jvmDllPath);
	  
		GetCreatedJavaVMs_t jvmDllFun = (GetCreatedJavaVMs_t)::GetProcAddress(hVM, "JNI_GetCreatedJavaVMs");
		if (jvmDllFun==NULL)
			THROW(CppUtils::OperationFailed, "exc_CannotResolveJVMLib_JNI_GetCreatedJavaVMs", "ctx_DDEJavaWrapCtr", "JNI_GetCreatedJavaVMs");
		

		//jint nStatus = JNI_GetCreatedJavaVMs(&pJvm_m, nSize, &nVms);
		nStatus = jvmDllFun(&pJvm_m, nSize, &nVms);

		return nStatus;


	}
*/
