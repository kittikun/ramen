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
#include <boost/foreach.hpp>
#include <GLES2/gl2.h>

#if defined(_WIN32)
#include <EGL/egl.h>
#include <SDL_syswm.h>
#endif

#include "../core.h"
#include "../coreComponents.h"
#include "../database.h"
#include "../log.h"
#include "../settings.h"
#include "../entity/entity.h"
#include "../io/filesystem.h"
#include "../perfmon/profiler.h"
#include "../utility.h"
#include "color.h"
#include "font.h"
#include "graphicUtility.h"

#include "camera.h"
#include "shader.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ramen
{
	Graphic::Graphic()
		: m_fps(boost::accumulators::tag::rolling_window::window_size = 100)
		, m_pWindow(nullptr)
		, m_bState(false)
		, m_pFontManager(new FontManager())
		, m_pContext(nullptr)
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
		LOGGFX << "Creating GL context...";

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

	bool Graphic::createWindow(const glm::ivec2& size)
	{
		LOGGFX << "Initializing window...";
		m_pWindow = SDL_CreateWindow("ramen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

		if (m_pWindow == nullptr) {
			LOGE << "Failed to initialize window...";
			return false;
		}

#if defined(_WIN32)
		// SDL on Windows doesn't support GLES yet so we have to do it manually
		EGLBoolean ret;
		EGLConfig eglConfig;
		EGLint configSize;
		EGLint major, minor;
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
		if (!resWMInfo) {
			LOGE << "Failed to get window info from SDL";
			return false;
		}

		hWnd = info.info.win.window;

		LOGGFX << "Initializing EGL..";

		m_eglDisplay = eglGetDisplay(GetDC(hWnd));
		if (m_eglDisplay == EGL_NO_DISPLAY) {
			VERIFYEGL();
			return false;
		}

		ret = eglInitialize(m_eglDisplay, &major, &minor);
		if (ret != EGL_TRUE) {
			VERIFYEGL();
			return false;
		}

		LOGGFX << "EGL version " << major << "." << minor;

		ret = eglChooseConfig(m_eglDisplay, eglConfigAttribs, &eglConfig, 1, &configSize);
		if (ret != EGL_TRUE) {
			VERIFYEGL();
			return false;
		}

		if (configSize != 1) {
			LOGE << "More than one EGL configuration was found";
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

	bool Graphic::initialize(const CoreComponents& components)
	{
		glm::ivec2 size(components.settings->get<int>("windowWidth"), components.settings->get<int>("windowHeight"));

		if (!createWindow(size)) {
			return false;
		}

		m_pDatabase = components.database;

		if (!m_pFontManager->initialize(components)) {
			return false;
		}

		Shader::setFilesystem(components.filesystem);

		m_sigError.connect(SigError::slot_type(&Core::slotError, components.core));

		return true;
	}

	bool Graphic::initializeThreadDependents()
	{
		if (!createContext()) {
			return false;
		}

		// All the following need a valid GLES context
		if (!m_pFontManager->initializeGL()) {
			return false;
		}

		// Fonts
		if (!m_pFontManager->loadFontFamillyFromFile("dim", "DIN Light.ttf")) {
			return false;
		}

		if (!m_pFontManager->loadFontFamillyFromFile("vera", "Vera.ttf")) {
			return false;
		}

		m_pFontManager->createFont("dim48", "dim", 48);
		m_pFontManager->createFont("dim16", "dim", 16);
		m_pFontManager->createFont("vera16", "vera", 16);

		return true;
	}

	void Graphic::run()
	{
		if (!initializeThreadDependents()) {
			m_sigError();
			return;
		}

		LOGC << "Starting graphic thread..";
		m_bState.store(true);

		boost::chrono::system_clock::time_point curTime = boost::chrono::system_clock::now();

		const glm::ivec2 size = windowSize();
		glViewport(0, 0, size.x, size.y);

		boost::shared_ptr<Material> material = m_pDatabase->get<boost::shared_ptr<Material>>("material");
		material->setupGL();

		while (m_bState.load()) {
			PROFILE;
			glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			m_pFontManager->addText("Ramen Framework", "dim48", color::yellow, glm::vec2(10, 700));
			m_pFontManager->addText("by kittikun", "dim16", color::cyan, glm::vec2(50, 730));

			const std::string fpsText = boost::str(boost::format("%1$1.0f fps cpu %2%, mem %3%, virt %4%")
				% (1000000 / boost::accumulators::rolling_mean(m_fps))
				% m_pDatabase->get<uint32_t>("cpu")
				% utility::readableSizeByte<size_t>(m_pDatabase->get<size_t>("physical memory"))
				% utility::readableSizeByte<size_t>(m_pDatabase->get<size_t>("virtual memory")));
			m_pFontManager->addText(fpsText, "vera16", color::cyan, glm::vec2(10, 26));

			boost::shared_ptr<Camera> camera = m_pDatabase->get<boost::shared_ptr<Camera>>("camera");

			camera->lookAt(glm::vec3());
			material->use();

			glm::mat4x4 proj = glm::perspective(90.f, float(size.x) / float(size.y), 0.1f, 100.f);
			glm::mat4x4 model;

			glm::rotate(model, 90.f, glm::vec3(1.f, 0.f, 0.f));

			material->mvp(proj * camera->view() * model);

			BOOST_FOREACH(auto entity, m_pDatabase->entities()) {
				entity->draw();
			}

			m_pFontManager->drawText();

			swapbuffers();

			m_fps(boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::system_clock::now() - curTime).count());
			curTime = boost::chrono::system_clock::now();
		}
		LOGC << "Exiting graphic thread..";
	}

	void Graphic::slotState(const bool state)
	{
		m_bState.store(state);
	}

	void Graphic::swapbuffers() const
	{
#if defined(_WIN32)
		if (eglSwapBuffers(m_eglDisplay, m_eglSurface) != EGL_TRUE) {
			VERIFYEGL();
		}
#else
		SDL_GL_SwapWindow(m_pWindow);
#endif
	}

	const glm::ivec2 Graphic::windowSize() const
	{
		glm::ivec2 size;

		SDL_GetWindowSize(m_pWindow, &size.x, &size.y);

		return size;
	}
} // namespace ramen