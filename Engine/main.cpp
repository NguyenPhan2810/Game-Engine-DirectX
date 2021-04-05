#include "d3dApp.h"

#include <iostream>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR szCmdLine, _In_ int iCmdShow) 
{
	D3DApp theApp(hInstance);

	if (!theApp.Init())
		return -1;

	return 0;
}