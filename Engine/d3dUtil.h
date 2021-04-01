#pragma once

#include <windows.h> // for FLOAT definition in xnamath.h
#include <xnamath.h>
#include <D3DX11.h>
#include <DxErr.h>
#include <cassert>

//-------------------------------------------------------------------------------------- -
// Simple d3d error checker for book demos.
//---------------------------------------------------------------------------------------

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)												   \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif