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

#ifndef BUILDER_H
#define BUILDER_H

#include <atomic>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

namespace ramen
{
    class Database;

    class Job : boost::noncopyable
    {
    public:
        Job(const boost::shared_ptr<Database>& database);
        virtual ~Job() {}

		virtual const bool prepare() = 0;
        virtual void process() = 0;

    protected:
        boost::shared_ptr<Database> m_pDatabase;
    };

    class Builder : boost::noncopyable
    {
    public:
        void run();
        void addJob(const boost::shared_ptr<Job>& job);

        // slots
        void slotState(const bool state);

    private:
        boost::condition_variable m_condvar;
        boost::mutex m_mutex;

        std::atomic<bool> m_bState;
        std::vector<boost::shared_ptr<Job>> m_work;
    };
} // namespace ramen

#endif // BUILDER_H
