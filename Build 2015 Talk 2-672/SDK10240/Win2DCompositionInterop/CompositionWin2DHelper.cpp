#include "pch.h"
#include "CompositionWin2DHelper.h"

using namespace Win2DCompositionInterop;
using namespace Platform;

ICompositionSurface^ Surface::CreateCompositionSurface(Compositor^ compositor, CanvasSwapChain^ swapChain)
{
	ComPtr<abi::ICompositorInterop> compositorInterop;
	__abi_ThrowIfFailed(reinterpret_cast<IInspectable*>(compositor)->QueryInterface(IID_PPV_ARGS(&compositorInterop)));

	auto dxgiSwapChain = GetWrappedResource<IDXGISwapChain>(swapChain);

	ComPtr<abi::ICompositionSurface> surface;

	__abi_ThrowIfFailed(compositorInterop->CreateCompositionSurfaceForSwapChain(
		dxgiSwapChain.Get(),
		&surface));

	return reinterpret_cast<ICompositionSurface^>(surface.Get());
}