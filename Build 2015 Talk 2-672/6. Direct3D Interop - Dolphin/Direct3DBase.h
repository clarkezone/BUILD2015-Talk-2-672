//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once


#define NUM_BACKBUFFER 2

// Helper class that initializes DirectX APIs for 3D rendering.
class Direct3DBase 
{
public:
    virtual ~Direct3DBase();

    virtual void Initialize(Windows::Foundation::Size & contentSize);

public:
    virtual float ConvertDipsToPixels(float dips);

    virtual void CreateDeviceResources();

    virtual void CreateSizeDependentResources();

    virtual void HandleDeviceLost();

    virtual void Present();

    virtual void Render() = 0;

 protected:
    // Direct3D Objects.
    ComPtr<ID3D11Device1>           _d3dDevice;
    ComPtr<ID3D11DeviceContext1>    _d3dContext;
    ComPtr<IDXGISwapChain1>         _swapChain;
    ComPtr<ID3D11DepthStencilView>  _depthStencilView;
    ComPtr<ID3D11RenderTargetView>  _renderTargetView;

    // Cached renderer properties.
    D3D_FEATURE_LEVEL               _featureLevel;
    Windows::Foundation::Size       _renderTargetSize;
    Windows::Foundation::Size       _destinationSize;
    DisplayOrientations             _orientation;

    // Transform used for display orientation.
    DirectX::XMFLOAT4X4             _orientationTransform3D;
};
