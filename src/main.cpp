/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */

#include "CommonTestUtils.h"

#ifdef _WIN32
#include <Windows.h>
#include <psapi.h>
int64_t getPeakBytes()
{
    PROCESS_MEMORY_COUNTERS counters;
    GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters));
    return counters.PeakWorkingSetSize;
}
#else
int64_t getPeakBytes()
{
    return 0;
}
#endif

int main(int argc, char** argv)
{
    try {
        CallbackList::callAll();

        CLIParams params;
        params.m_problemNameFilter                 = "";
        params.m_implNameFilter                    = "";
        params.m_implNameOrdering.m_order["naive"] = 1;
        params.m_loggingStream                     = &std::cout;
        if (!params.parseArgs(std::cerr, argc, argv))
            return 1;

        auto& allDesc = AbstractProblemDetails::getSortedProblemRunners();
        for (auto&& cb : allDesc) {
            if (!cb.m_cb(params))
                return 1;
        }
        std::cout << "Peak memory consumption was: " << getPeakBytes() << " bytes.\n";
    }
    catch (std::runtime_error& ex) {
        std::cerr << "std::exception was thrown:" << ex.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown exception was thrown.\n";
        return 1;
    }

    return 0;
}
