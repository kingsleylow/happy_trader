
#include "dde.hpp"
#include "core.hpp"

#include <tchar.h>

#define DDEJAVAWRAP "DDEJAVAWRAP" 

//map<HWND, DDEJavaWrap*> instanceMap_g;

HINSTANCE DDEJavaWrap::hInstance_m = 0;

// ------------------------------
// messages 
#define UWM_SUBSCRIBE_DDE_MSG _T("UWM_SUBSCRIBE_DDE_MSG-{7FDB2CB4-5510-4d30-99A9-CD7752E0D680}")

#define UWM_UNSUBSCRIBE_DDE_MSG _T("UWM_UNSUBSCRIBE_DDE_MSG-{14A24D22-D6C7-4361-94A2-BBD08AE8A1B3}")


UINT UWM_SUBSCRIBE_DDE;

UINT UWM_UNSUBSCRIBE_DDE;

// -----------------------------

int bootThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif

		DDEJavaWrap* pThis = (DDEJavaWrap*)ptr; 

		try {
			pThis->threadRun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_ERROR, DDEJAVAWRAP, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_ERROR, DDEJAVAWRAP, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_ERROR, DDEJAVAWRAP, "Unknown fatal exception");
		}

		return 42;

	}


// -----------------------------
//helper class
	DDEJavaWrap::DDEJavaWrap( CppUtils::String const& apps, CppUtils::DDEItemList const& itemlist, JavaVM *vmpp ):
	threadId_m(0),
	pJvm_m(vmpp)
{
	apps_m = apps;
	itemList_m = itemlist;

	// initialize thread
	thread_m = new CppUtils::Thread(bootThread, this);

	LOG( MSG_DEBUG, DDEJAVAWRAP, "DDE thread created" );

	if (!wndCreated_m.lock(5000))
		THROW(CppUtils::OperationFailed, "exc_CannotFinishInitialization", "ctx_DDEJavaWrapCtr", "");

	ddeAdviseStatus_m = false;
	switchDDEOn();

	if (!ddeAdviseStatus_m) {
		
		// deinitialize
		if (!stopThread()) {
			LOG( MSG_ERROR, DDEJAVAWRAP, "Cannot stop thread" );
		}

		thread_m->waitForThread();

		// Delete thread
		delete thread_m; 
		thread_m = NULL;
		
		LOG( MSG_DEBUG, DDEJAVAWRAP, "DDE thread terminated, DDE processor destroyed" );

		THROW(CppUtils::OperationFailed, "exc_CannotConnectDDE", "ctx_DDEJavaWrapCtr", "");
	}
		
}

DDEJavaWrap::~DDEJavaWrap()
{
	if (!wndCreated_m.lock(5000))
		THROW(CppUtils::OperationFailed, "exc_CannotFinishInitialization", "ctx_DDEJavaWrapDtr", "");

	switchDDEOff();

	if (!stopThread()) {
		LOG( MSG_ERROR, DDEJAVAWRAP, "Cannot stop thread" );
	}

	thread_m->waitForThread();

	// Delete thread
	delete thread_m;
	thread_m = NULL;
	LOG( MSG_DEBUG, DDEJAVAWRAP, "DDE thread terminated, DDE processor destroyed" );
}

void DDEJavaWrap::onDataArrived(CppUtils::DDEItem const& item, CppUtils::String const& value)
{
	callback_onDDEDataArrived( item, value );
	//LOG(MSG_DEBUG, DDEJAVAWRAP, "DDE item " << item.item_m << " - " << item.topic_m << " returned value " << value ); 
} 

//-------------------------------

void DDEJavaWrap::threadRun()
{
	hWnd_m = 0;

	// force to create message queue
	MSG Msg;
	PeekMessage(&Msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	// get thread id
	threadId_m = ::GetCurrentThreadId();

	
	// cache the JNIEnv* pointer
	pEnv_m = NULL;

	
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

	
	LOG( MSG_DEBUG, DDEJAVAWRAP, "Entered message loop" );

	
  while(GetMessage(&Msg, 0, 0, 0)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg); 
  } 

		// destroy window
	BOOL dwin = ::DestroyWindow( hWnd_m );

	if (!dwin) {
		LOG( MSG_DEBUG, DDEJAVAWRAP, "Cannot destroy window" );
	}

	LOG( MSG_DEBUG, DDEJAVAWRAP, "Finished message loop" );


}

void DDEJavaWrap::setReadyToOperate()
{
	wndCreated_m.signalEvent();
}



void DDEJavaWrap::switchDDEOn()
{
	
	SendMessage(hWnd_m, UWM_SUBSCRIBE_DDE, 0, (LPARAM)this);
	
}

void DDEJavaWrap::switchDDEOff()
{
	
	SendMessage(hWnd_m, UWM_UNSUBSCRIBE_DDE, 0, (LPARAM)this);
}

bool DDEJavaWrap::stopThread()
{
		
	//SendMessage(hWnd_m, WM_QUIT, 0, (LPARAM)this);
	BOOL result = PostThreadMessage(threadId_m, WM_QUIT, 0, (LPARAM)this);

	return result;
}


// -------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		try {

			
			if(Msg == UWM_SUBSCRIBE_DDE) {
				

				
				DDEJavaWrap* pThis = (DDEJavaWrap*)lParam;
				pThis->initializeDDE(pThis->getApps(), pThis->getItemList());
				pThis->adviseItems();

				pThis->setDDEConnectStatus( true );

				LOG(MSG_INFO, DDEJAVAWRAP, "Performed actual initialization");
        
			}
			else if(Msg == UWM_UNSUBSCRIBE_DDE) {
				DDEJavaWrap* pThis = (DDEJavaWrap*)lParam;
				pThis->unadviseItems();
				pThis->uninitializeDDE();

				LOG(MSG_INFO, DDEJAVAWRAP, "Performed actual de-initialization");
        
			}
			else if (Msg== WM_CREATE) {
				// window created - we are ready to work
				CREATESTRUCT* struct_create = (CREATESTRUCT*)lParam;
				DDEJavaWrap* pThis = (DDEJavaWrap*)struct_create->lpCreateParams;
				pThis->setReadyToOperate();

				
				
			}
			else if (Msg == WM_DESTROY ) {
				DDEJavaWrap* pThis = (DDEJavaWrap*)lParam;
				pThis->unadviseItems();
				pThis->uninitializeDDE();

				LOG(MSG_INFO, DDEJAVAWRAP, "Performed actual de-initialization because of WM_DESTROY");
        
			}
			else 
				return DefWindowProc(hWnd, Msg, wParam, lParam);
			}
		catch(CppUtils::Exception& e) {
			LOG(MSG_ERROR, DDEJAVAWRAP, "Fatal exception in WndProc(): " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_ERROR, DDEJAVAWRAP, "Fatal MSVC exception in WndProc(): " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_ERROR, DDEJAVAWRAP, "Unknown fatal exception in WndProc() ");
		}


    return 0;
}

// ------------------------------------

void DDEJavaWrap::registerWindowClass(HINSTANCE hinstDll)
{
	hInstance_m = hinstDll;

	UWM_SUBSCRIBE_DDE = RegisterWindowMessage(UWM_SUBSCRIBE_DDE_MSG);
	UWM_UNSUBSCRIBE_DDE = RegisterWindowMessage(UWM_UNSUBSCRIBE_DDE_MSG);

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
