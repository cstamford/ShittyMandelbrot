#include "MandelbrotViewer.h"
#include "windows.h"
#include <thread>

// Handle to the window
HWND gHwnd;

// Our MandelbrotViewer class
MandelbrotViewer* pMandelbrotViewer = nullptr;

// Prototypes
void logicThreadHandler();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void registerWindow(HINSTANCE hInstance);
bool initWindow(HINSTANCE hInstance, int nCmdShow);


// Entry point for the application.
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int nCmdShow)			
{	
	// Register the window
	registerWindow(hInstance);

	// Initialize the window, and if it fails, return
	if (!initWindow(hInstance, nCmdShow))
		return 0;

	// Create a new MandelbrotViewer, passing it the handle to the window,
	// and call the init function.
	pMandelbrotViewer = new MandelbrotViewer(gHwnd);
	pMandelbrotViewer->init();

	std::thread logicThread(logicThreadHandler);

	MSG msg;

	// The basic MandelbrotViewer loop. Calls run() repeatedly, unless a message
	// need be dealt with.
	while (true)					
	{				
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				pMandelbrotViewer->setQuitting(true);
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (pMandelbrotViewer->isQuitting())
		{
			logicThread.join();
			break;
		}
	}

	// Delete MandelbrotViewer, freeing up all resources.
	delete pMandelbrotViewer;
	pMandelbrotViewer = nullptr;

	return msg.wParam;										
}

void logicThreadHandler()
{
	while (!pMandelbrotViewer->isQuitting())
		pMandelbrotViewer->run();
}


// Collects windows events and forwards them to the MandelbrotViewer.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (pMandelbrotViewer != nullptr)
		pMandelbrotViewer->onWinEvent(hwnd, message, wParam, lParam);

	return DefWindowProc(hwnd, message, wParam, lParam);																
}


// Register the window.
void registerWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize        = sizeof(wcex);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = 0;
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = "MandelbrotViewerWindowMain";
	wcex.hIconSm       = 0;

	RegisterClassEx(&wcex);
}


// Initialize the window.
bool initWindow(HINSTANCE hInstance, int nCmdShow)
{
	HWND hwnd;

	hwnd = CreateWindow("MandelbrotViewerWindowMain",
						"MandelbrotViewer",
						WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						MandelbrotViewer::SCREEN_WIDTH,
						MandelbrotViewer::SCREEN_HEIGHT,
						NULL,
						NULL,
						hInstance,
						NULL);

	if (!hwnd)
		return false;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	gHwnd = hwnd;

	return true;
}