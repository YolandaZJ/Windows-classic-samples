//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************


#include "stdafx.h"
#include "CustomFontSetManager.h"


namespace DWriteCustomFontSets
{
    // Static list of app-defined local fonts, with properties.
    AppFontInfo const g_appFonts[] =
    {
        // familyName,       fullName,         fontWeight,  fontRelativeLocation (local file path relative to app),  TTC index
        {  L"IconFont",      L"IconFont",      L"400",      L"Fonts\\Symbols.ttf",                                   0         },
        {  L"BodyTextFont",  L"BodyTextFont",  L"350",      L"Fonts\\selawksl.ttf",                                  0         },
        {  L"HeadingFont",   L"HeadingFont",   L"600",      L"Fonts\\selawksb.ttf",                                  0         },
        {  L"CaptionFont",   L"CaptionFont",   L"400",      L"Fonts\\Selwk.ttc",                                     0         },
        {  L"BannerFont",    L"BannerFont",    L"300",      L"Fonts\\Selwk.ttc",                                     3         }
    };

    uint32_t const g_appFontsCount = ARRAYSIZE(g_appFonts);


    // Static list of app-defined remote (Web) fonts, with properties.
    const wchar_t* g_remoteFontBaseUrl = L"https://github.com/"; // When concatenated with a relative URL, must form the complete URL.

    AppFontInfo const g_remoteFonts[] =
    {
        // familyName,       fullName,         fontWeight,  fontRelativeLocation (URL relative to g_remoteFontBaseUrl),   fontIndex
        {  L"IconFont",      L"IconFont",      L"400",      L"winjs/winjs/blob/master/src/fonts/Symbols.ttf?raw=true",    0         },
        {  L"BodyTextFont",  L"BodyTextFont",  L"400",      L"winjs/winstrap/blob/master/src/fonts/selawk.ttf?raw=true",  0         }
    };

    uint32_t const g_remoteFontsCount = ARRAYSIZE(g_remoteFonts);



    // Static packed font data, in WOFF2 format:
    // For purposes of demonstration, we'll use statically-defined data. Actual app scenarios
    // might include obtaining WOFF or WOFF2 font data bundled with an app or embedded within an
    // app binary, or obtained from a non-public Web service that requires special interaction
    // not supported directly by DirectWrite.
    uint8_t const g_packedFontData1[] = {
        0x77, 0x4F, 0x46, 0x32,   0x00, 0x01, 0x00, 0x00,   0x00, 0x00, 0x03, 0xA4,   0x00, 0x0A, 0x00, 0x00,
        0x00, 0x00, 0x07, 0x4C,   0x00, 0x00, 0x03, 0x59,   0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,
        0x06, 0x60, 0x00, 0x4C,   0x0A, 0x82, 0x78, 0x82,   0x47, 0x01, 0x36, 0x02,   0x24, 0x03, 0x16, 0x0B,
        0x0E, 0x00, 0x04, 0x20,   0x05, 0x87, 0x38, 0x07,   0x20, 0x1B, 0x5A, 0x06,   0x00, 0x06, 0x1E, 0xE8,
        0xBD, 0xEC, 0xCD, 0x24,   0xE9, 0xA6, 0xD9, 0x1A,   0x69, 0x7D, 0x02, 0x4B,   0x1E, 0x44, 0xEC, 0x9E,
        0xF1, 0x0F, 0xE7, 0xEA,   0x52, 0xBE, 0x09, 0x48,   0x5E, 0xE0, 0x40, 0x64,   0xFF, 0xFB, 0x7E, 0xD5,
        0x6C, 0x0A, 0xD5, 0x42,   0xB5, 0x03, 0xBE, 0x68,   0x0A, 0x77, 0x8A, 0x76,   0x8E, 0x48, 0x38, 0x18,
        0x8A, 0x87, 0x34, 0xBA,   0x96, 0x78, 0xC2, 0xA3,   0x5F, 0xAB, 0x0A, 0x21,   0x40, 0xB7, 0x33, 0x50,
        0xD4, 0x42, 0x31, 0x17,   0x0D, 0x5D, 0x5B, 0xAE,   0x55, 0x02, 0x11, 0xBA,   0xFE, 0xB6, 0x72, 0x8F,
        0xCE, 0xD2, 0x47, 0xD7,   0xA4, 0x5D, 0x93, 0xAA,   0x54, 0xA5, 0x54, 0x00,   0xC1, 0x74, 0x4D, 0x9B,
        0x24, 0xD3, 0xA4, 0x3E,   0x26, 0xE7, 0xB4, 0x07,   0xFF, 0x3B, 0x08, 0x77,   0x7D, 0xBD, 0xFC, 0x2B,
        0x49, 0xA3, 0xA2, 0x2D,   0x1F, 0x68, 0x1A, 0x93,   0x40, 0xB9, 0xC9, 0xBA,   0xA1, 0x52, 0x5F, 0x4E,
        0xEB, 0x48, 0x54, 0x4C,   0x62, 0xA9, 0x40, 0x03,   0xCE, 0x59, 0x96, 0xBD,   0x7A, 0xF2, 0x06, 0xEB,
        0x8D, 0xF5, 0xC6, 0x7A,   0x60, 0x9D, 0xB3, 0x1A,   0x46, 0x15, 0x35, 0xE0,   0xA2, 0xBB, 0x09, 0x73,
        0x65, 0x45, 0xD6, 0x96,   0x5B, 0x47, 0xB5, 0x43,   0x19, 0xF5, 0x77, 0x14,   0xDD, 0x50, 0x75, 0xC3,
        0x16, 0x6F, 0xB3, 0x8D,   0xF0, 0x48, 0x37, 0x46,   0xD8, 0x87, 0x19, 0x99,   0xDD, 0x17, 0xAF, 0x0F,
        0x31, 0x16, 0x3E, 0xF4,   0xCB, 0x1E, 0x61, 0xA4,   0xD9, 0x87, 0xDE, 0xFC,   0xC5, 0xF2, 0x53, 0x8B,
        0xEC, 0xC3, 0x8C, 0x3F,   0x0E, 0x07, 0x63, 0x1F,   0x26, 0x63, 0x08, 0xE3,   0xA1, 0x33, 0x35, 0xCD,
        0xE1, 0xB0, 0xDB, 0x35,   0xA7, 0xD3, 0x29, 0xC6,   0x3D, 0x4A, 0x13, 0x76,   0x1C, 0x29, 0xC9, 0xF1,
        0xD7, 0x9D, 0xA9, 0xF2,   0xAD, 0x23, 0x45, 0x79,   0xF8, 0x90, 0x5F, 0x8F,   0xC4, 0x38, 0x67, 0xAA,
        0xAC, 0x5B, 0x50, 0x7D,   0x7D, 0xDF, 0x6A, 0x27,   0xA2, 0x3B, 0x2D, 0xDF,   0x1D, 0x3D, 0x69, 0xF9,
        0xF1, 0x1D, 0xB7, 0x95,   0x93, 0xF5, 0x1E, 0x37,   0x55, 0x23, 0xBC, 0x9B,   0x55, 0xFF, 0x7B, 0x92,
        0x72, 0x2E, 0xD9, 0xFB,   0x5E, 0x5E, 0x7A, 0x2C,   0xB4, 0xAF, 0xEB, 0xFB,   0x1A, 0xBE, 0x76, 0xF4,
        0xC0, 0x7F, 0x65, 0x8A,   0x62, 0x66, 0xCF, 0x98,   0x37, 0x00, 0xFF, 0x13,   0xBB, 0x29, 0xBB, 0x40,
        0x2E, 0x29, 0xEC, 0xBC,   0x61, 0xD8, 0xDA, 0x33,   0x37, 0xF6, 0x06, 0xDD,   0xEE, 0x7E, 0x59, 0xED,
        0xCE, 0xE9, 0x8A, 0xC3,   0x6B, 0xD6, 0x5F, 0xD8,   0xB7, 0xC6, 0xFA, 0xD2,   0xED, 0x55, 0xDF, 0xA1,
        0xE5, 0xE7, 0x75, 0xEC,   0xB4, 0x64, 0xC8, 0xC6,   0x4B, 0x07, 0xB7, 0xFB,   0xEC, 0xEF, 0xBE, 0x6B,
        0x93, 0x5C, 0x5B, 0xAF,   0xFE, 0xCA, 0xA9, 0xCE,   0xCE, 0xC1, 0xCD, 0xB8,   0x5D, 0xFE, 0x4C, 0xA7,
        0xC2, 0xFC, 0x32, 0x4B,   0x33, 0x3D, 0x7A, 0x64,   0x76, 0x08, 0xAE, 0x34,   0x37, 0x6B, 0x78, 0x5A,
        0x46, 0x41, 0xAF, 0xAC,   0x52, 0x4F, 0x01, 0x40,   0x00, 0x7C, 0xEC, 0xDC,   0xD6, 0xBE, 0x32, 0x25,
        0xCF, 0xB3, 0xF6, 0x37,   0x14, 0x1D, 0x46, 0x86,   0x7B, 0x8D, 0x4F, 0x26,   0x6D, 0x36, 0x3E, 0x7C,
        0xD0, 0xB7, 0x0E, 0x41,   0x56, 0x39, 0xCD, 0xCF,   0x16, 0x3D, 0xE2, 0xD8,   0x90, 0xAD, 0xFC, 0x34,
        0xFA, 0xFA, 0x7F, 0xFF,   0x41, 0x2B, 0x49, 0x10,   0xFE, 0x9A, 0x5F, 0x83,   0x82, 0x9A, 0x56, 0xD2,
        0x93, 0x15, 0x34, 0x54,   0x82, 0xA0, 0x77, 0x95,   0xDA, 0x1E, 0x07, 0xC5,   0xB6, 0x05, 0x17, 0x2A,
        0xA3, 0x60, 0xF5, 0x9F,   0x42, 0x6E, 0x2C, 0x6E,   0x5B, 0x8B, 0xEA, 0x0A,   0x22, 0x17, 0xF6, 0x5C,
        0xE0, 0x25, 0x72, 0xB9,   0xC4, 0x43, 0xF4, 0xE6,   0x0A, 0xA6, 0x18, 0x39,   0xAD, 0xDA, 0x71, 0x8D,
        0x20, 0xB1, 0x72, 0xDA,   0xE5, 0x8E, 0xDB, 0x18,   0x2C, 0x0E, 0x72, 0x9D,   0x92, 0x78, 0x70, 0x57,
        0xFC, 0x64, 0x49, 0xEE,   0xCE, 0x52, 0x59, 0xF7,   0xD9, 0x1E, 0xB4, 0x96,   0x4E, 0xB6, 0x61, 0x52,
        0x91, 0xF2, 0x54, 0xA0,   0x1A, 0x26, 0x6D, 0xE8,   0x46, 0x11, 0xFD, 0xE9,   0xC3, 0x00, 0xFA, 0xD0,
        0x99, 0x81, 0x98, 0x34,   0xA2, 0x0F, 0xFD, 0xE9,   0xBB, 0xAA, 0x2D, 0x60,   0x20, 0xDD, 0xE8, 0x43,
        0xEF, 0x41, 0x89, 0x0C,   0x99, 0xFD, 0xCB, 0xD2,   0xCB, 0x53, 0x28, 0x4B,   0xD1, 0x63, 0xBD, 0x28,
        0x41, 0x69, 0x4C, 0x86,   0xD0, 0x8D, 0x81, 0x74,   0xC5, 0xA4, 0x23, 0x9D,   0x18, 0x40, 0x27, 0xFA,
        0x33, 0x58, 0xB3, 0xC5,   0x98, 0x34, 0x45, 0x84,   0x58, 0xDB, 0xDA, 0x52,   0x40, 0x2F, 0x3A, 0x61,
        0x92, 0x24, 0xD4, 0x93,   0x02, 0x5F, 0xFF, 0x1E,   0x94, 0x3D, 0xD5, 0x4D,   0x61, 0x03, 0xB2, 0x05,
        0x98, 0x0C, 0x64, 0xA6,   0x17, 0x6B, 0xB6, 0x17,   0x05, 0xF4, 0xB7, 0x00,   0x1E, 0x63, 0xCC, 0x07,
        0xBB, 0xAD, 0x80, 0x27,   0x70, 0xA0, 0x14, 0x7A,   0xB3, 0x96, 0x58, 0x52,   0x12, 0x03, 0xED, 0xAD,
        0x4E, 0x71, 0xB3, 0xBD,   0x29, 0xA6, 0x1C, 0x0A,   0x44, 0x93, 0xFC, 0x60,   0x0F, 0x59, 0xE3, 0xA0,
        0x7C, 0xA8, 0x3F, 0xDD,   0x3C, 0xAC, 0xB2, 0xDC,   0x1B, 0x87, 0xD0, 0x83,   0x24, 0x06, 0x51, 0x58,
        0x03, 0xEA, 0xC2, 0x20,   0x9D, 0xDF, 0xDF, 0x62,   0x9E, 0xF9, 0x60, 0x2A,   0xBA, 0x33, 0x80, 0xE9,
        0xFA, 0x97, 0xA7, 0x2C,   0x35, 0x28, 0x5F, 0x11,   0x25, 0xBF, 0x81, 0x95,   0xC1, 0x2E, 0x67, 0xB1,
        0x41, 0xF4, 0xF5, 0x90,   0x3A, 0x88, 0x7D, 0x33,   0x56, 0xF2, 0xC1, 0x06,   0xA8, 0x12, 0x1B, 0x6C,
        0xC8, 0x22, 0xDF, 0x93,   0x64, 0xBA, 0x32, 0xE8,   0xEA, 0xA5, 0x95, 0x84,   0x8D, 0x43, 0xD4, 0xE9,
        0xC4, 0x11, 0x7B, 0x46,   0x17, 0x3A, 0xD1, 0x5B,   0xAA, 0x2E, 0x54, 0xCC,   0x2A, 0xAE, 0xA0, 0xC2,
        0x31, 0x9D, 0x44, 0x0B,   0x5A, 0x2B, 0xB5, 0xA3,   0xAF, 0x7D, 0x0F, 0xFA,   0x6C, 0x6B, 0x14, 0xF8,
        0xBA, 0xFA, 0x56, 0x89,   0x55, 0x80, 0x77, 0xBA,   0x2B, 0x03, 0x19, 0xA8,   0x54, 0x93, 0x72, 0xFD,
        0x92, 0xF0, 0x20, 0x6B,   0x7D, 0x90, 0x00, 0x0E,   0x96, 0xED, 0xA2, 0x5F,   0x17, 0xCA, 0x71, 0x3B,
        0x36, 0x80, 0x72, 0xB4,   0xA3, 0x29, 0xAD, 0x29,   0x83, 0x28, 0xFD, 0xA6,   0x17, 0xC2, 0x3A, 0x44,
        0x45, 0x26, 0x00, 0x00
        };

    PackedFontInfo const g_packedFonts[] =
    {
        // fontData,          fontDataSize,              fileContainer
        {  g_packedFontData1, sizeof(g_packedFontData1), DWRITE_CONTAINER_TYPE_WOFF2 }
    };

    uint32_t const g_packedFontsCount = ARRAYSIZE(g_packedFonts);


} // namespace DWriteCustomFontSets