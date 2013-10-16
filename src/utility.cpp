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

#include "utility.h"

#include <string>
#include <sstream>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "log.h"

namespace ramen
{
    const std::string glErrorEnumToString(GLenum err)
    {
        switch (err) {
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        }

        std::stringstream stream;

        stream << "Unknown GL error " << err << std::endl;

        return stream.str();
    }

    const bool VerifyGL(const unsigned int expectedError, const char* file, unsigned line)
    {
        GLenum glError = glGetError();
        if (glError != expectedError) {
            LOGE << "Unexpected GL Error at " << file << ":" << line << ":" << "Got " << glErrorEnumToString(glError) << " expected " << glErrorEnumToString(expectedError);
            return false;
        }
        return true;
    }

#if defined(_WIN32)
    const std::string eglErrorEnumToString(EGLint err)
    {
        switch (err) {
        case EGL_SUCCESS:
            return "EGL_SUCCESS";
        case EGL_NOT_INITIALIZED:
            return "EGL_NOT_INITIALIZED";
        case EGL_BAD_ACCESS:
            return "EGL_BAD_ACCESS";
        case EGL_BAD_ALLOC:
            return "EGL_BAD_ALLOC";
        case EGL_BAD_ATTRIBUTE:
            return "EGL_BAD_ATTRIBUTE";
        case EGL_BAD_CONTEXT:
            return "EGL_BAD_CONTEXT";
        case EGL_BAD_CONFIG:
            return "EGL_BAD_CONFIG";
        case EGL_BAD_CURRENT_SURFACE:
            return "EGL_BAD_CURRENT_SURFACE";
        case EGL_BAD_DISPLAY:
            return "EGL_BAD_DISPLAY";
        case EGL_BAD_SURFACE:
            return "EGL_BAD_SURFACE";
        case EGL_BAD_MATCH:
            return "EGL_BAD_MATCH";
        case EGL_BAD_PARAMETER:
            return "EGL_BAD_PARAMETER";
        case EGL_BAD_NATIVE_PIXMAP:
            return "EGL_BAD_NATIVE_PIXMAP";
        case EGL_BAD_NATIVE_WINDOW:
            return "EGL_BAD_NATIVE_WINDOW";
        case EGL_CONTEXT_LOST:
            return "EGL_CONTEXT_LOST";
        }

        std::stringstream stream;

        stream << "Unknown EGL error " << err << std::endl;

        return stream.str();
    }

    const bool VerifyEGL(const int expectedError, const char* file, unsigned line)
    {
#if defined(_WIN32)
        EGLint eglError = eglGetError();
        if (eglError != expectedError) {
            LOGE << "Unexpected EGL Error at " << file << ":" << line << ":" << "Got " << eglErrorEnumToString(eglError) << " expected " << eglErrorEnumToString(expectedError);
            return false;
        }
#endif
        return true;
    }
#endif

} // namespace ramen
