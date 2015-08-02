//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once

#include <agile.h>
#include <memory>
#include <windows.h>
#include <fileapi.h>

#define NOMINMAX
#include <WindowsNumerics.h>
#include <Windows.UI.Composition.h>
#include <Windows.UI.Composition.Interop.h>

//
// D3D, D2D
//

#include <d2d1_1.h>
#include <d2d1helper.h>
#include <dxgi1_2.h>
#include <d3d11_1.h>
#include <DirectXMath.h>

//if we can make this onecore don't need..
#include <fileapi.h>

//Ditto?
typedef struct tagBITMAPFILEHEADER {
	WORD  bfType;
	DWORD bfSize;
	WORD  bfReserved1;
	WORD  bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
//
// Namespace aliases throughout project
//

//#include "App.xaml.h"

using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Graphics::Display;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Core;

//
// Error handling 
//

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		// Set a breakpoint on this line to catch Win32 API errors
		throw Platform::Exception::CreateException(hr);
	}
}
