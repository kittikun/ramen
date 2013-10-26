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

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <atomic>
#include <memory>
#include <boost/signals2.hpp>
#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>
#include <glm/glm.hpp>

struct SDL_Window;

namespace ramen
{
	class Core;
	class Filesystem;
	class FontManager;

	class Graphic : boost::noncopyable
	{
	public:
		Graphic();
		~Graphic();

		bool initialize(const glm::ivec2& winSize, Core* core, boost::weak_ptr<Filesystem> filesystem);
		void run();

		const glm::ivec2 windowSize() const;

		// slots
		void slotState(const bool state);

	private:
		bool createWindow(const glm::ivec2& size);
		bool createContext();
		void swapbuffers() const;
		bool initializeThreadDependents();

	private:
		boost::weak_ptr<Filesystem> m_pFilesystem;
		SDL_Window* m_pWindow;
		std::atomic<bool> m_bState;
		std::unique_ptr<FontManager> m_pFontManager;
		void* m_pContext;

		// signals
		typedef boost::signals2::signal<void()> SigError;
		SigError m_sigError;

#if defined(_WIN32)
		void* m_eglDisplay;
		void* m_eglSurface;
		void* m_eglContext;
#endif
	};

} // namespace ramen

#endif // GRAPHIC_H


