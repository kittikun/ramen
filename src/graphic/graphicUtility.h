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

#ifndef GRAPHIC_UTILITY_H
#define GRAPHIC_UTILITY_H

#include <string>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <glm/glm.hpp>

// Enable VERIFY_GL_ON in order to get detailed information about failing tests,
// such as the line number and failure circumstances.
#define VERIFY_GL_ON 1

#if VERIFY_GL_ON
#define VERIFYGL() graphicUtility::VerifyGL(GL_NO_ERROR, __FILE__, __LINE__)
#define VERIFYGL_RET() if (!graphicUtility::VerifyGL(GL_NO_ERROR, __FILE__, __LINE__)) return false
#define VERIFYEGL() graphicUtility::VerifyEGL(EGL_SUCCESS, __FILE__, __LINE__)
#else
#define VERIFYGL()
#define VERIFYEGL()
#define VERIFYGL_RET()
#endif

namespace ramen
{
	namespace graphicUtility
	{
		const std::string glEnumtoString(const GLenum value);
		const bool VerifyEGL(const EGLint expectedError, const char *file, unsigned line);
		const bool VerifyGL(const GLenum expectedError, const char *file, unsigned line);
	} // namespace graphicUtility
} // namespace ramen

#endif // GRAPHIC_UTILITY_H
