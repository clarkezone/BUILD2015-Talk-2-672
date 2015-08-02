//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#include "pch.h"
#include "BitmapDecode.h"

#define MyReadData(pDst,Size) CopyMemory((PVOID)(pDst), (PVOID)(pFile), (Size)); (pFile)+=(Size);


HRESULT 
BitmapDecode::LoadBMP(
    PBYTE pFile,
    ULONG * pRetWidth,
    ULONG * pRetHeight,
    PBYTE * pRetData)
{
    HRESULT             hr = S_OK;
    BITMAPFILEHEADER    hdr = { 0 };
    BITMAPINFOHEADER    infoHdr = { 0 };
    RGBQUAD *           bmiColors = NULL;

    *pRetWidth = 0;
    *pRetHeight = 0;
    *pRetData = NULL;

    //
    // Load header and do error checking
    //

    MyReadData(&hdr.bfType, sizeof(hdr.bfType));

    // Check that this is a bitmap file.
    if (hdr.bfType != 0x4D42)
    {
        return E_FAIL;
    }

    MyReadData(&hdr.bfSize, sizeof(hdr.bfSize));
    MyReadData(&hdr.bfReserved1, sizeof(hdr.bfReserved1));
    MyReadData(&hdr.bfReserved2, sizeof(hdr.bfReserved2));
    MyReadData(&hdr.bfOffBits, sizeof(hdr.bfOffBits));
    MyReadData(&infoHdr.biSize, sizeof(infoHdr.biSize));
    MyReadData(&infoHdr.biWidth, sizeof(infoHdr.biWidth));
    MyReadData(&infoHdr.biHeight, sizeof(infoHdr.biHeight));
    MyReadData(&infoHdr.biPlanes, sizeof(infoHdr.biPlanes));
    MyReadData(&infoHdr.biBitCount, sizeof(infoHdr.biBitCount));
    MyReadData(&infoHdr.biCompression, sizeof(infoHdr.biCompression));
    MyReadData(&infoHdr.biSizeImage, sizeof(infoHdr.biSizeImage));
    MyReadData(&infoHdr.biXPelsPerMeter, sizeof(infoHdr.biXPelsPerMeter));
    MyReadData(&infoHdr.biYPelsPerMeter, sizeof(infoHdr.biYPelsPerMeter));
    MyReadData(&infoHdr.biClrUsed, sizeof(infoHdr.biClrUsed));
    MyReadData(&infoHdr.biClrImportant, sizeof(infoHdr.biClrImportant));

    if ((infoHdr.biSize == 40
        && infoHdr.biPlanes == 1
        && (infoHdr.biBitCount == 1
        || infoHdr.biBitCount == 2
        || infoHdr.biBitCount == 4
        || infoHdr.biBitCount == 8
        || infoHdr.biBitCount == 24
        || infoHdr.biBitCount == 32)
        && (infoHdr.biCompression == BI_RGB) == FALSE))
    {
        return E_FAIL;
    }

    PBYTE pData = new BYTE[infoHdr.biWidth * infoHdr.biHeight * 4];

    // Load the palette if this is a pallete format.
    if (infoHdr.biBitCount <= 8)
    {
        // This determines the number of colors that  
        // a format can have: 1 bit = 1, 4 bit = 16,
        // 8 bit = 256.  This is used to determine
        // how big the palette should be.
        int numColors = 1 << infoHdr.biBitCount;

        bmiColors = new RGBQUAD[numColors];

        for (int x = 0; x < numColors; ++x)
        {
            char r, g, b, res;

            MyReadData(&b, sizeof(b));
            MyReadData(&g, sizeof(g));
            MyReadData(&r, sizeof(r));
            MyReadData(&res, sizeof(res));

            bmiColors[x].rgbBlue = b;
            bmiColors[x].rgbGreen = g;
            bmiColors[x].rgbRed = r;
            bmiColors[x].rgbReserved = res;
        }
    }

    if (infoHdr.biCompression == BI_RGB)
    {
        if (infoHdr.biBitCount == 1)            // mono
        {
            hr = LoadMono(pFile, infoHdr, pData, bmiColors);
        }
        else if (infoHdr.biBitCount == 4)       // 16-colors uncompressed
        {
            hr = LoadRGB4(pFile, infoHdr, pData, bmiColors);
        }
        else if (infoHdr.biBitCount == 8)       // 256-colors uncompressed
        {
            hr = LoadRGB8(pFile, infoHdr, pData, bmiColors);
        }
        else if (infoHdr.biBitCount == 24)      // True-Color bitmap
        {
            hr = LoadRGB24(pFile, infoHdr, pData);
        }
        else if (infoHdr.biBitCount == 32)      // true-color bitmap with alpha-channel
        {
            hr = LoadRGB32(pFile, infoHdr, pData);
        }
    }

    if (bmiColors)
    {
        delete[] bmiColors;
    }

    if (FAILED(hr))
    {
        if (pData)
        {
            delete[] pData;
        }
    }
    else
    {
        *pRetWidth = infoHdr.biWidth;
        *pRetHeight = infoHdr.biHeight;
        *pRetData = pData;
    }

    return hr;
}


HRESULT 
BitmapDecode::LoadTGA(
    PBYTE pFile, 
    ULONG * pRetWidth, 
    ULONG * pRetHeight, 
    PBYTE * pRetData)
{
    HRESULT         hr = S_OK;
    TargaPalette *  pPalette = NULL;
    TargaHeader     hdr = { 0 };

    //
    // Load header and do error checking
    //

    MyReadData(&hdr.id_length, sizeof(hdr.id_length));
    MyReadData(&hdr.colormap_type, sizeof(hdr.colormap_type));
    MyReadData(&hdr.image_type, sizeof(hdr.image_type));
    MyReadData(&hdr.colormap_index, sizeof(hdr.colormap_index));
    MyReadData(&hdr.colormap_length, sizeof(hdr.colormap_length));
    MyReadData(&hdr.colormap_size, sizeof(hdr.colormap_size));
    MyReadData(&hdr.x_origin, sizeof(hdr.x_origin));
    MyReadData(&hdr.y_origin, sizeof(hdr.y_origin));
    MyReadData(&hdr.width, sizeof(hdr.width));
    MyReadData(&hdr.height, sizeof(hdr.height));
    MyReadData(&hdr.pixel_size, sizeof(hdr.pixel_size));
    MyReadData(&hdr.attributes, sizeof(hdr.attributes));

    if (hdr.image_type != NO_IMAGE &&
        hdr.image_type != UNCOMPRESSED_PALLETIZED &&
        hdr.image_type != UNCOMPRESSED_RGB &&
        hdr.image_type != UNCOMPRESSED_MONOCHROME &&
        hdr.image_type != RUNLENGTH_ENCODED_PALLETIZED &&
        hdr.image_type != RUNLENGTH_ENCODED_RGB &&
        hdr.image_type != COMPRESSED_MONOCHROME)
    {
        return E_FAIL;
    }

    if (hdr.image_type >= 9 
        && hdr.image_type <= 11)      // RLE image  not supported yet
    {
        return E_FAIL;
    }

    if (hdr.pixel_size != 32 
        && hdr.pixel_size != 24 
        && hdr.pixel_size != 8)
    {
        return E_FAIL;
    }

    //
    // Allocate memory for the bitmap
    //

    PBYTE pData = new BYTE[hdr.width * hdr.height * 4];
    pPalette = new TargaPalette[hdr.colormap_size];

    //
    // Load the pallet for palletized formats.
    //

    if (hdr.image_type == UNCOMPRESSED_PALLETIZED 
        || hdr.image_type == RUNLENGTH_ENCODED_PALLETIZED)
    {
        if (hdr.colormap_size == 15 
            || hdr.colormap_size == 16)
        {
            UINT     a, b;

            for (int i = 0; i < hdr.colormap_length; ++i)
            {
                MyReadData(&a, sizeof(a));
                MyReadData(&b, sizeof(b));
                pPalette[i].r = BYTE(a & 0x1F);
                pPalette[i].g = BYTE(((b & 0x03) << 3) | ((a & 0xE0) >> 5));
                pPalette[i].b = BYTE((b & 0x7C) >> 2);
                pPalette[i].a = 0;
            }
        }
        else if (hdr.colormap_length == 24)
        {
            for (int i = 0; i < hdr.colormap_length; ++i)
            {
                MyReadData(&pPalette[i].b, sizeof(pPalette[i].b));
                MyReadData(&pPalette[i].g, sizeof(pPalette[i].g));
                MyReadData(&pPalette[i].r, sizeof(pPalette[i].r));
                pPalette[i].a = 0;
            }
        }
        else if (hdr.colormap_size == 32)
        {
            for (int i = 0; i < hdr.colormap_length; ++i)
            {
                MyReadData(&pPalette[i].b, sizeof(pPalette[i].b));
                MyReadData(&pPalette[i].g, sizeof(pPalette[i].g));
                MyReadData(&pPalette[i].r, sizeof(pPalette[i].r));
                MyReadData(&pPalette[i].a, sizeof(pPalette[i].a));
            }
        }
    }
    else if (hdr.colormap_size == 0 
        && hdr.pixel_size == 8)   // grey-scale image
    {
        // if not platted, but 8 bpp
        // create greyscale identity palette
        for (UINT i = 0; i < 256; ++i)
        {
            pPalette[i].b = (BYTE)i;
            pPalette[i].g = (BYTE)i;
            pPalette[i].r = (BYTE)i;
            pPalette[i].a = (BYTE)i;
        }
    }

    if (hdr.image_type == UNCOMPRESSED_PALLETIZED 
        || hdr.image_type == UNCOMPRESSED_RGB 
        || hdr.image_type == UNCOMPRESSED_MONOCHROME)
    {
        if (hdr.pixel_size == 8)
        {
            hr = LoadRGB8(pFile, hdr, (BYTE *)pData, pPalette);
        }
        else if (hdr.pixel_size == 24)
        {
            hr = LoadRGB24(pFile, hdr, (BYTE *)pData);
        }
        else if (hdr.pixel_size == 32)
        {
            hr = LoadRGB32(pFile, hdr, (BYTE *)pData);
        }
    }

    if (pPalette)
    {
        delete[] pPalette;
    }

    if (FAILED(hr))
    {
        if (pData)
        {
            delete[] pData;
        }
    }
    else
    {
        *pRetWidth = hdr.width;
        *pRetHeight = hdr.height;
        *pRetData = pData;
    }

    return hr;
}


HRESULT 
BitmapDecode::LoadMono(
    PBYTE pFile, 
    BITMAPINFOHEADER& bmpHeader, 
    _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) 
    BYTE * imageBuffer, 
    RGBQUAD * pPalette)
{
    HRESULT hr = S_OK;
    DWORD   height = bmpHeader.biHeight;
    DWORD   width = bmpHeader.biWidth;
    DWORD   count = 0;
    DWORD   itter = 0;
    BYTE    alpha = 0;
    BYTE    byte = 0;

    for (INT y = height - 1; y >= 0; y--)
    {
        itter = (y * width * 4);
        for (UINT x = 0; x < width; x++, count += 4)
        {
            MyReadData(&byte, sizeof(byte));
            imageBuffer[itter++] = pPalette[byte ? 1 : 0].rgbRed;
            imageBuffer[itter++] = pPalette[byte ? 1 : 0].rgbGreen;
            imageBuffer[itter++] = pPalette[byte ? 1 : 0].rgbBlue;
            imageBuffer[itter++] = alpha;
        }

        // skip remaining bytes
        for (count = (width + 1) / 2; count % 4; count++)
        {
            MyReadData(&byte, sizeof(byte));
        }
    }
    return hr;
}


HRESULT 
BitmapDecode::LoadRGB4(
    PBYTE pFile, 
    BITMAPINFOHEADER& bmpHeader, 
    _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) 
    BYTE * imageBuffer, 
    RGBQUAD * pPalette)
{
    HRESULT hr = S_OK;

    DWORD   height = bmpHeader.biHeight;
    DWORD   width = bmpHeader.biWidth;
    DWORD   count = 0;
    DWORD   itter = 0;
    BYTE    alpha = 0;
    BYTE    byte = 0;

    for (INT y = height - 1; y >= 0; --y)
    {
        itter = (y * width * 4);

        for (UINT x = 0; x < width; x += 2, count += 4)
        {
            MyReadData(&byte, sizeof(byte));
            imageBuffer[itter++] = pPalette[byte >> 4].rgbRed;
            imageBuffer[itter++] = pPalette[byte >> 4].rgbGreen;
            imageBuffer[itter++] = pPalette[byte >> 4].rgbBlue;
            imageBuffer[itter++] = alpha;

            imageBuffer[itter++] = pPalette[byte & 0x0f].rgbRed;
            imageBuffer[itter++] = pPalette[byte & 0x0f].rgbGreen;
            imageBuffer[itter++] = pPalette[byte & 0x0f].rgbBlue;
            imageBuffer[itter++] = alpha;
        }

        // skip remaining bytes
        for (count = (width + 1) / 2; count % 4; count++)
        {
            MyReadData(&byte, sizeof(byte));
        }
    }
    return hr;
}


HRESULT
BitmapDecode::LoadRGB8(
    PBYTE pFile, 
    BITMAPINFOHEADER& bmpHeader, 
    _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) BYTE * imageBuffer, 
    RGBQUAD * pPalette)
{
    HRESULT hr = S_OK;

    DWORD   height = bmpHeader.biHeight;
    DWORD   width = bmpHeader.biWidth;
    DWORD   count = 0;
    DWORD   itter = 0;
    BYTE    alpha = 0;
    BYTE    byte = 0;

    for (INT y = height - 1; y >= 0; y--)
    {
        itter = (y * width * 4);

        for (UINT x = 0; x < width; x++, count += 4)
        {
            MyReadData(&byte, sizeof(byte));
            imageBuffer[itter++] = pPalette[byte].rgbRed;
            imageBuffer[itter++] = pPalette[byte].rgbGreen;
            imageBuffer[itter++] = pPalette[byte].rgbBlue;
            imageBuffer[itter++] = alpha;
        }

        // skip remaining bytes
        for (; count % 4; count++)    // skip remaining bytes
        {
            MyReadData(&byte, sizeof(byte));
        }
    }

    return hr;
}


HRESULT 
BitmapDecode::LoadRGB8(
    PBYTE pFile, 
    TargaHeader& Header, 
    _Out_writes_bytes_(Header.height*Header.width * 4) BYTE * imageBuffer, 
    TargaPalette * pPalette)
{
    HRESULT     hr = S_OK;

    UINT        height = Header.height;
    UINT        width = Header.width;
    UINT        count = 0;
    UINT        itter = 0;

    BYTE        byte = 0;
    BYTE        trash = 0;

    for (INT y = height - 1; y >= 0; y--)
    {
        itter = (y * width * 4);

        for (UINT x = 0; x < width; x++, count += 3)
        {
            MyReadData(&byte, sizeof(byte));
            imageBuffer[itter++] = pPalette[byte].r;
            imageBuffer[itter++] = pPalette[byte].g;
            imageBuffer[itter++] = pPalette[byte].b;
            imageBuffer[itter++] = pPalette[byte].a;
        }

        // skip remaining bytes
        for (; count % 4; count++)    // skip remaining bytes
        {
            MyReadData(&trash, sizeof(trash));
        }
    }

    return hr;
}


HRESULT 
BitmapDecode::LoadRGB24(
    PBYTE pFile, 
    BITMAPINFOHEADER& bmpHeader, 
    _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) BYTE * imageBuffer)
{
    HRESULT hr = S_OK;
    DWORD   height = bmpHeader.biHeight;
    DWORD   width = bmpHeader.biWidth;
    DWORD   count = 0;
    DWORD   itter = 0;

    BYTE    red = 0;
    BYTE    green = 0;
    BYTE    blue = 0;
    BYTE    alpha = 0;
    BYTE    trash = 0;

    for (INT y = height - 1; y >= 0; y--)
    {
        // 
        // BMP files are stored bottom to top,
        // so this code indexes into the proper
        // row in the buffer to display it right
        //

        itter = (y * width * 4);

        for (UINT x = count = 0; x < width; x++, count += 4)
        {
            MyReadData(&blue, sizeof(blue));
            MyReadData(&green, sizeof(green));
            MyReadData(&red, sizeof(red));

            // The layout of a BMP 24 is BGR not RGB
            imageBuffer[itter++] = red;
            imageBuffer[itter++] = green;
            imageBuffer[itter++] = blue;
            imageBuffer[itter++] = alpha;
        }

        for (; count % 4; count++)    // skip remaining bytes
        {
            MyReadData(&trash, sizeof(trash));
        }
    }

    return hr;
}


HRESULT 
BitmapDecode::LoadRGB24(
    PBYTE pFile, 
    TargaHeader& Header, 
    _Out_writes_bytes_(Header.height*Header.width * 4) BYTE * imageBuffer)
{
    HRESULT     hr = S_OK;

    UINT        height = Header.height;
    UINT        width = Header.width;
    UINT        itter = 0;

    BYTE        red = 0;
    BYTE        green = 0;
    BYTE        blue = 0;

    for (INT y = height - 1; y >= 0; y--)
    {
        // 
        // BMP files are stored bottom to top,
        // so this code indexes into the proper
        // row in the buffer to display it right
        //

        itter = (y * width * 4);

        for (UINT x = 0; x < width; x++)
        {
            MyReadData(&blue, sizeof(blue));
            MyReadData(&green, sizeof(green));
            MyReadData(&red, sizeof(red));

            // The layout of a TGA 24 is BGR not RGB
            imageBuffer[itter++] = red;
            imageBuffer[itter++] = green;
            imageBuffer[itter++] = blue;
            imageBuffer[itter++] = 0;
        }
    }

    return hr;
}


HRESULT 
BitmapDecode::LoadRGB32(
    PBYTE pFile, 
    BITMAPINFOHEADER& bmpHeader, 
    _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) BYTE * imageBuffer)
{
    HRESULT hr = S_OK;
    DWORD   height = bmpHeader.biHeight;
    DWORD   width = bmpHeader.biWidth;
    DWORD   count = 0;
    DWORD   itter = 0;

    BYTE    red = 0;
    BYTE    green = 0;
    BYTE    blue = 0;
    BYTE    alpha = 0;

    for (INT y = height - 1; y >= 0; y--)
    {
        //
        // BMP files are stored bottom to top,
        // so this code indexes into the proper
        // row in the buffer to display it right.
        //

        itter = (y * width * 3);

        for (UINT x = count = 0; x < width; x++, count += 3)
        {
            MyReadData(&blue, sizeof(blue));
            MyReadData(&green, sizeof(green));
            MyReadData(&red, sizeof(red));
            MyReadData(&alpha, sizeof(alpha));

            // The layout of a BMP is BGR not RGB
            imageBuffer[itter++] = red;
            imageBuffer[itter++] = green;
            imageBuffer[itter++] = blue;
            imageBuffer[itter++] = alpha;
        }
    }

    return hr;
}


HRESULT 
BitmapDecode::LoadRGB32(
    PBYTE pFile, 
    TargaHeader& Header, 
    _Out_writes_bytes_(Header.height*Header.width * 4) BYTE * imageBuffer)
{
    HRESULT     hr = S_OK;

    UINT        height = Header.height;
    UINT        width = Header.width;
    UINT        itter = 0;

    BYTE        red = 0;
    BYTE        green = 0;
    BYTE        blue = 0;
    BYTE        alpha = 0;

    for (INT y = height - 1; y >= 0; y--)
    {
        //
        // TGA files are stored bottom to top,
        // so this code indexes into the proper
        // row in the buffer to display it right
        //

        itter = (y * width * 4);

        for (UINT x = 0; x < width; x++)
        {
            MyReadData(&blue, sizeof(blue));
            MyReadData(&green, sizeof(green));
            MyReadData(&red, sizeof(red));
            MyReadData(&alpha, sizeof(alpha));

            // The layout of a TGA 32 is BGR not RGB
            imageBuffer[itter++] = red;
            imageBuffer[itter++] = green;
            imageBuffer[itter++] = blue;
            imageBuffer[itter++] = alpha;
        }
    }

    return hr;
}


UINT 
BitmapDecode::GetDXGIFormatTexelSize(
    DXGI_FORMAT Format)
{
    switch (Format)
    {
    case DXGI_FORMAT_UNKNOWN:
        return 0;
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        return sizeof(INT) * 4;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return sizeof(FLOAT) * 4;
    case DXGI_FORMAT_R32G32B32A32_UINT:
        return sizeof(UINT) * 4;
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return sizeof(INT) * 4;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        return sizeof(INT) * 3;
    case DXGI_FORMAT_R32G32B32_FLOAT:
        return sizeof(FLOAT) * 3;
    case DXGI_FORMAT_R32G32B32_UINT:
        return sizeof(UINT) * 3;
    case DXGI_FORMAT_R32G32B32_SINT:
        return sizeof(INT) * 3;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        return sizeof(INT) * 2;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return sizeof(FLOAT) * 2;
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        return sizeof(UINT) * 2;
    case DXGI_FORMAT_R16G16B16A16_UINT:
        return sizeof(UINT) * 2;
    case DXGI_FORMAT_R16G16B16A16_SNORM:
        return sizeof(INT) * 2;
    case DXGI_FORMAT_R16G16B16A16_SINT:
        return sizeof(INT) * 2;
    case DXGI_FORMAT_R32G32_TYPELESS:
        return sizeof(INT) * 2;
    case DXGI_FORMAT_R32G32_FLOAT:
        return sizeof(FLOAT) * 2;
    case DXGI_FORMAT_R32G32_UINT:
        return sizeof(UINT) * 2;
    case DXGI_FORMAT_R32G32_SINT:
        return sizeof(INT) * 2;
    case DXGI_FORMAT_R32G8X24_TYPELESS:
        return sizeof(INT) * 2;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return sizeof(FLOAT) * 2;
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        return sizeof(INT) * 2;
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return sizeof(UINT) * 2;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        return sizeof(INT);
    case DXGI_FORMAT_R10G10B10A2_UNORM:
        return sizeof(UINT);
    case DXGI_FORMAT_R10G10B10A2_UINT:
        return sizeof(UINT);
    case DXGI_FORMAT_R11G11B10_FLOAT:
        return sizeof(FLOAT);
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        return sizeof(INT);
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return sizeof(INT);
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return sizeof(UINT);
    case DXGI_FORMAT_R8G8B8A8_UINT:
        return sizeof(UINT);
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        return sizeof(INT);
    case DXGI_FORMAT_R8G8B8A8_SINT:
        return sizeof(UINT);
    case DXGI_FORMAT_R16G16_TYPELESS:
        return sizeof(UINT);
    case DXGI_FORMAT_R16G16_FLOAT:
        return sizeof(FLOAT);
    case DXGI_FORMAT_R16G16_UNORM:
        return sizeof(UINT);
    case DXGI_FORMAT_R16G16_UINT:
        return sizeof(UINT);
    case DXGI_FORMAT_R16G16_SNORM:
        return sizeof(INT);
    case DXGI_FORMAT_R16G16_SINT:
        return sizeof(INT);
    case DXGI_FORMAT_R32_TYPELESS:
        return sizeof(INT);
    case DXGI_FORMAT_D32_FLOAT:
        return sizeof(FLOAT);
    case DXGI_FORMAT_R32_FLOAT:
        return sizeof(FLOAT);
    case DXGI_FORMAT_R32_UINT:
        return sizeof(UINT);
    case DXGI_FORMAT_R32_SINT:
        return sizeof(INT);
    case DXGI_FORMAT_R24G8_TYPELESS:
        return sizeof(INT);
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return sizeof(UINT);
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        return sizeof(INT);
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return sizeof(UINT);
    case DXGI_FORMAT_R8G8_TYPELESS:
        return sizeof(WORD);
    case DXGI_FORMAT_R8G8_UNORM:
        return sizeof(WORD);
    case DXGI_FORMAT_R8G8_UINT:
        return sizeof(WORD);
    case DXGI_FORMAT_R8G8_SNORM:
        return sizeof(WORD);
    case DXGI_FORMAT_R8G8_SINT:
        return sizeof(WORD);
    case DXGI_FORMAT_R16_TYPELESS:
        return sizeof(WORD);
    case DXGI_FORMAT_R16_FLOAT:
        return sizeof(FLOAT) / 2;
    case DXGI_FORMAT_D16_UNORM:
        return sizeof(WORD);
    case DXGI_FORMAT_R16_UNORM:
        return sizeof(WORD);
    case DXGI_FORMAT_R16_UINT:
        return sizeof(WORD);
    case DXGI_FORMAT_R16_SNORM:
        return sizeof(WORD);
    case DXGI_FORMAT_R16_SINT:
        return sizeof(WORD);
    case DXGI_FORMAT_R8_TYPELESS:
        return sizeof(BYTE);
    case DXGI_FORMAT_R8_UNORM:
        return sizeof(BYTE);
    case DXGI_FORMAT_R8_UINT:
        return sizeof(BYTE);
    case DXGI_FORMAT_R8_SNORM:
        return sizeof(CHAR);
    case DXGI_FORMAT_R8_SINT:
        return sizeof(CHAR);
    case DXGI_FORMAT_A8_UNORM:
        return sizeof(BYTE);
    case DXGI_FORMAT_R1_UNORM:
        return 1;
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        return sizeof(FLOAT);
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        return sizeof(UINT);
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        return sizeof(UINT);
    
    // Compressed fornmats not supported
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    default:
        return 0;
    }
}