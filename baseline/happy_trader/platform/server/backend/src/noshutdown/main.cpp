
#include "main.hpp"
#define MAIN "MAIN"



// ---------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,
                         WPARAM wParam, LPARAM lParam)
{
  switch( uMsg )  {
	case WM_CREATE:
		LOG( MSG_INFO, MAIN, "No shutdown started");
		CppUtils::setThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);

		cout << "Starting..." << endl;

		return 0;
  case WM_DESTROY:
    // Tell the message loop to quit
    PostQuitMessage(0);
    return 0;
	case WM_QUERYENDSESSION:
	
		if (lParam & ENDSESSION_LOGOFF) {
			// do not allow log off
			LOG( MSG_INFO, MAIN, "Logging off, trying to reject...");
			cout << "Log off attempt" << endl;
		}
		else if (lParam==0) {
			LOG( MSG_INFO, MAIN, "Shutting down, trying to reject...");
			cout << "Shutdown attempt" << endl;
		}
		
		
		return FALSE;

	case WM_POWERBROADCAST:
		LOG( MSG_INFO, MAIN, "Suspending the system, trying to reject...");
		cout << "Suspend attempt" << endl;
		return BROADCAST_QUERY_DENY;
	}

  // Let the default window procedure handle 
  // any message that we don't care about
  return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


HRESULT Create(HINSTANCE hInst)
{
  // Register a window class
  WNDCLASS WndClass;
  
  WndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WndClass.lpfnWndProc   = (WNDPROC)WndProc; 
  WndClass.cbClsExtra    = 0;                
  WndClass.cbWndExtra    = 0;                
  WndClass.hInstance     = hInst; 
  WndClass.hIcon         = LoadIcon(NULL, IDI_WINLOGO); 
  WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW); 
  WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  WndClass.lpszMenuName  = NULL;             
  WndClass.lpszClassName = "ShutdownHookApp"; 

  if( !RegisterClass( &WndClass ) )
    return E_FAIL;


	// Create the render window
  HWND hWnd = CreateWindow(
    WndClass.lpszClassName,   // lpClassName
    "No shutdown",          // lpWindowName
    WS_OVERLAPPEDWINDOW,    // dwStyle
    CW_USEDEFAULT,          // x
    CW_USEDEFAULT,          // y
    200,                    // nWidth
    200,                    // nHeight
    NULL,                   // hWndParent
    NULL,                   // hMenu
    hInst,                  // hInstance
    NULL );                 // lpParam
                   
  if( hWnd == NULL )
    return E_FAIL;

  // Show the window
  //ShowWindow( hWnd, SW_SHOW );
  //UpdateWindow( hWnd );

  return S_OK;
}

int Run()
{
  // Check messages until we receive a WM_QUIT message
  MSG  Msg;
  while( GetMessage( &Msg, NULL, 0U, 0U ) )
  {
    // Dispatch the message to the window procedure
    DispatchMessage( &Msg );
  }

  return Msg.wParam;
}





// =============================================================
// Main
//
int main(int argc, char* argv[])
{
	if (S_OK==Create(GetModuleHandle(NULL)) ) {
		
		Run();
	}

	return 0;
}


