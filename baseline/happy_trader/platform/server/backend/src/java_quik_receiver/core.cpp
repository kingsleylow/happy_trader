#include "core.hpp"


#define QUIK_RECEIVER "QUIK_RECEIVER"

// global
jobject g_jobj = NULL;
CppUtils::Thread* thread_g = NULL;
JNIEnv *pEnv_g = NULL;
DWORD threadId_g = NULL;



int bootThread (void *ptr)
	{
	
		threadId_g = ::GetCurrentThreadId();
		JavaVM *vmpp = (JavaVM *)ptr;

		
		try {  

				jint nStatus = vmpp->AttachCurrentThread(reinterpret_cast<void**>(&pEnv_g), NULL);
				if(nStatus != 0) { 
					THROW(CppUtils::OperationFailed, "exc_CannotAttachThreadJava", "bootThread()", "");
				}
			
				HWND hwnd;
				MSG iMsg;
				BOOL bRet;
				// create MSG queue
				::PeekMessage(&iMsg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

				// register class
				
				WNDCLASSEX  WMClass;
				WMClass.cbSize = sizeof(WNDCLASSEX); 
				WMClass.style = CS_HREDRAW | CS_VREDRAW;
				WMClass.lpfnWndProc = WndProc;
				
				

				if(FindWindow(TARGET_WINDOW_CLASS,NULL)!=NULL){
					THROW(CppUtils::OperationFailed, "exc_AlreadyRunning", "bootThread()", TARGET_WINDOW_CLASS);
				}
				else{
					WMClass.hInstance = GetModuleHandle(NULL);
					WMClass.lpszClassName = TARGET_WINDOW_CLASS;
					WMClass.cbClsExtra = 0;
					WMClass.cbWndExtra = 0;
					WMClass.hbrBackground = NULL;
					WMClass.hCursor = NULL;
					WMClass.hIcon = NULL;
					WMClass.lpszMenuName = NULL;
					WMClass.hIconSm		= 0;
					if(::RegisterClassEx(&WMClass) == 0){
						THROW(CppUtils::OperationFailed, "exc_CannotRegisterWindowClass", "bootThread()", CppUtils::getOSError() + " - " + TARGET_WINDOW_CLASS);
						
					}
					else {
						if((hwnd = ::CreateWindow((LPCTSTR)TARGET_WINDOW_CLASS, TARGET_WINDOW_CLASS, 0 , 0, 0, 0, 0, NULL, NULL, WMClass.hInstance, NULL)) == NULL)
						{
							THROW(CppUtils::OperationFailed, "exc_CannotCreateWindow", "bootThread()", CppUtils::getOSError() + " - " + TARGET_WINDOW_CLASS);
						}
						else {
							
							

							while( (bRet = GetMessage( &iMsg, NULL, 0, 0 )) != 0)
							{ 
									if (bRet == -1)
									{
											// handle the error and possibly exit
									}
									else
									{
											//TranslateMessage(&msg); 
										::DispatchMessage(&iMsg); 
									}
							} 
							// 


							// finished the queue

							::DestroyWindow(hwnd);
							::UnregisterClass(WMClass.lpszClassName, WMClass.hInstance );
						}		  		
					} 
				}



		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_ERROR, QUIK_RECEIVER, "Fatal exception: " << e.message());
			callJavaErrorCallback(e.message());
		}
		catch(...) {
			LOG(MSG_ERROR, QUIK_RECEIVER, "Unknown fatal exception");
			callJavaErrorCallback("Unknown exception");
		}

		

		return 42;

	};

// ---------------------------------------

extern "C" {


BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReasion, LPVOID lpReserved) {
	if(dwReasion == DLL_PROCESS_ATTACH) {
		
	}

	return TRUE;
}

// ----------------------
JNIEXPORT jboolean JNICALL Java_com_fin_httrader_utils_win32_HtQuikListener_init
  (JNIEnv *pEnv, jobject jobj)
{
	if (g_jobj == NULL) {
		g_jobj = pEnv->NewGlobalRef(jobj);

		JavaVM *vmpp = NULL;
		jint res = pEnv->GetJavaVM(&vmpp);
		if (res != 0)
				return false;

		// pass Java VM
		thread_g = new CppUtils::Thread(bootThread, vmpp );
	}
	return true;
}

/*
 * Class:     com_fin_httrader_utils_win32_HtQuikListener
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_HtQuikListener_destroy
  (JNIEnv *pEnv, jobject jobj)
{
	if(g_jobj) {
		
		//PostQuitMessage(0); 
			
		BOOL result = ::PostThreadMessage(threadId_g, WM_QUIT, 0, 0);
		
		delete thread_g;
		pEnv_g = NULL;
		threadId_g = NULL;

		pEnv->DeleteGlobalRef(g_jobj);
		g_jobj = NULL;
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
   
	LPWM_DATASTRUCTURE lpMsg;
  
  if(uMsg == WM_COPYDATA) {
    PCOPYDATASTRUCT pMyCDS = (PCOPYDATASTRUCT) lParam;
		lpMsg = (LPWM_DATASTRUCTURE)pMyCDS->lpData;
		

	 
	 switch(pMyCDS->dwData)
	 {
		case	0:
				// call here JAVA stuff
				callJavaCallback(lpMsg);
			break;
		
		default:
			;
	 };

	 return TRUE;
  }		
  else	if (uMsg == WM_QUIT )	{
		return -1;
	}
	else
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void callJavaErrorCallback(CppUtils::String const& error)
{
	if (pEnv_g==NULL)
		return;
	
	jclass cls = pEnv_g->GetObjectClass(g_jobj);
	jmethodID mid = pEnv_g->GetMethodID(cls, "onErrorArrived", "(Ljava/lang/String;)V");

	if (mid==NULL) 
		return;

	pEnv_g->CallVoidMethod(g_jobj, mid, pEnv_g->NewStringUTF(error.c_str()));

}

void callJavaCallback(LPWM_DATASTRUCTURE lpMsg)
{
	if (pEnv_g==NULL)
		return;

	//public void onQuikTickArrived(int, int, int, java.lang.String, java.lang.String,	double, double);
  //Signature: (IIILjava/lang/String;Ljava/lang/String;DD)V

	jclass cls = pEnv_g->GetObjectClass(g_jobj);
	jmethodID mid = pEnv_g->GetMethodID(cls, "onQuikTickArrived", "(IIILjava/lang/String;Ljava/lang/String;DD)V");

	if (mid==NULL) 
		return;

	pEnv_g->CallVoidMethod(g_jobj, mid, 
		lpMsg->hour_m, 
		lpMsg->minute_m, 
		lpMsg->second_m,
		pEnv_g->NewStringUTF(lpMsg->symbol_m), 
		pEnv_g->NewStringUTF(lpMsg->operation_m), 
		lpMsg->price_m,
		lpMsg->volume_m
	);
	
}

}; // end of namespace