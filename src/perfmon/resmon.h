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

#ifndef PERFMON_H
#define PERFMON_H

#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/shared_ptr.hpp>

namespace ramen
{
    struct CoreComponents;
    class Database;

    class Resmon : boost::noncopyable
    {
    public:
        Resmon();

        void initialize(const CoreComponents* components);
        void run();

        // slots
        void slotState(const bool state);

    private:
        void update();
        void updateCPU() const;
        void updateMemory() const;

    private:
        int m_waitTime;
        boost::asio::io_service m_io;
        boost::asio::system_timer m_timer;
        boost::shared_ptr<Database> m_pDatabase;
        std::atomic<bool> m_bState;
    };
} // namespace ramen

#endif // PERFMON_H
