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

#include "builder.h"

#include "../database.h"
#include "../log.h"

namespace ramen
{

    void Builder::addJob(const Job& Job)
    {
        {
            boost::lock_guard<boost::mutex> lock(m_mutex);
            m_work.push_back(Job);
        }
        m_condvar.notify_one();
    }

	void Builder::run()
	{
		LOGB << "Starting builder thread..";
		m_bState.store(true);

		while (m_bState.load()) {

			boost::unique_lock<boost::mutex> lock(m_mutex);
			while (m_work.empty()) {
				m_condvar.wait(lock);	

                if (!m_bState.load())
                    break;

			}

            if (!m_work.empty()) {
                m_work.back().process();
                m_work.pop_back();
            }
		}

		LOGC << "Exiting builder thread..";
	}

	void Builder::slotState(const bool state)
	{
		m_bState.store(state);

        if (!m_bState.load())
            m_condvar.notify_one();
	}

    Job::Job(const boost::shared_ptr<Database>& database)
        : m_pDatabase(database)
    {
    }

} // namespace ramen

