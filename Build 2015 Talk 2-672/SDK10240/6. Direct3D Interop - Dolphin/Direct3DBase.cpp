//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#include "pch.h"
#include "Direct3DBase.h"
#include <corewindow.h>


Direct3DBase::~Direct3DBase()
{
}


void 
Direct3DBase::Initialize(
    Windows::Foundation::Size & destinationSize)
{
    _destinationSize = destinationSize;

    CreateDeviceResources();
    CreateSizeDependentResources();
}


void 
Direct3DBase::HandleDeviceLost()
{
    _swapChain = nullptr;
    CreateDeviceResources();

    ID3D11RenderTargetView* nullViews[] = { nullptr };
    _d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    _renderTargetView = nullptr;
    _depthStencilView = nullptr;
    _d3dContext->Flush();
    CreateSizeDependentResources();
}


void 
Direct3DBase::CreateDeviceResources()
{
    //
    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D
    //

    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    //
    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Don't forget to declare your application's minimum required feature level in its
    // description.  All applications are assumed to support 9.1 unless otherwise stated
    //

    D3D_FEATURE_LEVEL featureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    //
    // Create the Direct3D 11 API device object and a corresponding context
    //

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ThrowIfFailed(D3D11CreateDevice(
        nullptr,            // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,      // Set set debug and Direct2D compatibility flags.
        featureLevels,      // List of feature levels this app can support.
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,  // Always set this to D3D11_SDK_VERSION for Windows Store apps.
        &device,            // Returns the Direct3D device created.
        &_featureLevel,     // Returns feature level of device created.
        &context));         // Returns the device immediate context.

    //
    // Get the Direct3D 11.1 API device and context interfaces
    //

    ThrowIfFailed(device.As(&_d3dDevice));
    ThrowIfFailed(context.As(&_d3dContext));
}


void 
Direct3DBase::CreateSizeDependentResources()
{ 
    if (_destinationSize.Width == 0 
        || _destinationSize.Height == 0)
    {
        ThrowIfFailed(E_INVALIDARG);
    }

    //
    // Calculate the necessary swap chain and render target size in pixels.
    //

    float targetWidth = ConvertDipsToPixels(_destinationSize.Width);
    float targetHeight = ConvertDipsToPixels(_destinationSize.Height);

    //
    // The width and height of the swap chain must be based on the destination's
    // landscape-oriented width and height. If the destination rect is in a portrait
    // orientation, the dimensions must be reversed
    //

    _orientation = DisplayProperties::CurrentOrientation;
    bool swapDimensions =
        _orientation == DisplayOrientations::Portrait ||
        _orientation == DisplayOrientations::PortraitFlipped;
    _renderTargetSize.Width = swapDimensions ? targetHeight : targetWidth;
    _renderTargetSize.Height = swapDimensions ? targetWidth : targetHeight;

    if(_swapChain != nullptr)
    {
        // If the swap chain already exists, resize it.
        ThrowIfFailed(_swapChain->ResizeBuffers(
            NUM_BACKBUFFER,
            static_cast<UINT>(_renderTargetSize.Width),
            static_cast<UINT>(_renderTargetSize.Height),
            DXGI_FORMAT_B8G8R8A8_UNORM,
            0));
    }
    else
    {
        // Otherwise, create a new one using the same adapter as the existing Direct3D device.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
        swapChainDesc.Width = static_cast<UINT>(_renderTargetSize.Width); 
        swapChainDesc.Height = static_cast<UINT>(_renderTargetSize.Height);
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
        swapChainDesc.Stereo = false;
        swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = NUM_BACKBUFFER; // Use double-buffering to minimize latency.
        swapChainDesc.Flags = 0;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

        ComPtr<IDXGIDevice1>  dxgiDevice;
        ThrowIfFailed(_d3dDevice.As(&dxgiDevice));

        ComPtr<IDXGIAdapter> dxgiAdapter;
        ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

        ComPtr<IDXGIFactory2> dxgiFactory;
        ThrowIfFailed(dxgiAdapter->GetParent(
            __uuidof(IDXGIFactory2), 
            &dxgiFactory));
        ThrowIfFailed(dxgiFactory->CreateSwapChainForComposition(
            _d3dDevice.Get(),
            &swapChainDesc,
            NULL,
            &_swapChain));

        //
        // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
        // ensures that the application will only render after each VSync, minimizing power consumption
        //

        ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));
    }

    //
    // Create a render target view of the swap chain back buffer
    //

    ComPtr<ID3D11Texture2D> backBuffer;
    ThrowIfFailed( _swapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        &backBuffer));

    ThrowIfFailed(_d3dDevice->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        &_renderTargetView));

    //
    // Create a depth stencil view.
    //

    D3D11_TEXTURE2D_DESC descDepth;

    ZeroMemory(&descDepth,sizeof(descDepth));
    descDepth.Width = static_cast<UINT>(_renderTargetSize.Width);
    descDepth.Height = static_cast<UINT>(_renderTargetSize.Height);
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D16_UNORM;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> depthStencil;
    ThrowIfFailed(_d3dDevice->CreateTexture2D(
        &descDepth,
        nullptr,
        &depthStencil));

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    if (descDepth.SampleDesc.Count > 1)
    {
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    }
    else
    {
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
    }

    ThrowIfFailed(_d3dDevice->CreateDepthStencilView(
        depthStencil.Get(),
        &descDSV,
        &_depthStencilView));

    ComPtr<ID3D11RasterizerState> rasterizerState;
    D3D11_RASTERIZER_DESC RSDesc;
    ZeroMemory(&RSDesc,sizeof(RSDesc));
    RSDesc.FillMode = D3D11_FILL_SOLID;
    RSDesc.CullMode = D3D11_CULL_BACK;
    RSDesc.FrontCounterClockwise = FALSE;
    RSDesc.DepthBias = 0;
    RSDesc.DepthBiasClamp = 0;
    RSDesc.DepthClipEnable = TRUE;
    RSDesc.SlopeScaledDepthBias = 0.0f;
    RSDesc.ScissorEnable = FALSE;
    RSDesc.MultisampleEnable = FALSE;
    RSDesc.AntialiasedLineEnable = FALSE;

    ThrowIfFailed(_d3dDevice->CreateRasterizerState(
        &RSDesc, 
        &rasterizerState));

    //         
    // Set the rendering viewport to target the entire destination rect
    //

    D3D11_VIEWPORT vp;
    ZeroMemory(&vp,sizeof(vp));
    vp.Width = _renderTargetSize.Width;
    vp.Height = _renderTargetSize.Height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    _d3dContext->RSSetViewports(1, &vp);
}


void 
Direct3DBase::Present()
{
    //
    // The application may optionally specify "dirty" or "scroll"
    // rects to improve efficiency in certain scenarios
    //

    DXGI_PRESENT_PARAMETERS parameters = {0};
    parameters.DirtyRectsCount = 0;
    parameters.pDirtyRects = nullptr;
    parameters.pScrollRect = nullptr;
    parameters.pScrollOffset = nullptr;
    
    //
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen
    //

    HRESULT hr = _swapChain->Present1(1, 0, &parameters);

    //
    // Discard the contents of the render target.
    // This is a valid operation only when the existing contents will be entirely
    // overwritten. If dirty or scroll rects are used, this call should be removed.
    //

    _d3dContext->DiscardView(_renderTargetView.Get());

    //
    // Discard the contents of the depth stencil
    //

    _d3dContext->DiscardView(_depthStencilView.Get());

    //
    // If the device was removed either by a disconnect or a driver upgrade, we 
    // must recreate all device resources
    //

    if (hr == DXGI_ERROR_DEVICE_REMOVED)
    {
        HandleDeviceLost();
    }
    else
    {
        ThrowIfFailed(hr);
    }
}

// Method to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
float 
Direct3DBase::ConvertDipsToPixels(
    float dips)
{
    static const float dipsPerInch = 96.0f;

    // Round to the nearest integer
    return floor(dips * DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); 
}
