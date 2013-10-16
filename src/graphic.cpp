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

#include "graphic.h"

#include <SDL.h>
#include <GLES2/gl2.h>

#if defined(_WIN32)
#include <EGL/egl.h>
#include <SDL_syswm.h>
#endif

#include "log.h"
#include "font.h"
#include "utility.h"

namespace ramen
{
    Graphic::Graphic()
       : m_bState(false)
       , m_pContext(nullptr)
       , m_pTextRenderer(new TextRenderer())
       , m_pWindow(nullptr)
    {
    }

    Graphic::~Graphic()
    {
        LOGGFX << "Destroying graphics...";
        SDL_GL_DeleteContext(m_pContext);
        SDL_DestroyWindow(m_pWindow);
    }

    bool Graphic::createContext()
    {
#if defined(_WIN32)
        EGLBoolean	ret = eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
        if (ret != EGL_TRUE) {
            VERIFYEGL();
            return false;
        }
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_EGL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        m_pContext = SDL_GL_CreateContext(m_pWindow);

        if (m_pContext == nullptr) {
            LOGE << SDL_GetError();
            return false;
        }
#endif

        return true;
    }

    bool Graphic::createWindow(const int width, const int height)
    {
        LOGGFX << "Initializing window...";
        m_pWindow = SDL_CreateWindow("ramen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if (m_pWindow == nullptr) {
            LOGE << "Failed to initialize window...";
            return false;
        }

#if defined(_WIN32)
        // SDL on Windows doesn't support GLES yet so we have to do it manually
        EGLBoolean ret;
        EGLConfig eglConfig;
        EGLint configSize;
        EGLNativeWindowType hWnd;
        SDL_bool resWMInfo;
        SDL_SysWMinfo info;

        EGLint eglConfigAttribs[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
        };

        EGLint eglContextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        SDL_VERSION(&info.version); // initialize info structure with SDL version info
        resWMInfo = SDL_GetWindowWMInfo(m_pWindow, &info);
        assert(resWMInfo);
        hWnd = info.info.win.window;

        LOGGFX << "Initializiting EGL..";

        m_eglDisplay = eglGetDisplay(GetDC(hWnd));
        if (m_eglDisplay == EGL_NO_DISPLAY) {
            VERIFYEGL();
            return false;
        }

        ret = eglInitialize(m_eglDisplay, NULL, NULL);
        if (ret != EGL_TRUE) {
            VERIFYEGL();
            return false;
        }

        ret = eglChooseConfig(m_eglDisplay, eglConfigAttribs, &eglConfig, 1, &configSize);
        assert(configSize == 1);
        if (ret != EGL_TRUE) {
            VERIFYEGL();
            return false;
        }

        ret = eglBindAPI(EGL_OPENGL_ES_API);
        if (ret != EGL_TRUE) {
            VERIFYEGL();
            return false;
        }

        m_eglSurface = eglCreateWindowSurface(m_eglDisplay, eglConfig, hWnd, NULL);
        if (m_eglSurface == EGL_NO_SURFACE) {
            VERIFYEGL();
            return false;
        }

        m_eglContext = eglCreateContext(m_eglDisplay, eglConfig, EGL_NO_CONTEXT, eglContextAttribs);
        if (m_eglContext == EGL_NO_CONTEXT) {
            VERIFYEGL();
            return false;
        }
#endif

        return true;
    }

    bool Graphic::initialize(const int width, const int height)
    {
        if (!createWindow(width, height)) {
            return false;
        }

        if (!m_pTextRenderer->initialize()) {
            return false;
        }

        return true;
    }

    void Graphic::run()
    {
        LOGGFX << "Creating GL context...";

        if (!createContext()) {
            return;
        }

        LOGC << "Starting graphic loop..";
        m_bState = true;

        while (m_bState.load()) {
            glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            VERIFYGL();
            swapbuffers();
        }
        LOGC << "Exiting graphic loop..";
    }

    void Graphic::slotState(const bool state)
    {
        m_bState.store(state);
    }

    void Graphic::swapbuffers()
    {
#if defined(_WIN32)
        if (eglSwapBuffers(m_eglDisplay, m_eglSurface) != EGL_TRUE) {
            VERIFYEGL();
        }
#else
        SDL_GL_SwapWindow(m_pWindow);
#endif
    }

} // namespace ramen
