//
// pch.h
// Header for standard system include files.
//

#pragma once

/*
#undef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP

#include <inspectable.h>
#include <dcomp.h>
#include <dcompp.h>

#undef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_APP
*/

#include <collection.h>
#include <ppltasks.h>
#include <agile.h>
#include <WindowsNumerics.h>

#include <wrl\implements.h>
#include <wrl\module.h>
#include <wrl\event.h>

/*
#include <CoreWindow.h>
#include <Windows.ApplicationModel.h>
#include <Windows.ApplicationModel.core.h>
*/

using namespace Platform;

using namespace Microsoft::UI::Composition::Effects;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;


using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Core;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#define DO(x) { HRESULT hrTemp = (x); if (FAILED(hrTemp)) { __debugbreak(); } }

#include "Grid.h"