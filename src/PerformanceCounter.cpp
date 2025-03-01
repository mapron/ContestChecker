/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "PerformanceCounter.h"
#include "CustomAlloc.h"

#include <chrono>
#include <iostream>

#ifdef _WIN32
// minimal is Windows 10
#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
#include <Windows.h>
#include <psapi.h>
#endif

namespace PerformanceCounterDetails {
int64_t getCurrentNanoseconds()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}
}

namespace {
#ifdef _WIN32
int64_t FILETIME2us(FILETIME time)
{
    auto const timeUs = ((int64_t) time.dwHighDateTime << 32 | time.dwLowDateTime) / 10; // winapi FILETIME is hundreds of nsec.
    return timeUs;
}
int64_t getCurrentCpuTime()
{
    FILETIME userTime;
    if (!GetSystemTimes(nullptr, nullptr, &userTime)) {
        auto err = GetLastError();
        return 0;
    }
    return FILETIME2us(userTime);
}
int64_t getPeakBytes()
{
    PROCESS_MEMORY_COUNTERS counters;
    GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters));
    return counters.PeakWorkingSetSize;
}

#else
int64_t getCurrentCpuTime()
{
    return 0;
}
int64_t getPeakBytes()
{
    return 0;
}
#endif

/// Current precision clock in microseconds
int64_t getCurrentMicroseconds()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}

void printTime(std::ostream& os, int64_t us)
{
    if (us < 100'000) {
        os << us << " us.";
        return;
    }
    if (us < 100'000'000) {
        os << (us / 1000) << " ms.";
        return;
    }
    os << (us / 1'000'000) << " s.";
}

}
void PerformanceCounter::start(Perf p)
{
    using enum Perf;
    switch (p) {
        case ExecTime:
        {
            m_startUS = getCurrentMicroseconds();
        } break;
        case CpuClock:
        {
            m_startClock = getCurrentCpuTime();

        } break;
        case PeakHeap:
        {
        } break;
        case NewCalls:
        {
            auto info           = CustomAlloc::getNewInfo();
            m_startNewCalls     = info.m_calls;
            m_startNewTotal     = info.m_totalBytes;
            m_startNewElapsedNs = info.m_timeSpentNanosec;
        } break;
        case DeleteCalls:
        {
            auto info              = CustomAlloc::getDeleteInfo();
            m_startDeleteCalls     = info.m_calls;
            m_startDeleteElapsedNs = info.m_timeSpentNanosec;
        } break;
        case TimeSpentAlloc:
        {
            m_startNewElapsedNs    = CustomAlloc::getNewInfo().m_timeSpentNanosec;
            m_startDeleteElapsedNs = CustomAlloc::getDeleteInfo().m_timeSpentNanosec;
        } break;
    }
}

bool PerformanceCounter::isTimedOut(int64_t executionLimit) const
{
    return getCurrentMicroseconds() > (m_startUS + executionLimit);
}

void PerformanceCounter::printTo(std::ostream& os, bool addNewLine)
{
    if (m_enableExecTime) {
        os << ", exec time: ";
        printTime(os, getCurrentMicroseconds() - m_startUS);
    }
    if (m_enableCpuClock) {
        os << ", cpu user time: ";
        printTime(os, getCurrentCpuTime() - m_startClock);
    }
    if (m_enablePeakHeap) {
        os << ", peak heap allocation: " << (getPeakBytes() / 1024) << " kB.";
    }
    if (m_enableNewCalls) {
        const auto info = CustomAlloc::getNewInfo() - CustomAlloc::Info{ m_startNewCalls, m_startNewTotal, m_startNewElapsedNs };
        os << ", new() calls: " << info.m_calls
           << ", total allocated: " << (info.m_totalBytes / 1024) << " kB."
           << ", time spent in new(): ";
        printTime(os, info.m_timeSpentNanosec / 1000);
    }
    if (m_enableDeleteCalls) {
        const auto info = CustomAlloc::getDeleteInfo() - CustomAlloc::Info{ m_startDeleteCalls, 0, m_startDeleteElapsedNs };
        os << ", delete() calls: " << info.m_calls
           << ", time spent in delete(): ";
        printTime(os, info.m_timeSpentNanosec / 1000);
    }
    if (m_enableTimeSpentAlloc) {
        auto       elapsed = getCurrentMicroseconds() - m_startUS;
        auto       ns      = (CustomAlloc::getNewInfo().m_timeSpentNanosec - m_startNewElapsedNs) + (CustomAlloc::getDeleteInfo().m_timeSpentNanosec - m_startDeleteElapsedNs);
        const auto info    = CustomAlloc::getDeleteInfo() - CustomAlloc::Info{ m_startDeleteCalls, 0, m_startDeleteElapsedNs };
        os << ", percent of time in new+delete: " << ((ns / 10) / elapsed) << "%";
    }
    if (addNewLine)
        os << "\n"
           << std::flush;
}
