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

#include "resmon.h"

#include <boost/bind.hpp>

#if defined(_WIN32)
#include <windows.h>
#include <Psapi.h>
#elif defined(__unix__)
#include <sys/times.h>
#include <sys/vtimes.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#endif

#include "../coreComponents.h"
#include "../database.h"
#include "../log.h"
#include "../settings.h"
#include "../utility.h"
#include "../graphic/color.h"

namespace ramen
{
#if defined(_WIN32)
	static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	static HANDLE self;
#elif defined(__unix__)
	static clock_t lastCPU, lastSysCPU, lastUserCPU;

	static int parseLine(char* line)
	{
		int i = strlen(line);
		while (*line < '0' || *line > '9')
			line++;
		line[i - 3] = '\0';
		i = atoi(line);
		return i;
	}
#endif

	Resmon::Resmon()
		: m_timer(m_io, boost::chrono::milliseconds(1))
	{
	}

	Resmon::~Resmon()
	{
		LOGP << "Destroying Resmon..";
	}

	void Resmon::initialize(const CoreComponents& components)
	{
		m_timer.async_wait(boost::bind(&Resmon::update, this));
		m_pDatabase = components.database;
		m_iWaitTime = components.settings->get<int>("resmonTimer");

#if defined(_WIN32)
		SYSTEM_INFO sysInfo;
		FILETIME ftime, fsys, fuser;

		GetSystemInfo(&sysInfo);
		m_iNumProcessors = sysInfo.dwNumberOfProcessors;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&lastCPU, &ftime, sizeof(FILETIME));

		self = GetCurrentProcess();
		GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
		memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
		memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
#elif defined(__unix__)
		FILE* file;
		struct tms timeSample;
		char line[128];

		lastCPU = times(&timeSample);
		lastSysCPU = timeSample.tms_stime;
		lastUserCPU = timeSample.tms_utime;

		file = fopen("/proc/cpuinfo", "r");
		m_iNumProcessors = 0;
		while (fgets(line, 128, file) != NULL) {
			if (strncmp(line, "processor", 9) == 0)
				m_iNumProcessors++;
		}
		fclose(file);
#endif
	}

	void Resmon::run()
	{
		LOGP << "Starting resource monitor thread..";

		m_bState.store(true);
		m_io.run();
		LOGP << "Exiting resource monitor thread..";
	}

	void Resmon::update()
	{
		updateCPU();
		updateMemory();

		if (m_bState.load()) {
			m_timer.expires_at(m_timer.expires_at() +  boost::chrono::milliseconds(m_iWaitTime));
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
		percent = static_cast<double>((sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart));
		percent /= (now.QuadPart - lastCPU.QuadPart);
		percent /= m_iNumProcessors;
		lastCPU = now;
		lastUserCPU = user;
		lastSysCPU = sys;
		m_pDatabase->set<uint32_t>("cpu", static_cast<uint32_t>(percent * 100));
#elif defined (__unix__)
		struct tms timeSample;
		clock_t now;
		double percent;

		now = times(&timeSample);
		if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
			timeSample.tms_utime < lastUserCPU){
				//Overflow detection. Just skip this value.
				percent = -1.0;
		}
		else{
			percent = (timeSample.tms_stime - lastSysCPU) +
				(timeSample.tms_utime - lastUserCPU);
			percent /= (now - lastCPU);
			percent /= m_iNumProcessors;
			percent *= 100;
		}
		lastCPU = now;
		lastSysCPU = timeSample.tms_stime;
		lastUserCPU = timeSample.tms_utime;
		m_pDatabase->set<uint32_t>("cpu", static_cast<uint32_t>(percent));
#endif
	}

	void Resmon::updateMemory() const
	{
#if defined(_WIN32)
		PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
			m_pDatabase->set<size_t>("virtual memory", pmc.PagefileUsage);
			m_pDatabase->set<size_t>("physical memory", pmc.WorkingSetSize);
		}
#elif defined(__unix__)
		FILE* file = fopen("/proc/self/status", "r");
		char line[128];

		while (fgets(line, 128, file) != NULL){
			if (strncmp(line, "VmSize:", 7) == 0){
				m_pDatabase->set<size_t>("virtual memory",parseLine(line) * 1000);
			}

			if (strncmp(line, "VmRSS:", 6) == 0){
				m_pDatabase->set<size_t>("physical memory",parseLine(line) * 1000);
			}
		}
		fclose(file);
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