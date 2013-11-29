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

#ifndef LOG_H
#define LOG_H

#include <boost/format.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/circular_buffer.hpp>

#define LOGB BOOST_LOG_SEV(ramen::log::boost_log::get(), ramen::log::Log_Builder)
#define LOGC BOOST_LOG_SEV(ramen::log::boost_log::get(), ramen::log::Log_Core)
#define LOGD BOOST_LOG_SEV(ramen::log::boost_log::get(), ramen::log::Log_Database)
#define LOGGFX BOOST_LOG_SEV(ramen::log::boost_log::get(), ramen::log::Log_GFX)
#define LOGI BOOST_LOG_SEV(ramen::log::boost_log::get(), ramen::log::Log_Io)
#define LOGP BOOST_LOG_SEV(ramen::log::boost_log::get(), ramen::log::Log_Perfmon)
#define LOGW BOOST_LOG_SEV(ramen::log::boost_log::get(), ramen::log::Log_Warning)
#define LOGE BOOST_LOG_SEV(ramen::log::boost_log::get(), ramen::log::Log_Error)

namespace ramen
{
	class FontManager;
	struct CoreComponents;

	namespace log
	{
		enum ELogLevel
		{
			Log_Builder,
			Log_Core,
			Log_Database,
			Log_GFX,
			Log_Io,
			Log_Perfmon,
			Log_Warning,
			Log_Error
		};

		class LogData
		{
		public:
			LogData(const CoreComponents& components);

			void draw();

			boost::circular_buffer<std::string>& buffer() { return m_buffer; }

		private:
			boost::circular_buffer<std::string> m_buffer;
			boost::shared_ptr<FontManager> m_pFontManager;
		};

		BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(boost_log, boost::log::sources::severity_logger_mt<ELogLevel>)

		void initialize();
		void initializeGL(const CoreComponents& components);
	};
} // namespace ramen

#endif // LOG_H
