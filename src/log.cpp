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

#include "log.h"

#include <iostream>
#include <boost/date_time.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/debug_output_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>

#include "coreComponents.h"
#include "database.h"
#include "settings.h"
#include "graphic/font.h"
#include "graphic/color.h"

namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;

namespace ramen
{
	namespace log
	{
		class GLSink : public sinks::basic_formatted_sink_backend<char, sinks::synchronized_feeding>
		{
		public:
			GLSink(const boost::shared_ptr<LogData>& data);
			void consume(logging::record_view const& rec, string_type const& str);

		private:
			boost::shared_ptr<LogData> m_pData;
		};

		std::ostream& operator<<(std::ostream& strm, ELogLevel level)
		{
			static const char* strings[] =
			{
				"builder",
				"core",
				"database",
				"gfx",
				"io",
				"perfmon",
				"WARNING",
				"ERROR",
			};

			if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
				strm << strings[level];
			else
				strm << static_cast<int>(level);

			return strm;
		}

		BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", ELogLevel)

		void initialize()
		{
			logging::add_console_log(std::cout, keywords::format = expr::format("%1%: [%2%] %3%")
				% expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S")
				% severity
				% expr::message);

			logging::add_common_attributes();

#if defined(_WIN32)
			boost::shared_ptr<logging::core> core = logging::core::get();
			boost::shared_ptr<sinks::synchronous_sink< sinks::debug_output_backend>> debugSink(new sinks::synchronous_sink<sinks::debug_output_backend>());

			debugSink->set_filter(expr::is_debugger_present());
			debugSink->set_formatter(expr::format("%1%: [%2%] %3%\n")
				% expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S")
				% severity
				% expr::message);

			core->add_sink(debugSink);
#endif
		}

		void initializeGL(const CoreComponents& components)
		{
			boost::shared_ptr<logging::core> core = logging::core::get();
			boost::shared_ptr<LogData> logData(new LogData(components));
			boost::shared_ptr<sinks::synchronous_sink<GLSink>> sink(new sinks::synchronous_sink<GLSink>(logData));

			components.database->set<boost::shared_ptr<LogData>>("logData", logData);

			sink->set_formatter(expr::format("%1%: [%2%] %3%\n")
				% expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S")
				% severity
				% expr::message);
			core->add_sink(sink);
		}

		//-------------------------------------------------------------------------------------
		// GLSINK
		//-------------------------------------------------------------------------------------
		GLSink::GLSink(const boost::shared_ptr<LogData>& data)
			: m_pData(data)
		{
		}

		// The function consumes the log records that come from the frontend
		void GLSink::consume(logging::record_view const& rec, string_type const& str)
		{
			m_pData->buffer().push_back(str);
		}

		//-------------------------------------------------------------------------------------
		// LOGDATA
		//-------------------------------------------------------------------------------------
		LogData::LogData(const CoreComponents& components)
			: m_buffer(components.settings->get<int>("glLogSize"))
			, m_pFontManager(components.fontManager)
		{

		}

		void LogData::draw()
		{
			size_t size = m_buffer.size();
			const glm::vec2 offset(10, 150);

			for (size_t i = 0; i < size; ++i) {
				m_pFontManager->addText(m_buffer[i], "vera16", color::yellow, offset + glm::vec2(0.f, i * 21.f));
			}
		}

	} // namespace log
} // namespace ramen