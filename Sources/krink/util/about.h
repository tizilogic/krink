#pragma once

#include <string.h>

/*! \file about.h
    \brief Provides license text from 3rd party code used in krink.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kr_3rd_party_license {
    KR_3RD_PARTY_KINC = 0,
    KR_3RD_PARTY_FLECS,
    KR_3RD_PARTY_STB_TRUE_TYPE,
    KR_3RD_PARTY_TWEEN,
    KR_3RD_PARTY_TLSF,
    KR_3RD_PARTY_NUKLEAR,
    KR_3RD_PARTY_TOTAL
} kr_3rd_party_license_t;

char all_licenses[8192] = {0};

/*! \brief Contains all licenses of 3rd party dependencies of krink.
*/
const char *kr_3rd_party_licenses[] = {
    "Kinc - Modern low level game library and hardware abstraction.\n"
    "\n"
    "Copyright (c) 2017 the Kore Development Team\n"
    "\n"
    "This software is provided 'as-is', without any express or implied\n"
    "warranty. In no event will the authors be held liable for any damages\n"
    "arising from the use of this software.\n"
    "\n"
    "Permission is granted to anyone to use this software for any purpose,\n"
    "including commercial applications, and to alter it and redistribute it\n"
    "freely, subject to the following restrictions:\n"
    "\n"
    "  1. The origin of this software must not be misrepresented; you must not\n"
    "     claim that you wrote the original software. If you use this software\n"
    "     in a product, an acknowledgment in the product documentation would be\n"
    "     appreciated but is not required.\n"
    "\n"
    "  2. Altered source versions must be plainly marked as such, and must not be\n"
    "     misrepresented as being the original software.\n"
    "\n"
    "  3. This notice may not be removed or altered from any source distribution.\n",

    "flecs - A fast entity component system (ECS) for C & C++\n"
    "\n"
    "MIT License\n"
    "\n"
    "Copyright (c) 2019 Sander Mertens\n"
    "\n"
    "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
    "of this software and associated documentation files (the \"Software\"), to deal\n"
    "in the Software without restriction, including without limitation the rights\n"
    "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
    "copies of the Software, and to permit persons to whom the Software is\n"
    "furnished to do so, subject to the following conditions:\n"
    "\n"
    "The above copyright notice and this permission notice shall be included in all\n"
    "copies or substantial portions of the Software.\n"
    "\n"
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
    "SOFTWARE.\n",

    "stb_truetype\n"
    "\n"
    "MIT License\n"
    "\n"
    "Copyright (c) 2017 Sean Barrett\n"
    "\n"
    "Permission is hereby granted, free of charge, to any person obtaining a copy of\n"
    "this software and associated documentation files (the \" Software \"), to deal in\n"
    "the Software without restriction, including without limitation the rights to\n"
    "use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies\n"
    "of the Software, and to permit persons to whom the Software is furnished to do\n"
    "so, subject to the following conditions:\n"
    "The above copyright notice and this permission notice shall be included in all\n"
    "copies or substantial portions of the Software.\n"
    "THE SOFTWARE IS PROVIDED \" AS IS \", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
    "SOFTWARE.\n",

    "Port of Tween.hx (armory3d/iron) - By Lubos Lenco\n"
    "\n"
    "The zlib/libpng License\n"
    "\n"
    "This software is provided 'as-is', without any express or implied warranty.\n"
    "In no event will the authors be held liable for any damages arising from\n"
    "the use of this software. Permission is granted to anyone to use this software\n"
    "for any purpose, including commercial applications, and to alter it and redistribute\n"
    "it freely, subject to the following restrictions:\n"
    "    * The origin of this software must not be misrepresented;\n"
    "      you must not claim that you wrote the original software.\n"
    "    * If you use this software in a product, an acknowledgment in\n"
    "      the product documentation would be appreciated but is not required.\n"
    "    * Altered source versions must be plainly marked as such, and must\n"
    "      not be misrepresented as being the original software.\n"
    "    * This notice may not be removed or altered from any source distribution.\n",

    "Two Level Segregated Fit memory allocator, version 3.1.\n"
    "Written by Matthew Conte\n"
    "    http://tlsf.baisoku.org\n"
    "\n"
    "Based on the original documentation by Miguel Masmano:\n"
    "    http://www.gii.upv.es/tlsf/main/docs\n"
    "\n"
    "This implementation was written to the specification\n"
    "of the document, therefore no GPL restrictions apply.\n"
    "\n"
    "Copyright (c) 2006-2016, Matthew Conte\n"
    "All rights reserved.\n"
    "\n"
    "Redistribution and use in source and binary forms, with or without\n"
    "modification, are permitted provided that the following conditions are met:\n"
    "    * Redistributions of source code must retain the above copyright\n"
    "      notice, this list of conditions and the following disclaimer.\n"
    "    * Redistributions in binary form must reproduce the above copyright\n"
    "      notice, this list of conditions and the following disclaimer in the\n"
    "      documentation and/or other materials provided with the distribution.\n"
    "    * Neither the name of the copyright holder nor the\n"
    "      names of its contributors may be used to endorse or promote products\n"
    "      derived from this software without specific prior written permission.\n"
    "\n"
    "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\n"
    "ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n"
    "WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
    "DISCLAIMED. IN NO EVENT SHALL MATTHEW CONTE BE LIABLE FOR ANY\n"
    "DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n"
    "(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n"
    "LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n"
    "ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
    "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n"
    "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n",

    "Nuklear - A single-header ANSI C immediate mode cross-platform GUI library\n\n"
    "MIT License\n\n"
    "Copyright (c) 2017 Micha Mettke\n\n"
    "Permission is hereby granted, free of charge, to any person obtaining a copy of\n"
    "this software and associated documentation files (the \"Software\"), to deal in\n"
    "the Software without restriction, including without limitation the rights to\n"
    "use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies\n"
    "of the Software, and to permit persons to whom the Software is furnished to do\n"
    "so, subject to the following conditions:\n"
    "The above copyright notice and this permission notice shall be included in all\n"
    "copies or substantial portions of the Software.\n"
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
    "SOFTWARE.\n"};


/// <summary>
/// Get a specific 3rd party license.
/// </summary>
/// <param name="license"></param>
/// <returns></returns>
static inline const char *kr_get_license(kr_3rd_party_license_t license) {
    return kr_3rd_party_licenses[license];
}

/// <summary>Get all 3rd party licenses.</summary>
///
/// \warning The content of the buffer is only valid until the next call to this function!
///
/// <param name="sep">Separator to use</param>
/// <returns></returns>
static inline char *kr_get_all_licenses(const char *sep) {
    for (int i = 0; i < KR_3RD_PARTY_TOTAL; ++i) {
        if (i > 0) strcat(all_licenses, sep);
        strcat(all_licenses, kr_3rd_party_licenses[i]);
    }
    return all_licenses;
}

#ifdef __cplusplus
}
#endif
