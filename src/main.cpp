/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */

#include "CommonTestUtils.h"

int main(int argc, char** argv)
{
    try {
        CallbackList::callAll();

        CLIParams params;
        if (!params.parseArgs(std::cerr, argc, argv))
            return 1;

        params.createStreams();

        PerformanceCounter topCounter(std::array{ Perf::ExecTime, Perf::PeakHeap });
        auto&              allDesc = AbstractProblemData::getSortedProblemRunners();
        for (auto&& cb : allDesc) {
            if (!cb.m_cb(params))
                return 1;
        }
        (*params.m_loggingStream) << "Finished";
        topCounter.printTo(*params.m_loggingStream, true);
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
