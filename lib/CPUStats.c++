/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2014  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
*/

#include "commonc++/CPUStats.h++"
#include "commonc++/FilePtr.h++"

#ifdef CCXX_OS_MACOSX
#include <mach-o/arch.h>
#include <mach/mach.h>
#include <mach/mach_error.h>

#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#if defined(CCXX_OS_WINDOWS) && !defined(CCXX_OS_WINDOWS_GNU)
#include <pdh.h>
#include <pdhmsg.h>
#endif

#ifdef CCXX_OS_WINDOWS
#include "cbits.h"
#endif

#include <cstring>

namespace ccxx {

struct CPUStats::CPUMetrics
{
  CPUMetrics();
  void reset();

#if defined(CCXX_OS_WINDOWS) && !defined(CCXX_OS_WINDOWS_GNU)
  PDH_HCOUNTER systemCounter;
  PDH_HCOUNTER userCounter;
  PDH_HCOUNTER idleCounter;
  PDH_HCOUNTER usedCounter;
#else
  uint_t lastSystemTicks;
  uint_t lastUserTicks;
  uint_t lastNiceTicks;
  uint_t lastIdleTicks;
  uint_t lastTotalTicks;
#endif

  float system;
  float user;
  float nice;
  float idle;
  float used;
};

/*
 */

CPUStats::CPUMetrics::CPUMetrics()
{
  reset();
}

/*
 */

void CPUStats::CPUMetrics::reset()
{
#ifdef CCXX_OS_WINDOWS
#else
  lastSystemTicks = 0;
  lastUserTicks = 0;
  lastNiceTicks = 0;
  lastIdleTicks = 0;
  lastTotalTicks = 0;
#endif
  system = 0.0f;
  user = 0.0f;
  nice = 0.0f;
  idle = 0.0f;
  used = 0.0f;
}

/*
 */

CPUStats::CPUStats()
  : _cpus(0),
    _metrics(NULL)
#ifdef CCXX_OS_WINDOWS
  , _context(INVALID_HANDLE_VALUE)
#endif
{
}

/*
 */

CPUStats::~CPUStats()
{
  delete[] _metrics;

#if defined(CCXX_OS_WINDOWS) && !defined(CCXX_OS_WINDOWS_GNU)
  if(_context != INVALID_HANDLE_VALUE)
    ::PdhCloseQuery(_context);
#endif
}

/*
 */

void CPUStats::reset()
{
  for(int i = 0; i <= static_cast<int>(_cpus); ++i)
    _metrics[i].reset();
}

/*
 */

bool CPUStats::collect()
{
  if(! _cpus)
  {
    _cpus = lookupCPUCount();
    _metrics = new CPUMetrics[_cpus + 1];

#if defined(CCXX_OS_WINDOWS)
#if !defined(CCXX_OS_WINDOWS_GNU)
    // Initialize PDH query
    if(::PdhOpenQuery(NULL, 0, &_context) != ERROR_SUCCESS)
      return(false);

    // Create counters
    char buf[128];
    for(uint_t index = 0; index <= _cpus; ++index)
    {
      CPUMetrics &metrics = _metrics[index];
      char tag[8];

      if(index != 0)
        ::snprintf(tag, sizeof(tag), "%d", (index - 1));

      ::snprintf(buf, sizeof(buf), "\\Processor(%s)\\%% Privileged Time",
                 (index == 0) ? "_Total" : tag);
      if(::PdhAddCounterA(_context, buf, 0, &metrics.systemCounter)
         != ERROR_SUCCESS)
        return(false);

      ::snprintf(buf, sizeof(buf), "\\Processor(%s)\\%% User Time",
                 (index == 0) ? "_Total" : tag);
      if(::PdhAddCounterA(_context, buf, 0, &metrics.userCounter)
         != ERROR_SUCCESS)
        return(false);

      ::snprintf(buf, sizeof(buf), "\\Processor(%s)\\%% Idle Time",
                 (index == 0) ? "_Total" : tag);
      if(::PdhAddCounterA(_context, buf, 0, &metrics.idleCounter)
         != ERROR_SUCCESS)
        return(false);

      ::snprintf(buf, sizeof(buf), "\\Processor(%s)\\%% Processor Time",
                 (index == 0) ? "_Total" : tag);
      if(::PdhAddCounterA(_context, buf, 0, &metrics.usedCounter)
         != ERROR_SUCCESS)
        return(false);
    }

#else // defined(CCXX_OS_WINDOWS_GNU)

#warning "No implementation available"

    return(false);

#endif // defined(CCXX_OS_WINDOWS_GNU)
#endif // defined(CCXX_OS_WINDOW)
  }

#if defined(CCXX_OS_WINDOWS) && !defined(CCXX_OS_WINDOWS_GNU)
  if(::PdhCollectQueryData(_context) != ERROR_SUCCESS)
    return(false);

  PDH_FMT_COUNTERVALUE value;

  for(uint_t index = 0; index <= _cpus; ++index)
  {
    CPUMetrics &metrics = _metrics[index];

    if(::PdhGetFormattedCounterValue(metrics.systemCounter, PDH_FMT_DOUBLE, 0,
                                     &value) == ERROR_SUCCESS)
      metrics.system = static_cast<float>(value.doubleValue);
    else
      return(false);

    if(::PdhGetFormattedCounterValue(metrics.userCounter, PDH_FMT_DOUBLE, 0,
                                     &value) == ERROR_SUCCESS)
      metrics.user = static_cast<float>(value.doubleValue);
    else
      return(false);

    if(::PdhGetFormattedCounterValue(metrics.idleCounter, PDH_FMT_DOUBLE, 0,
                                     &value) == ERROR_SUCCESS)
      metrics.idle = static_cast<float>(value.doubleValue);
    else
      return(false);

    if(::PdhGetFormattedCounterValue(metrics.usedCounter, PDH_FMT_DOUBLE, 0,
                                     &value) == ERROR_SUCCESS)
      metrics.used = static_cast<float>(value.doubleValue);
    else
      return(false);
  }

  return(true);

#elif defined(CCXX_OS_MACOSX)

  processor_cpu_load_info_t cpu_load;
  mach_msg_type_number_t cpu_msg_count;
  natural_t cpu_count;

  kern_return_t kret = host_processor_info(mach_host_self(),
                                           PROCESSOR_CPU_LOAD_INFO,
                                           &cpu_count,
                                           (processor_info_array_t *)&cpu_load,
                                           &cpu_msg_count);
  if(kret != KERN_SUCCESS)
    return(false);

  uint_t totalTicksAll = 0, userTicksAll = 0, niceTicksAll = 0,
    systemTicksAll = 0, idleTicksAll = 0;

  for(uint_t index = 0; index < _cpus; ++index)
  {
    CPUMetrics &metrics = _metrics[index + 1];

    uint_t userTicks = cpu_load[index].cpu_ticks[CPU_STATE_USER];
    userTicksAll += userTicks;
    uint_t niceTicks = cpu_load[index].cpu_ticks[CPU_STATE_NICE];
    niceTicksAll += niceTicks;
    uint_t systemTicks = cpu_load[index].cpu_ticks[CPU_STATE_SYSTEM];
    systemTicksAll += systemTicks;
    uint_t idleTicks = cpu_load[index].cpu_ticks[CPU_STATE_IDLE];
    idleTicksAll += idleTicks;

    uint_t deltaSystem = systemTicks - metrics.lastSystemTicks;
    uint_t deltaUser = userTicks - metrics.lastUserTicks;
    uint_t deltaNice = niceTicks - metrics.lastNiceTicks;
    uint_t deltaIdle = idleTicks - metrics.lastIdleTicks;

    uint_t totalTicks = systemTicks + userTicks + niceTicks + idleTicks;
    totalTicksAll += totalTicks;

    if(metrics.lastTotalTicks != 0)
    {
      float deltaTotal = static_cast<float>(totalTicks
                                            - metrics.lastTotalTicks);
      if(deltaTotal > 0)
      {
        metrics.used = (static_cast<float>(deltaSystem + deltaUser + deltaNice)
                        / deltaTotal * 100.0f);
        metrics.system = static_cast<float>(deltaSystem) / deltaTotal * 100.0f;
        metrics.user = static_cast<float>(deltaUser) / deltaTotal * 100.0f;
        metrics.nice = static_cast<float>(deltaNice) / deltaTotal * 100.0f;
        metrics.idle = static_cast<float>(deltaIdle) / deltaTotal * 100.0f;
      }
      else if(metrics.idle == 0.0f)
        metrics.idle = 100.0f;
    }

    metrics.lastTotalTicks = totalTicks;
    metrics.lastSystemTicks = systemTicks;
    metrics.lastUserTicks = userTicks;
    metrics.lastNiceTicks = niceTicks;
    metrics.lastIdleTicks = idleTicks;
  }

  // Calculate average across all CPUs

  CPUMetrics &avgMetrics = _metrics[0];

  avgMetrics.system = avgMetrics.user = avgMetrics.nice = avgMetrics.idle
    = 0.0f;

  for(uint_t index = 1; index <= _cpus; ++index)
  {
    avgMetrics.system += _metrics[index].system;
    avgMetrics.user += _metrics[index].user;
    avgMetrics.nice += _metrics[index].nice;
    avgMetrics.idle += _metrics[index].idle;
  }

  avgMetrics.system /= _cpus;
  avgMetrics.user /= _cpus;
  avgMetrics.nice /= _cpus;
  avgMetrics.idle /= _cpus;

  return(true);

#elif defined(CCXX_OS_POSIX)

  FilePtr fp("/proc/stat", "r");

  if(! fp)
    return(false);

  char buf[128];
  uint_t userTicks = 0, niceTicks = 0, systemTicks = 0, idleTicks = 0;

  while(::fgets(buf, sizeof(buf), fp) != NULL)
  {
    uint_t a = 0, b = 0, c = 0, d = 0, e = 0;
    int index = -1;

    if(::strncmp(buf, "cpu", 3))
      break;

    ::sscanf(buf + 3, "%u %u %u %u %u", &a, &b, &c, &d, &e);

    if(isdigit(buf[3]))
    {
      index = ++a;
      userTicks = b;
      niceTicks = c;
      systemTicks = d;
      idleTicks = e;
    }
    else
    {
      index = 0;
      userTicks = a;
      niceTicks = b;
      systemTicks = c;
      idleTicks = d;
    }

    if((index < 0) || (index > static_cast<int>(_cpus)))
      continue;

    CPUMetrics &metrics = _metrics[index];

    uint_t deltaSystem = systemTicks - metrics.lastSystemTicks;
    uint_t deltaUser = userTicks - metrics.lastUserTicks;
    uint_t deltaNice = niceTicks - metrics.lastNiceTicks;
    uint_t deltaIdle = idleTicks - metrics.lastIdleTicks;
    uint_t totalTicks = systemTicks + userTicks + niceTicks + idleTicks;

    if(metrics.lastTotalTicks != 0)
    {
      float deltaTotal = static_cast<float>(totalTicks
                                            - metrics.lastTotalTicks);

      metrics.used = (static_cast<float>(deltaSystem + deltaUser + deltaNice)
                      / deltaTotal * 100.0f);
      metrics.system = static_cast<float>(deltaSystem) / deltaTotal * 100.0f;
      metrics.user = static_cast<float>(deltaUser) / deltaTotal * 100.0f;
      metrics.nice = static_cast<float>(deltaNice) / deltaTotal * 100.0f;
      metrics.idle = static_cast<float>(deltaIdle) / deltaTotal * 100.0f;
    }

    metrics.lastTotalTicks = totalTicks;
    metrics.lastSystemTicks = systemTicks;
    metrics.lastUserTicks = userTicks;
    metrics.lastNiceTicks = niceTicks;
    metrics.lastIdleTicks = idleTicks;
  }

  return(true);

#else

#warning "No implementation available"

  return(false);

#endif
}

/*
 */

float CPUStats::getSystem(int cpu /* = -1 */) const
{
  if((cpu < -1) || (cpu >= static_cast<int>(_cpus)))
    throw OutOfBoundsException();

  return(_metrics[cpu + 1].system);
}

/*
 */

float CPUStats::getUser(int cpu /* = -1 */) const
{
  if((cpu < -1) || (cpu >= static_cast<int>(_cpus)))
    throw OutOfBoundsException();

  return(_metrics[cpu + 1].user);
}

/*
 */

float CPUStats::getNice(int cpu /* = -1 */) const
{
  if((cpu < -1) || (cpu >= static_cast<int>(_cpus)))
    throw OutOfBoundsException();

  return(_metrics[cpu + 1].nice);
}

/*
 */

float CPUStats::getIdle(int cpu /* = -1 */) const
{
  if((cpu < -1) || (cpu >= static_cast<int>(_cpus)))
    throw OutOfBoundsException();

  return(_metrics[cpu + 1].idle);
}

/*
 */

uint_t CPUStats::lookupCPUCount() const
{
  uint_t cpus = 0;

#if defined(CCXX_OS_WINDOWS)

    SYSTEM_INFO sysinfo;
    ::GetSystemInfo(&sysinfo);
    cpus = static_cast<uint_t>(sysinfo.dwNumberOfProcessors);

#elif defined(CCXX_OS_MACOSX)

    int r, mib[2] = { CTL_HW, HW_NCPU }, n;
    size_t len = sizeof(int);
    r = ::sysctl(mib, CCXX_LENGTHOF(mib), &n, &len, NULL, 0);
    if(r == 0)
      cpus = static_cast<uint_t>(n);
    else
      cpus = 1;

#elif defined(CCXX_OS_POSIX)

    FilePtr fp("/proc/cpuinfo", "r");
    if(fp)
    {
      char buf[128];

      while(::fgets(buf, sizeof(buf), fp) != NULL)
      {
        if(!::strncmp(buf, "processor\t", 10))
          ++cpus;
      }
    }
    else
      cpus = 1;

#else

    // assume 1 processor
    cpus = 1;

#endif

    return(cpus);
}


} // namespace ccxx
