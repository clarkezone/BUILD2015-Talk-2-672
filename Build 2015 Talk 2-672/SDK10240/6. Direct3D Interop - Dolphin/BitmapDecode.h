//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once


class BitmapDecode
{
private:
    BitmapDecode();

public:
    static HRESULT LoadBMP(
        PBYTE pFile, 
        ULONG * pRetWidth, 
        ULONG * pRetHeight, 
        PBYTE * pRetData);

    static HRESULT LoadTGA(
        PBYTE pFile, 
        ULONG * pRetWidth, 
        ULONG * pRetHeight, 
        PBYTE * pRetData);

    static UINT GetDXGIFormatTexelSize(DXGI_FORMAT Format);

public:
    enum TARGA_IMAGE_TYPE {
        NO_IMAGE = 0,                       // No Image included
        UNCOMPRESSED_PALLETIZED = 1,        // Uncompressed, color-mapped images
        UNCOMPRESSED_RGB = 2,               // Uncompressed, RGB images
        UNCOMPRESSED_MONOCHROME = 3,        // Uncompressed, black and white images
        RUNLENGTH_ENCODED_PALLETIZED = 9,   // Runlength encoded color-mapped images
        RUNLENGTH_ENCODED_RGB = 10,         // Runlength encoded RGB images
        COMPRESSED_MONOCHROME = 11,         // Compressed, black and white images
        COMPRESSED_PALLETIZED = 32,         // Compressed color-mapped data, using Huffman, Delta, and runlength encoding
        COMPRESSED_PALLETIZED_4_PASS = 33,  // COMPRESSED_PALLETIZED w/ 4-pass quadtree 
    };

    struct TargaHeader 
    {
        BYTE        id_length;
        BYTE        colormap_type;
        BYTE        image_type;
        USHORT      colormap_index;
        USHORT      colormap_length;
        BYTE        colormap_size;
        USHORT      x_origin;
        USHORT      y_origin;
        USHORT      width;
        USHORT      height;
        BYTE        pixel_size;
        BYTE        attributes;
    };

    struct TargaPalette 
    {
        BYTE b;
        BYTE g;
        BYTE r;
        BYTE a;
    };

private:
    static HRESULT LoadMono(
        PBYTE pFile, 
        BITMAPINFOHEADER& bmpHeader, 
        _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) BYTE * imageBuffer, 
        RGBQUAD * pPalette);

    static HRESULT LoadRGB4(
        PBYTE pFile, 
        BITMAPINFOHEADER& bmpHeader, 
        _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) BYTE * imageBuffer, 
        RGBQUAD * pPalette);

    static HRESULT LoadRGB8(
        PBYTE pFile, BITMAPINFOHEADER& bmpHeader, 
        _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) BYTE * imageBuffer, 
        RGBQUAD * pPalette);

    static HRESULT LoadRGB8(
        PBYTE pFile,
        TargaHeader& Header,
        _Out_writes_bytes_(Header.height*Header.width * 4) BYTE * imageBuffer,
        TargaPalette * pPalette);

    static HRESULT LoadRGB24(
        PBYTE pFile, 
        BITMAPINFOHEADER& bmpHeader, 
        _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) BYTE * imageBuffer);

    static HRESULT LoadRGB24(
        PBYTE pFile,
        TargaHeader& Header,
        _Out_writes_bytes_(Header.height*Header.width * 4)
        BYTE* imageBuffer);

    static HRESULT LoadRGB32(
        PBYTE pFile, 
        BITMAPINFOHEADER& bmpHeader, 
        _Out_writes_bytes_(bmpHeader.biHeight * bmpHeader.biWidth * 4) BYTE * imageBuffer);

    static HRESULT LoadRGB32(
        PBYTE pFile,
        TargaHeader& Header,
        _Out_writes_bytes_(Header.height*Header.width * 4) BYTE * imageBuffer);
};
