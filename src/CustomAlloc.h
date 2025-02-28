/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include <cstdint>

namespace PerformanceCounterDetails {
int64_t getCurrentNanoseconds();
}
namespace CustomAlloc {

struct Info {
    uint64_t m_calls            = 0;
    uint64_t m_totalBytes       = 0;
    uint64_t m_timeSpentNanosec = 0;

    friend Info operator-(const Info& l, const Info& r)
    {
        return Info{
            .m_calls            = l.m_calls - r.m_calls,
            .m_totalBytes       = l.m_totalBytes - r.m_totalBytes,
            .m_timeSpentNanosec = l.m_timeSpentNanosec - r.m_timeSpentNanosec,
        };
    }
};

Info getNewInfo();
Info getDeleteInfo();

}
