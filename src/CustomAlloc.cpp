/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "CustomAlloc.h"

#include <malloc.h>

namespace {
CustomAlloc::Info s_newInfo{};
CustomAlloc::Info s_deleteInfo{};
}
namespace CustomAlloc {
Info getNewInfo()
{
    return s_newInfo;
}
Info getDeleteInfo()
{
    return s_deleteInfo;
}
}

void* operator new(decltype(sizeof(0)) n) noexcept(false)
{
    s_newInfo.m_calls++;
    s_newInfo.m_totalBytes += n;
    int64_t startNS = PerformanceCounterDetails::getCurrentNanoseconds();
    void*   result  = malloc(n);
    s_newInfo.m_timeSpentNanosec += PerformanceCounterDetails::getCurrentNanoseconds() - startNS;
    return result;
}

void operator delete(void* p) throw()
{
    s_deleteInfo.m_calls++;
    int64_t startNS = PerformanceCounterDetails::getCurrentNanoseconds();
    free(p);
    s_deleteInfo.m_timeSpentNanosec += PerformanceCounterDetails::getCurrentNanoseconds() - startNS;
}
