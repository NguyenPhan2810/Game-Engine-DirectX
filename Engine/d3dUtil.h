#pragma once

#include <windows.h> // must be included before xnamath.n
#include <xnamath.h>
#include <D3DX11.h>
#include <cassert>
#include <sal.h>
#include <wchar.h>
#include <iostream>

//----------------------------------------------------------------------------
// Macro for safe releasing COM object
//----------------------------------------------------------------------------
#define ReleaseCOM(x)	\
{						\
	if (x)				\
	{					\
		x->Release();	\
		x = nullptr;	\
	}					\
}						\

//----------------------------------------------------------------------------
// HR macro
//----------------------------------------------------------------------------
// Error reminder and tracking in Debug mode
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)												\
{															\
	HRESULT hr = (x);										\
	if(FAILED(hr))											\
	{														\
		DXTraceW(__FILEW__, (DWORD)__LINE__, hr, L#x, true);\
	}														\
}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif 
#endif

// ------------------------------
// DXTraceW function
// ------------------------------
// Output formatting error message in debug output window, optional error window pops up (has finished)
// [In]strFile current file name, usually passing the macro __FILEW__
// [In]hlslFileName current line number, usually passing the macro __LINE__
// [In]hr function returns the HRESULT value returned when there is a problem
// [In]strMsg is used to help debug the positioned string, usually passing L#x (possibly NULL)
// [In]bPopMsgBox If TRUE, a message popup window pops up to inform the error message.
// Return value: Form hr
HRESULT WINAPI DXTraceW(_In_z_ const WCHAR* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ const WCHAR* strMsg, _In_ bool bPopMsgBox);


namespace Colors
{
	XMGLOBALCONST XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

	XMGLOBALCONST XMVECTORF32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST XMVECTORF32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}