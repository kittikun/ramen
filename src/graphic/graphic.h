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
#include <glm/glm.hpp>


struct SDL_Window;

namespace ramen
{
	class Core;
	class TextRenderer;

	class Graphic : boost::noncopyable, public boost::signals2::trackable
	{
	public:
		Graphic();
		~Graphic();

		bool initialize(Core* core, const int width, const int height);
		void run();

		const glm::ivec2 getWindowSize() const;

		// slots
		void slotState(const bool state);

	private:
		bool createWindow(const int width, const int height);
		bool createContext();
		void swapbuffers();
		bool initializeThreadDependents();

	private:
		std::atomic<bool> m_bState;
		void* m_pContext;
		std::unique_ptr<TextRenderer> m_pTextRenderer;
		SDL_Window* m_pWindow;

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


