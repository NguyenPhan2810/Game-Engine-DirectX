#include "NPGameEngine.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR szCmdLine, _In_ int iCmdShow) 
{
	NPGameEngine gameEngine(hInstance);

	if (!gameEngine.Init())
		return -1;

	return gameEngine.Run();
}