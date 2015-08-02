//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#include "pch.h"
#include "DolphinRenderer.h"

#include "BitmapDecode.h"


const D3D11_INPUT_ELEMENT_DESC g_vertexDesc[] = 
{
    { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "POSITION",  1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",    1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",  1, DXGI_FORMAT_R32G32_FLOAT,    1, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "POSITION",  2, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",    2, DXGI_FORMAT_R32G32B32_FLOAT, 2, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",  2, DXGI_FORMAT_R32G32_FLOAT,    2, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

const FLOAT fWaterColor[4] = {0.0f, 0.5f, 1.0f, 1.0f};


DolphinRenderer::DolphinRenderer() :
    _loadingComplete(false),
    _indexCount(0)
{
}

void 
DolphinRenderer::CreateDeviceResources()
{
    Direct3DBase::CreateDeviceResources();

    D3D11_BUFFER_DESC cbDesc;
    ZeroMemory(&cbDesc,sizeof(cbDesc));
    cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;

    ThrowIfFailed(_d3dDevice->CreateBuffer(
        &cbDesc, 
        nullptr, 
        &_VSConstantBuffer));

    ThrowIfFailed(_d3dDevice->CreateBuffer(
        &cbDesc, 
        nullptr, 
        &_PSConstantBuffer));

    {
		ULONG dolphinHeight = 0;
		ULONG dolphinWidth = 0;
		BYTE* fileData = NULL;
		DWORD fileDataLength = 0;
        BYTE* dataTemp = NULL;

        ThrowIfFailed(ReadFileContents(
            L"Assets\\dolphin.bmp", 
            &fileData, 
            &fileDataLength));
        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(BitmapDecode::LoadBMP(
            fileData, 
            &dolphinWidth, 
            &dolphinHeight, 
            &dataTemp));
       std::unique_ptr<BYTE[]> data2(dataTemp);

        CreateShaderResourceViewFromBuffer(
            dataTemp, 
            dolphinWidth, 
            dolphinHeight, 
            DXGI_FORMAT_R8G8B8A8_UNORM, 
            &_dolphinTextureView);
    }

    {
		BYTE* fileData = NULL;
		DWORD fileDataLength = 0;

        ThrowIfFailed(ReadFileContents(
            L"Assets\\Dolphin1.sdkmesh", 
            &fileData, 
            &fileDataLength));

        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(_dolphinMesh1.Create(
            _d3dDevice.Get(), 
            fileData, 
            fileDataLength));

        _dolphinVB1 = _dolphinMesh1.GetVB11(0, 0);
        _dolphinIB = _dolphinMesh1.GetIB11(0);

        SDKMESH_SUBSET* pSubset = _dolphinMesh1.GetSubset(0, 0);
        _dolphinPrimType = CDXUTSDKMesh::GetPrimitiveType11((SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType);
        _numDolphinIndices = static_cast<DWORD>(_dolphinMesh1.GetNumIndices(0));
        _dolphinVertexStride = static_cast<DWORD>(_dolphinMesh1.GetVertexStride(0, 0));
        _dolphinVertexFormat = _dolphinMesh1.GetIBFormat11(0);
    }

    {
        BYTE* fileData;
        DWORD fileDataLength;

        ThrowIfFailed(ReadFileContents(
            L"Assets\\dolphin2.sdkmesh", 
            &fileData, 
            &fileDataLength));
        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(_dolphinMesh2.Create(
            _d3dDevice.Get(), fileData, fileDataLength));

        _dolphinVB2 = _dolphinMesh2.GetVB11(0, 0);
    };

    {
        BYTE* fileData;
        DWORD fileDataLength;

        ThrowIfFailed(ReadFileContents(
            L"Assets\\dolphin3.sdkmesh", 
            &fileData, 
            &fileDataLength));
        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(_dolphinMesh3.Create(
            _d3dDevice.Get(), 
            fileData, 
            fileDataLength));

        _dolphinVB3 = _dolphinMesh3.GetVB11(0, 0);
    };

    {
        BYTE* fileData;
        DWORD fileDataLength;

        ThrowIfFailed(ReadFileContents(
            L"Assets\\dolphintween.xvu", 
            &fileData, 
            &fileDataLength));
        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(_d3dDevice->CreateVertexShader(
            fileData, 
            fileDataLength, 
            nullptr, 
            &_dolphinVertexShader));
        ThrowIfFailed(_d3dDevice->CreateInputLayout(
            g_vertexDesc, 
            ARRAYSIZE(g_vertexDesc), 
            fileData, 
            fileDataLength, 
            &_dolphinVertexLayout));
    };

    {
        ULONG imageHeight;
        ULONG imageWidth;
        BYTE* dataTemp = NULL;
        BYTE* fileData;
        DWORD fileDataLength;

        ThrowIfFailed(ReadFileContents(
            L"Assets\\seafloor.bmp", 
            &fileData, 
            &fileDataLength));
        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(BitmapDecode::LoadBMP(
            fileData, 
            &imageWidth, 
            &imageHeight, 
            &dataTemp));
        std::auto_ptr<BYTE> data2(dataTemp);
        CreateShaderResourceViewFromBuffer(
            dataTemp, 
            imageWidth, imageHeight, 
            DXGI_FORMAT_R8G8B8A8_UNORM, 
            &_seaFloorTextureView);
    };

    {
        BYTE* fileData;
        DWORD fileDataLength;

        ThrowIfFailed(ReadFileContents(
            L"Assets\\seafloor.sdkmesh", 
            &fileData, 
            &fileDataLength));
        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(_seaFloorMesh.Create(
            _d3dDevice.Get(), 
            fileData, 
            fileDataLength));

        _seaFloorVB = _seaFloorMesh.GetVB11(0, 0);
        _seaFloorIB = _seaFloorMesh.GetIB11(0);

        SDKMESH_SUBSET* pSubset = _seaFloorMesh.GetSubset(0, 0);
        _seaFloorPrimType = CDXUTSDKMesh::GetPrimitiveType11((SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType);
        _numSeaFloorIndices = static_cast<DWORD>(_seaFloorMesh.GetNumIndices(0));
        _seaFloorVertexStride = static_cast<DWORD>(_seaFloorMesh.GetVertexStride(0, 0));
        _seaFloorVertexFormat = _seaFloorMesh.GetIBFormat11(0);

        D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };

        ThrowIfFailed(_d3dContext->Map(
            _seaFloorVB.Get(), 
            0, 
            D3D11_MAP_WRITE_NO_OVERWRITE, 
            0, 
            &mappedResource));

        srand(5);
        BYTE* pDst = reinterpret_cast<BYTE *>(mappedResource.pData);
        for (DWORD i = 0; i < _seaFloorMesh.GetNumVertices(0, 0) ; ++i)
        {
            ((DirectX::XMFLOAT3 *)pDst)->y += (rand() / (FLOAT)RAND_MAX);
            ((DirectX::XMFLOAT3 *)pDst)->y += (rand() / (FLOAT)RAND_MAX);
            ((DirectX::XMFLOAT3 *)pDst)->y += (rand() / (FLOAT)RAND_MAX);
            pDst += sizeof(FLOAT) * 8;
        }
        _d3dContext->Unmap(_seaFloorVB.Get(), 0);
    };

    {
        BYTE* fileData;
        DWORD fileDataLength;

        ThrowIfFailed(ReadFileContents(
            L"Assets\\SeaFloor.xvu", 
            &fileData, 
            &fileDataLength));
        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(_d3dDevice->CreateVertexShader(
            fileData, 
            fileDataLength, 
            nullptr, 
            &_seaFloorVertexShader));

        ThrowIfFailed(_d3dDevice->CreateInputLayout(
            g_vertexDesc, 
            ARRAYSIZE(g_vertexDesc), 
            fileData, 
            fileDataLength, 
            &_seaFloorVertexLayout));
    };

    for (int i = 0; i < 32; i++)
    {
        ULONG height;
        ULONG width;
        BYTE* dataTemp = NULL;
        BYTE* fileData;
        DWORD fileDataLength;

        wchar_t fileName[19];
        swprintf_s(
            fileName, 
            ARRAYSIZE(fileName), 
            L"Assets\\caust%02I32d.tga", 
            i);

        ThrowIfFailed(ReadFileContents(
            fileName, 
            &fileData, 
            &fileDataLength));
        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(BitmapDecode::LoadTGA(
            fileData, 
            &width, &height, 
            &dataTemp));
        std::unique_ptr<BYTE[]> data2(dataTemp);

        _causticTextureViews[i] = nullptr;
        CreateShaderResourceViewFromBuffer(
            dataTemp, 
            width, height, 
            DXGI_FORMAT_R8G8B8A8_UNORM, 
            &_causticTextureViews[i]);
    }

    {
        BYTE* fileData;
        DWORD fileDataLength;

        ThrowIfFailed(ReadFileContents(
            L"Assets\\ShadeCausticsPixel.xpu", 
            &fileData, 
            &fileDataLength));
        std::unique_ptr<BYTE[]> data(fileData);

        ThrowIfFailed(_d3dDevice->CreatePixelShader(
            fileData, 
            fileDataLength, 
            nullptr, 
            &_pixelShader));
    };

    // Create a blend state to disable alpha blending
    {
        D3D11_BLEND_DESC blendState;
        ZeroMemory(&blendState, sizeof(blendState));
        blendState.RenderTarget[0].BlendEnable = FALSE;
        blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        ThrowIfFailed(_d3dDevice->CreateBlendState(
            &blendState, 
            &_blendStateNoBlend));
    }

    // Create a rasterizer state to disable culling
    {
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
        RSDesc.MultisampleEnable = TRUE;
        RSDesc.AntialiasedLineEnable = FALSE;       
        ThrowIfFailed(_d3dDevice->CreateRasterizerState(
            &RSDesc, 
            &_rasterizerStateNoCull));
    }

    // Create a depth stencil state to enable less-equal depth testing
    {
        D3D11_DEPTH_STENCIL_DESC DSDesc;
        ZeroMemory(&DSDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        DSDesc.DepthEnable = TRUE;
        DSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        ThrowIfFailed(_d3dDevice->CreateDepthStencilState(
            &DSDesc, 
            &_lessEqualDepth));
    }

    // Create Mirror Sampler state
    {
        D3D11_SAMPLER_DESC sampMirror;
        ZeroMemory(&sampMirror, sizeof(D3D11_SAMPLER_DESC));
        sampMirror.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampMirror.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampMirror.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampMirror.MaxLOD = D3D11_FLOAT32_MAX;
        sampMirror.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        ThrowIfFailed(_d3dDevice->CreateSamplerState(
            &sampMirror, 
            &_samplerMirror));
    }

    // Create Wrap sampler state
    {
        D3D11_SAMPLER_DESC sampWrap;
        ZeroMemory(&sampWrap, sizeof(D3D11_SAMPLER_DESC));
        sampWrap.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampWrap.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampWrap.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampWrap.MaxLOD = D3D11_FLOAT32_MAX;
        sampWrap.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        ThrowIfFailed(_d3dDevice->CreateSamplerState(
            &sampWrap, 
            &_samplerWrap));
    }

    _loadingComplete = true;
}

void 
DolphinRenderer::CreateSizeDependentResources()
{
    Direct3DBase::CreateSizeDependentResources();

    float aspectRatio = _destinationSize.Width / _destinationSize.Height;
    float fovAngleY = 70.0f * DirectX::XM_PI / 180.0f;

    //
    // Note that the _orientationTransform3D matrix is post-multiplied here
    // in order to correctly orient the scene to match the display orientation.
    // This post-multiplication step is required for any draw calls that are
    // made to the swap chain render target. For draw calls to other targets,
    // this transform should not be applied
    //

    DirectX::XMStoreFloat4x4(
        &_constantBufferData.projection,
        DirectX::XMMatrixTranspose(
            DirectX::XMMatrixMultiply(
                DirectX::XMMatrixPerspectiveFovRH(
                    fovAngleY,
                    aspectRatio,
                    0.01f,
                    100.0f),
                DirectX::XMLoadFloat4x4(&_orientationTransform3D))));

    if (_destinationSize.Width >= _destinationSize.Height)
    {
        // Set the transform matrices
        DirectX::XMVECTOR vEyePt = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
        DirectX::XMVECTOR vLookatPt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        DirectX::XMVECTOR vUpVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        _matrixWorld = DirectX::XMMatrixIdentity();
        _matrixView = DirectX::XMMatrixLookAtLH(vEyePt, vLookatPt, vUpVec);
        _matrixProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 3, aspectRatio, 1.0f, 10000.0f);
    }
    else
    {
        // Set the transform matrices
        DirectX::XMVECTOR vEyePt = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
        DirectX::XMVECTOR vLookatPt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        DirectX::XMVECTOR vUpVec = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        _matrixWorld = DirectX::XMMatrixIdentity();
        _matrixView = DirectX::XMMatrixLookAtLH(vEyePt, vLookatPt, vUpVec);
        _matrixProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 2, aspectRatio, 1.0f, 10000.0f);
    }
}


IDXGISwapChain1 * 
DolphinRenderer::GetSwapChain()
{
    return _swapChain.Get();
}

void 
DolphinRenderer::Update(
    float timeTotal, 
    float)
{
    FLOAT fAmbient[4] = 
    { 
        0.25f, 
        0.25f, 
        0.25f, 
        0.25f 
    };

    //
    // Animation attributes for the dolphin
    //

    FLOAT fKickFreq = 2 * timeTotal;
    FLOAT fPhase = timeTotal / 3;
    FLOAT fBlendWeight = sinf(fKickFreq);

    //
    // Move the dolphin in a circle
    //

    DirectX::XMMATRIX matDolphin, matTrans, matRotate1, matRotate2;
    matDolphin = DirectX::XMMatrixScaling(0.01f, 0.01f, 0.01f);
    matRotate1 = DirectX::XMMatrixRotationZ(-cosf(fKickFreq) / 6);
    matDolphin = DirectX::XMMatrixMultiply(matDolphin, matRotate1);
    matRotate2 = DirectX::XMMatrixRotationY(fPhase);
    matDolphin = DirectX::XMMatrixMultiply(matDolphin, matRotate2);
    matTrans = DirectX::XMMatrixTranslation(-5 * sinf(fPhase), sinf(fKickFreq) / 2, 10 - 10 * cosf(fPhase));
    matDolphin = DirectX::XMMatrixMultiply(matDolphin, matTrans);

    //
    // Animate the caustic textures
    //

    DWORD tex = ((DWORD)(timeTotal * 32)) % 32;
    _currentCausticTextureView = _causticTextureViews[tex].Get();

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    ThrowIfFailed(_d3dContext->Map(
        _VSConstantBuffer.Get(), 
        0, 
        D3D11_MAP_WRITE_DISCARD, 
        0, 
        &MappedResource));

    {
        VS_CONSTANT_BUFFER* pVsConstData = (VS_CONSTANT_BUFFER*)MappedResource.pData;

        pVsConstData->vZero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        pVsConstData->vConstants = DirectX::XMVectorSet(1.0f, 0.5f, 0.2f, 0.05f);

        FLOAT fWeight1;
        FLOAT fWeight2;
        FLOAT fWeight3;

        if (fBlendWeight > 0.0f)
        {
            fWeight1 = fabsf(fBlendWeight);
            fWeight2 = 1.0f - fabsf(fBlendWeight);
            fWeight3 = 0.0f;
        }
        else
        {
            fWeight1 = 0.0f;
            fWeight2 = 1.0f - fabsf(fBlendWeight);
            fWeight3 = fabsf(fBlendWeight);
        }
        pVsConstData->vWeight = DirectX::XMVectorSet(fWeight1, fWeight2, fWeight3, 0.0f);

        //
        // Lighting vectors (in world space and in dolphin model space)
        // and other constants
        //

        pVsConstData->vLight = DirectX::XMVectorSet(0.00f, 1.00f, 0.00f, 0.00f);
        pVsConstData->vLightDolphinSpace = DirectX::XMVectorSet(0.00f, 1.00f, 0.00f, 0.00f);
        pVsConstData->vDiffuse = DirectX::XMVectorSet(1.00f, 1.00f, 1.00f, 1.00f);
        pVsConstData->vAmbient = DirectX::XMVectorSet(fAmbient[0], fAmbient[1], fAmbient[2], fAmbient[3]);
        pVsConstData->vFog = DirectX::XMVectorSet(0.50f, 50.00f, 1.00f / (50.0f - 1.0f), 0.00f);
        pVsConstData->vCaustics = DirectX::XMVectorSet(0.05f, 0.05f, sinf(timeTotal) / 8, cosf(timeTotal) / 10);

        DirectX::XMVECTOR vDeterminant;
        DirectX::XMMATRIX matDolphinInv = DirectX::XMMatrixInverse(&vDeterminant, matDolphin);
        pVsConstData->vLightDolphinSpace = DirectX::XMVector4Normalize(
            DirectX::XMVector4Transform(
                pVsConstData->vLight, 
                matDolphinInv));

        //
        // Vertex shader operations use transposed matrices
        //

        DirectX::XMMATRIX mat, matCamera;
        matCamera = DirectX::XMMatrixMultiply(matDolphin, _matrixView);
        mat = DirectX::XMMatrixMultiply(matCamera, _matrixProjection);
        pVsConstData->matTranspose = DirectX::XMMatrixTranspose(mat);
        pVsConstData->matCameraTranspose = DirectX::XMMatrixTranspose(matCamera);
        pVsConstData->matViewTranspose = DirectX::XMMatrixTranspose(_matrixView);
        pVsConstData->matProjTranspose = DirectX::XMMatrixTranspose(_matrixProjection);
    }

    _d3dContext->Unmap(_VSConstantBuffer.Get(), 0);

    ThrowIfFailed(
        _d3dContext->Map(_PSConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)
       );

    {
        PS_CONSTANT_BUFFER* pPsConstData = (PS_CONSTANT_BUFFER*)MappedResource.pData;
        memcpy(pPsConstData->fAmbient, fAmbient, sizeof(fAmbient));
        memcpy(pPsConstData->fFogColor, fWaterColor, sizeof(fWaterColor));
    }
    _d3dContext->Unmap(_PSConstantBuffer.Get(), 0);
}

void 
DolphinRenderer::Render()
{
    if (!_loadingComplete)
    {
        return;
    }

    //
    // Set RenderTargetView
    //

    _d3dContext->OMSetRenderTargets(
        1,
        _renderTargetView.GetAddressOf(),
        _depthStencilView.Get());

    //
    // Clear render target
    //

    _d3dContext->ClearRenderTargetView(
        _renderTargetView.Get(),
        fWaterColor);

    _d3dContext->ClearDepthStencilView(
        _depthStencilView.Get(),
        D3D11_CLEAR_DEPTH,
        1.0f,
        0);

    //
    // Set state
    //

    _d3dContext->OMSetBlendState(_blendStateNoBlend.Get(), 0, 0xffffffff);
    _d3dContext->RSSetState(_rasterizerStateNoCull.Get());
    _d3dContext->OMSetDepthStencilState(_lessEqualDepth.Get(), 0);

    ID3D11SamplerState* pSamplers[] = 
    {
        _samplerMirror.Get(), 
        _samplerWrap.Get()
    };

    _d3dContext->PSSetSamplers(0, 2, pSamplers);
    _d3dContext->VSSetConstantBuffers(0, 1, _VSConstantBuffer.GetAddressOf());
    _d3dContext->PSSetConstantBuffers(0, 1, _PSConstantBuffer.GetAddressOf());

    //
    // Render sea floor
    //

    _d3dContext->IASetInputLayout(_seaFloorVertexLayout.Get());
    UINT SeaFloorStrides[1];
    UINT SeaFloorOffsets[1];
    SeaFloorStrides[0] = (UINT) _seaFloorVertexStride;
    SeaFloorOffsets[0] = 0;
    _d3dContext->IASetVertexBuffers(0, 1, _seaFloorVB.GetAddressOf(), SeaFloorStrides, SeaFloorOffsets);
    _d3dContext->IASetIndexBuffer(_seaFloorIB.Get(), _seaFloorVertexFormat, 0);
    _d3dContext->IASetPrimitiveTopology(_dolphinPrimType);
    _d3dContext->VSSetShader(_seaFloorVertexShader.Get(), NULL, 0);
    _d3dContext->GSSetShader(NULL,NULL, 0);
    _d3dContext->PSSetShader(_pixelShader.Get(), NULL, 0);
    _d3dContext->PSSetShaderResources(0,1,_seaFloorTextureView.GetAddressOf());
    _d3dContext->PSSetShaderResources(1,1,&_currentCausticTextureView);
    _d3dContext->DrawIndexed((UINT)_numSeaFloorIndices, 0, 0);

    //
    // Render Dolphin
    //

    _d3dContext->IASetInputLayout(_dolphinVertexLayout.Get());
    UINT DolphinStrides[3];
    UINT DolphinOffsets[3];
    ID3D11Buffer* pDolphinVB[3];
    pDolphinVB[0] = _dolphinVB1.Get();
    pDolphinVB[1] = _dolphinVB2.Get();
    pDolphinVB[2] = _dolphinVB3.Get();
    DolphinStrides[0] = (UINT) _dolphinVertexStride;
    DolphinStrides[1] = (UINT) _dolphinVertexStride;
    DolphinStrides[2] = (UINT) _dolphinVertexStride;
    DolphinOffsets[0] = 0;
    DolphinOffsets[1] = 0;
    DolphinOffsets[2] = 0;
    _d3dContext->IASetVertexBuffers(0, 3, pDolphinVB, DolphinStrides, DolphinOffsets);
    _d3dContext->IASetIndexBuffer(_dolphinIB.Get(), _dolphinVertexFormat, 0);
    _d3dContext->IASetPrimitiveTopology(_dolphinPrimType);
    _d3dContext->VSSetShader(_dolphinVertexShader.Get() ,NULL, 0);
    _d3dContext->GSSetShader(NULL,NULL, 0);
    _d3dContext->PSSetShader(_pixelShader.Get(), NULL, 0);
    _d3dContext->PSSetShaderResources(0,1,_dolphinTextureView.GetAddressOf());
    _d3dContext->PSSetShaderResources(1,1,&_currentCausticTextureView);
    _d3dContext->DrawIndexed((UINT)_numDolphinIndices, 0, 0);
}


void 
DolphinRenderer::CreateShaderResourceViewFromBuffer(
    _In_ BYTE* buffer,
    _In_ ULONG width,
    _In_ ULONG height,
    _In_ DXGI_FORMAT format,
    _Out_ ID3D11ShaderResourceView** view)
{
    D3D11_TEXTURE2D_DESC desc = {0};

    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA pixelData[1] = {0};
    pixelData[0].pSysMem = buffer;
    pixelData[0].SysMemPitch = width * BitmapDecode::GetDXGIFormatTexelSize(desc.Format);
    pixelData[0].SysMemSlicePitch = 0;

    ComPtr<ID3D11Texture2D> texture;
    ThrowIfFailed(_d3dDevice->CreateTexture2D(
        &desc,
        pixelData,
        &texture));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc,sizeof(srvDesc));
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;

    // Must be between 0 and MipLevels - 1.
    srvDesc.Texture2D.MostDetailedMip = 0;

    ThrowIfFailed(_d3dDevice->CreateShaderResourceView(
        texture.Get(),
        &srvDesc,
        view));
}


HRESULT 
DolphinRenderer::ReadFileContents(
    const wchar_t *szFilename,
    BYTE ** ppbBuffer,
    DWORD *pcbBuffer)
{
    HANDLE hFile = NULL;
    HRESULT hr = S_OK;
    BYTE *pb = NULL;
    DWORD dwRead;
	LARGE_INTEGER fileSize = { 0 };

	hFile = CreateFile2(
		szFilename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto done;
    }

	// Get the file size.
	FILE_STANDARD_INFO fileInfo;
	if (!GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo)))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	fileSize = fileInfo.EndOfFile;

	// File is too big for 32-bit allocation, so reject read.
	if (fileSize.HighPart > 0)
	{ 
        hr = E_FAIL;
        goto done;
    }

    pb = new BYTE[fileSize.LowPart];

    if (!ReadFile(hFile, pb, fileSize.LowPart, &dwRead, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto done;
    }

    if (dwRead < fileSize.LowPart)
    {
        goto done;
    }

    *ppbBuffer = pb;
    *pcbBuffer = fileSize.LowPart;

    pb = NULL;

done:
	if (pb)
	{
		delete[] pb;
	}

    if (NULL != hFile)
    {
        CloseHandle(hFile);
    }

    return hr;
}
