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

#ifndef CORE_H
#define CORE_H

#include <atomic>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

namespace ramen
{
	class Builder;
	class Database;
	class EntityManipulator;
	class FBXManager;
	class Filesystem;
	class FontManager;
	class Graphic;
	class Resmon;
	class Settings;
	struct CoreComponents;

	class Core : boost::noncopyable
	{
	public:
		Core();
		~Core();

		const bool initialize();
		void run();

		// slots
		void slotError();

	private:
		void stop();
		void fillCoreComponents(CoreComponents& out);

	private:
		// members
		std::atomic<bool> m_bState;
		std::unique_ptr<EntityManipulator> m_pManipulator;
		boost::thread_group m_threads;
		boost::shared_ptr<Builder> m_pBuilder; // shared_ptr because of signals2 tracking
		boost::shared_ptr<Database> m_pDatabase;
		boost::shared_ptr<FBXManager> m_pFbxManager;
		boost::shared_ptr<Filesystem> m_pFilesystem;
		boost::shared_ptr<Graphic> m_pGraphic; // shared_ptr because of signals2 trackings
		boost::shared_ptr<Resmon> m_pResmon;
		boost::shared_ptr<Settings> m_pSettings;

		// signals
		typedef boost::signals2::signal<void(const bool)> SigState;
		SigState m_sigState;
	};
} // namespace ramen

#endif // CORE_H
