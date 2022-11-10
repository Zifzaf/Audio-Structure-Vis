/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#include <cstring>

namespace BinaryData
{

//================== README.txt ==================
static const unsigned char temp_binary_data_0[] =
"Code for reading/writing wav and reading flac and mp3 files.\n"
"\n"
"These 3 files are released to public domain by the author:\n"
"dr_flac.h\n"
"dr_wav.h\n"
"dr_mp3.h\n"
"\n"
"See also information in these files.\n"
"\n"
"https://github.com/mackron/dr_libs\n";

const char* README_txt = (const char*) temp_binary_data_0;

//================== specializations.i ==================
static const unsigned char temp_binary_data_1[] =
"/**\n"
" * Copyright (C) 2016-2022 Fractalium Ltd (http://www.kfrlib.com)\n"
" * This file is part of KFR\n"
" *\n"
" * KFR is free software: you can redistribute it and/or modify\n"
" * it under the terms of the GNU General Public License as published by\n"
" * the Free Software Foundation, either version 2 of the License, or\n"
" * (at your option) any later version.\n"
" *\n"
" * KFR is distributed in the hope that it will be useful,\n"
" * but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
" * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
" * GNU General Public License for more details.\n"
" *\n"
" * You should have received a copy of the GNU General Public License\n"
" * along with KFR.\n"
" */\n"
"#pragma once\n"
"\n"
"#include \"../vec.hpp\"\n"
"#ifndef KFR_SHUFFLE_SPECIALIZATIONS\n"
"#include \"../shuffle.hpp\"\n"
"#endif\n"
"\n"
"#ifdef KFR_COMPILER_GNU\n"
"\n"
"namespace kfr\n"
"{\n"
"inline namespace CMT_ARCH_NAME\n"
"{\n"
"\n"
"namespace intrinsics\n"
"{\n"
"template <>\n"
"inline vec<f32, 32> shufflevector<f32, 32>(\n"
"    csizes_t<0, 1, 8, 9, 16, 17, 24, 25, 2, 3, 10, 11, 18, 19, 26, 27, 4, 5, 12, 13, 20, 21, 28, 29, 6, 7, 14,\n"
"             15, 22, 23, 30, 31>,\n"
"    const vec<f32, 32>& x, const vec<f32, 32>&)\n"
"{\n"
"    f32x32 w = x;\n"
"\n"
"    w = concat(permute<0, 1, 8, 9, 4, 5, 12, 13, 2, 3, 10, 11, 6, 7, 14, 15>(low(w)),\n"
"               permute<0, 1, 8, 9, 4, 5, 12, 13, 2, 3, 10, 11, 6, 7, 14, 15>(high(w)));\n"
"\n"
"    w = permutegroups<(4), 0, 4, 2, 6, 1, 5, 3, 7>(w); // avx: vperm2f128 & vinsertf128, sse: no-op\n"
"    return w;\n"
"}\n"
"\n"
"template <>\n"
"inline vec<f32, 32> shufflevector<f32, 32>(\n"
"    csizes_t<0, 1, 16, 17, 8, 9, 24, 25, 4, 5, 20, 21, 12, 13, 28, 29, 2, 3, 18, 19, 10, 11, 26, 27, 6, 7, 22,\n"
"             23, 14, 15, 30, 31>,\n"
"    const vec<f32, 32>& x, const vec<f32, 32>&)\n"
"{\n"
"    f32x32 w = x;\n"
"\n"
"    w = concat(permute<0, 1, 8, 9, 4, 5, 12, 13, /**/ 2, 3, 10, 11, 6, 7, 14, 15>(even<8>(w)),\n"
"               permute<0, 1, 8, 9, 4, 5, 12, 13, /**/ 2, 3, 10, 11, 6, 7, 14, 15>(odd<8>(w)));\n"
"\n"
"    w = permutegroups<(4), 0, 4, 1, 5, 2, 6, 3, 7>(w); // avx: vperm2f128 & vinsertf128, sse: no-op\n"
"    return w;\n"
"}\n"
"\n"
"inline vec<f32, 32> bitreverse_2(const vec<f32, 32>& x)\n"
"{\n"
"    return shufflevector<f32, 32>(csizes<0, 1, 16, 17, 8, 9, 24, 25, 4, 5, 20, 21, 12, 13, 28, 29, 2, 3, 18,\n"
"                                         19, 10, 11, 26, 27, 6, 7, 22, 23, 14, 15, 30, 31>,\n"
"                                  x, x);\n"
"}\n"
"\n"
"template <>\n"
"inline vec<f32, 64> shufflevector<f32, 64>(\n"
"    csizes_t<0, 1, 32, 33, 16, 17, 48, 49, 8, 9, 40, 41, 24, 25, 56, 57, 4, 5, 36, 37, 20, 21, 52, 53, 12, 13,\n"
"             44, 45, 28, 29, 60, 61, 2, 3, 34, 35, 18, 19, 50, 51, 10, 11, 42, 43, 26, 27, 58, 59, 6, 7, 38,\n"
"             39, 22, 23, 54, 55, 14, 15, 46, 47, 30, 31, 62, 63>,\n"
"    const vec<f32, 64>& x, const vec<f32, 64>&)\n"
"{\n"
"    return permutegroups<(8), 0, 4, 1, 5, 2, 6, 3, 7>(\n"
"        concat(bitreverse_2(even<8>(x)), bitreverse_2(odd<8>(x))));\n"
"}\n"
"\n"
"template <>\n"
"inline vec<f32, 16> shufflevector<f32, 16>(csizes_t<0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15>,\n"
"                                           const vec<f32, 16>& x, const vec<f32, 16>&)\n"
"{\n"
"    //    asm volatile(\"int $3\");\n"
"    const vec<f32, 16> xx = permutegroups<(4), 0, 2, 1, 3>(x);\n"
"\n"
"    return concat(shuffle<0, 2, 8 + 0, 8 + 2>(low(xx), high(xx)),\n"
"                  shuffle<1, 3, 8 + 1, 8 + 3>(low(xx), high(xx)));\n"
"}\n"
"\n"
"template <>\n"
"inline vec<f32, 16> shufflevector<f32, 16>(csizes_t<0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15>,\n"
"                                           const vec<f32, 16>& x, const vec<f32, 16>&)\n"
"{\n"
"    const vec<f32, 16> xx =\n"
"        concat(shuffle<0, 8 + 0, 1, 8 + 1>(low(x), high(x)), shuffle<2, 8 + 2, 3, 8 + 3>(low(x), high(x)));\n"
"\n"
"    return permutegroups<(4), 0, 2, 1, 3>(xx);\n"
"}\n"
"\n"
"template <>\n"
"inline vec<f32, 32> shufflevector<f32, 32>(\n"
"    csizes_t<0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23, 8, 24, 9, 25, 10, 26, 11, 27, 12, 28, 13,\n"
"             29, 14, 30, 15, 31>,\n"
"    const vec<f32, 32>& x, const vec<f32, 32>&)\n"
"{\n"
"    const vec<f32, 32> xx = permutegroups<(8), 0, 2, 1, 3>(x);\n"
"\n"
"    return concat(interleavehalves(low(xx)), interleavehalves(high(xx)));\n"
"}\n"
"} // namespace intrinsics\n"
"} // namespace CMT_ARCH_NAME\n"
"} // namespace kfr\n"
"#endif\n";

const char* specializations_i = (const char*) temp_binary_data_1;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes);
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x2aaab85f:  numBytes = 223; return README_txt;
        case 0x2fdc5a32:  numBytes = 4153; return specializations_i;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "README_txt",
    "specializations_i"
};

const char* originalFilenames[] =
{
    "README.txt",
    "specializations.i"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
        if (strcmp (namedResourceList[i], resourceNameUTF8) == 0)
            return originalFilenames[i];

    return nullptr;
}

}
