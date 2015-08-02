#pragma once

#include <wrl.h>
#include <windows.ui.composition.interop.h>
#include <Microsoft.Graphics.Canvas.native.h>
#include <dxgi.h>

using namespace Windows::UI::Composition;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL;

namespace abi
{
	using namespace ABI::Windows::UI::Composition;
}

namespace Win2DCompositionInterop
{
	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class Surface sealed
	{
	public:
		static ICompositionSurface^ CreateCompositionSurface(Compositor^ compositor, CanvasSwapChain^ swapChain);
	};
}
