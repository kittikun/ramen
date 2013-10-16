//  Copyright (C) 2013  kittikun
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef UTILITY_H
#define UTILITY_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <string>

// Enable VERIFY_GL_ON in order to get detailed information about failing tests,
// such as the line number and failure circumstances.
#define VERIFY_GL_ON 1

#if VERIFY_GL_ON
    #define VERIFYGL() VerifyGL(GL_NO_ERROR, __FILE__, __LINE__)
#else
    #define VERIFYGL() (true)
#endif

// for Windows, we also use EGL directly
#if defined(_WIN32)
    #if VERIFY_GL_ON
        #define VERIFYEGL() VerifyEGL(EGL_SUCCESS, __FILE__, __LINE__)
    #else
        #define VERIFYEGL() (true)
    #endif

        const bool ramen::VerifyEGL(EGLint expectedError, const char *file, unsigned line);
#endif

namespace ramen {

    const bool VerifyGL(GLenum expectedError, const char *file, unsigned line);
     
} // namespace ramen

#endif // UTILITY_H
