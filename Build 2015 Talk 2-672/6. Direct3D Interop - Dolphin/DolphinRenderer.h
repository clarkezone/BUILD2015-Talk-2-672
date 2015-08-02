//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once

#include "direct3dbase.h"
#include "sdkmesh.h"


struct ModelViewProjectionConstantBuffer
{
    DirectX::XMFLOAT4X4 model;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
};


struct VS_CONSTANT_BUFFER
{
    DirectX::XMVECTOR vZero;
    DirectX::XMVECTOR vConstants;
    DirectX::XMVECTOR vWeight;

    DirectX::XMMATRIX matTranspose;
    DirectX::XMMATRIX matCameraTranspose;
    DirectX::XMMATRIX matViewTranspose;
    DirectX::XMMATRIX matProjTranspose;

    DirectX::XMVECTOR vLight;
    DirectX::XMVECTOR vLightDolphinSpace;
    DirectX::XMVECTOR vDiffuse;
    DirectX::XMVECTOR vAmbient;
    DirectX::XMVECTOR vFog;
    DirectX::XMVECTOR vCaustics;
};


struct VertexPositionColor
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 color;
};


struct PS_CONSTANT_BUFFER
{
    FLOAT fAmbient[4];
    FLOAT fFogColor[4];
};


class DolphinRenderer :
    public Direct3DBase
{
public:
    DolphinRenderer();

    virtual void CreateDeviceResources() override;

    virtual void CreateSizeDependentResources() override;

    IDXGISwapChain1 * GetSwapChain();

    virtual void Render() override;

    void Update(
        float timeTotal, 
        float timeDelta);

private:
    void CreateShaderResourceViewFromBuffer(
        _In_ BYTE * buffer,
        _In_ ULONG width,
        _In_ ULONG height,
        _In_ DXGI_FORMAT format,
        _Out_ ID3D11ShaderResourceView ** view);

     HRESULT ReadFileContents(
        const wchar_t *szFilename,
        BYTE **ppbBuffer,
        DWORD *pcbBuffer);

private:
    bool                                _loadingComplete;

    ComPtr<ID3D11InputLayout>           _inputLayout;
    ComPtr<ID3D11Buffer>                _vertexBuffer;
    ComPtr<ID3D11Buffer>                _indexBuffer;
    ComPtr<ID3D11VertexShader>          _vertexShader;
    ComPtr<ID3D11PixelShader>           _pixelShader;
    ComPtr<ID3D11Buffer>                _constantBuffer;

    uint32                              _indexCount;
    ModelViewProjectionConstantBuffer   _constantBufferData;

    DirectX::XMMATRIX                   _matrixWorld;
    DirectX::XMMATRIX                   _matrixView;
    DirectX::XMMATRIX                   _matrixProjection;
    ComPtr<ID3D11Buffer>                _VSConstantBuffer;
    ComPtr<ID3D11Buffer>                _PSConstantBuffer;

    ComPtr<ID3D11ShaderResourceView>    _dolphinTextureView;
    ComPtr<ID3D11VertexShader>          _dolphinVertexShader;
    ComPtr<ID3D11InputLayout>           _dolphinVertexLayout;

    ComPtr<ID3D11ShaderResourceView>    _seaFloorTextureView;
    ComPtr<ID3D11VertexShader>          _seaFloorVertexShader;
    ComPtr<ID3D11InputLayout>           _seaFloorVertexLayout;

    CDXUTSDKMesh                        _dolphinMesh1;
    CDXUTSDKMesh                        _dolphinMesh2;
    CDXUTSDKMesh                        _dolphinMesh3;
    CDXUTSDKMesh                        _seaFloorMesh;
    ComPtr<ID3D11Buffer>                _dolphinVB1;
    ComPtr<ID3D11Buffer>                _dolphinVB2;
    ComPtr<ID3D11Buffer>                _dolphinVB3;
    ComPtr<ID3D11Buffer>                _dolphinIB;
    ComPtr<ID3D11Buffer>                _seaFloorVB;
    ComPtr<ID3D11Buffer>                _seaFloorIB;

    D3D11_PRIMITIVE_TOPOLOGY            _dolphinPrimType;
    DWORD                               _numDolphinIndices;
    DWORD                               _dolphinVertexStride;
    DXGI_FORMAT                         _dolphinVertexFormat;

    D3D11_PRIMITIVE_TOPOLOGY            _seaFloorPrimType;
    DWORD                               _seaFloorVertexStride;
    DWORD                               _numSeaFloorIndices;
    DXGI_FORMAT                         _seaFloorVertexFormat;

    ComPtr<ID3D11ShaderResourceView>    _causticTextureViews[32];
    ID3D11ShaderResourceView *          _currentCausticTextureView;

    ComPtr<ID3D11BlendState>            _blendStateNoBlend;
    ComPtr<ID3D11RasterizerState>       _rasterizerStateNoCull;
    ComPtr<ID3D11DepthStencilState>     _lessEqualDepth;
    ComPtr<ID3D11SamplerState>          _samplerMirror;
    ComPtr<ID3D11SamplerState>          _samplerWrap;
};
