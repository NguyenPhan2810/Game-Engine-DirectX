
#include "DemoShape.h"
#include "DemoWave.h"
#include "DemoShowRoom.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR szCmdLine, _In_ int iCmdShow) 
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	DemoShowRoom theApp(hInstance);

	if (!theApp.Init())
		return -1;

	return theApp.Run();
}