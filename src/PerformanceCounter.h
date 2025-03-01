/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include <iosfwd>
#include <cstdint>
#include <array>

enum class Perf
{
    ExecTime,
    CpuClock,
    PeakHeap,

    NewCalls,
    DeleteCalls,

    TimeSpentAlloc,
};
namespace PerformanceCounterDetails {
int64_t getCurrentNanoseconds();
}

class PerformanceCounter {
public:
    PerformanceCounter(Perf p1)
    {
        enablePerf(p1);
    }
    template<size_t N>
    PerformanceCounter(const std::array<Perf, N>& ps)
    {
        enablePerf(ps);
    }

    bool isTimedOut(int64_t executionLimit) const;
    void printTo(std::ostream& os, bool addNewLine);

    template<size_t N>
    void enablePerf(const std::array<Perf, N>& ps)
    {
        for (auto p : ps)
            enablePerf(p);
    }

    void enablePerf(Perf p)
    {
        using enum Perf;
        bool* flag = nullptr;
        switch (p) {
            case ExecTime:
                flag = &m_enableExecTime;
                break;
            case CpuClock:
                flag = &m_enableCpuClock;
                break;
            case PeakHeap:
                flag = &m_enablePeakHeap;
                break;
            case NewCalls:
                flag = &m_enableNewCalls;
                break;
            case DeleteCalls:
                flag = &m_enableDeleteCalls;
                break;
            case TimeSpentAlloc:
                flag = &m_enableTimeSpentAlloc;
                break;
        }
        if (*flag)
            return;
        *flag = true;
        start(p);
    }

private:
    void start(Perf p);

private:
    int64_t m_startUS    = 0;
    int64_t m_startClock = 0;

    uint64_t m_startNewCalls        = 0;
    uint64_t m_startNewTotal        = 0;
    uint64_t m_startNewElapsedNs    = 0;
    uint64_t m_startDeleteCalls     = 0;
    uint64_t m_startDeleteElapsedNs = 0;

    bool m_enableExecTime       = false;
    bool m_enableCpuClock       = false;
    bool m_enablePeakHeap       = false;
    bool m_enableNewCalls       = false;
    bool m_enableDeleteCalls    = false;
    bool m_enableTimeSpentAlloc = false;
};
