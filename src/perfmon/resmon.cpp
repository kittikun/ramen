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

#include "Resmon.h"

#include <boost/bind.hpp>
#include <boost/chrono.hpp>

#if defined(_WIN32)
#include "windows.h"
#include "psapi.h"
#endif

#include "../coreComponents.h"
#include "../database.h"
#include "../log.h"
#include "../settings.h"

namespace ramen
{
#if defined(_WIN32)
    static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    static int numProcessors;
    static HANDLE self;
#endif

    Resmon::Resmon()
        : m_timer(m_io, boost::chrono::milliseconds(1))
    {
    }

    void Resmon::initialize(const CoreComponents* components)
    {
        m_timer.async_wait(boost::bind(&Resmon::update, this));
        m_pDatabase = components->database;
        m_waitTime = components->settings->get<int>("resmonTimer");

#if defined(_WIN32)
        SYSTEM_INFO sysInfo;
        FILETIME ftime, fsys, fuser;

        GetSystemInfo(&sysInfo);
        numProcessors = sysInfo.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCPU, &ftime, sizeof(FILETIME));

        self = GetCurrentProcess();
        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
#endif
    }

    void Resmon::run()
    {
        LOGB << "Starting resource monitor thread..";

        m_bState.store(true);
        m_io.run();
        LOGB << "Exiting resource monitor thread..";
    }

    void Resmon::update()
    {
        updateCPU();
        updateMemory();

        if (m_bState.load()) {
            m_timer.expires_at(m_timer.expires_at() +  boost::chrono::milliseconds(m_waitTime));
            m_timer.async_wait(boost::bind(&Resmon::update, this));
        }
    }

    void Resmon::updateCPU() const
    {
#if defined(_WIN32)
        FILETIME ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;
        double percent;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        percent = (sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart);
        percent /= (now.QuadPart - lastCPU.QuadPart);
        percent /= numProcessors;
        lastCPU = now;
        lastUserCPU = user;
        lastSysCPU = sys;

        m_pDatabase->uint("cpu", static_cast<uint32_t>(percent * 100));
#endif
    }

    void Resmon::updateMemory() const
    {
#if defined(_WIN32)
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            m_pDatabase->uint("virtual memory", pmc.PagefileUsage);
            m_pDatabase->uint("physical memory", pmc.WorkingSetSize);
        }
#endif
    }

    void Resmon::slotState(const bool state)
    {
        m_bState.store(state);

        if (!state) {
            m_io.stop();
        }
    }
} // namespace ramen