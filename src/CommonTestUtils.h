/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include "CommandLine.h"
#include "CommonProblemTypes.h"
#include "PerformanceCounter.h"

#include <algorithm>

/// Non-heap allocating linked list of funtion pointers.
/// This list is global for process.
/// To add a new callback, just declare a global/static variable and pass callback to constructor.
struct CallbackList {
    using Callback             = void (*)(void);
    using CallbackNodePtr      = CallbackList*;
    Callback        m_callback = nullptr;
    CallbackNodePtr m_next     = nullptr;

    CallbackList() = default;
    explicit CallbackList(Callback cb)
    {
        CallbackNodePtr& back = getBack();
        back->m_next          = this;
        back->m_callback      = cb;
        back                  = this;
    }

    static CallbackNodePtr getFront()
    {
        static CallbackList s_front;
        return &s_front;
    }
    static CallbackNodePtr& getBack()
    {
        static CallbackList* s_back = getFront();
        return s_back;
    }
    static void callAll()
    {
        CallbackNodePtr node = getFront();
        while (node->m_next) {
            if (node->m_callback)
                node->m_callback();
            node = node->m_next;
        }
    }
};

/// Helper class for ability to store string literals as NTTP (not-type template parameter)
template<size_t N>
struct CompileTimeLiteral {
    char m_chars[N] = {};

    consteval CompileTimeLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, m_chars);
    }
};

/// Stores all run() functions for every problem.
struct AbstractProblemData {
    using Callback = bool (*)(const CLIParams& params);
    struct RunnerDesc {
        Callback         m_cb;
        std::string_view m_problemName;
    };

    using ProblemDescList = std::vector<RunnerDesc>;

    static ProblemDescList& getProblemRunners()
    {
        static ProblemDescList list;
        return list;
    }
    static void registerRunner(Callback callback, std::string_view name) { getProblemRunners().push_back({ callback, name }); }

    static const ProblemDescList& getSortedProblemRunners()
    {
        auto& allDesc = getProblemRunners();
        std::sort(allDesc.begin(), allDesc.end(), [](const RunnerDesc& l, const RunnerDesc& r) {
            return l.m_problemName < r.m_problemName;
        });
        return allDesc;
    }
};

/** Abstract problem runner
 
 InputTypeT - input of solution
 OutputTypeT - output of solution
 problemName - problem identificator
 thanks to problemName even same Input+Output AbstractProblem will be separate type (and solve ODR issues)
*/
template<class InputTypeT, class OutputTypeT, CompileTimeLiteral problemName>
struct AbstractProblem final {
    using InputType  = InputTypeT;
    using OutputType = OutputTypeT;

    using TestCase     = CommonTypes::TestCase<InputType, OutputType>;
    using TestCaseList = CommonTypes::TestCaseList<InputType, OutputType>;

    struct TestCaseSource {
        const TestCaseList* m_cases;
        std::string_view    m_sourceName; // "compile", "source tree" etc.
    };
    using TestCaseSourceList = std::vector<TestCaseSource>;

    using Transform = OutputType (*)(const InputType&);
    struct Solution {
        Transform        m_transform = nullptr;
        std::string_view m_implName;
        std::string_view m_studentName;
    };

    using SolutionList = std::vector<Solution>;

    constexpr static std::string_view s_problemName = problemName.m_chars;

    static SolutionList& getSolutions()
    {
        static SolutionList impls;
        return impls;
    }

    static void registerSolution(Transform t, std::string_view implName, std::string_view studentName)
    {
        getSolutions().push_back({ t, implName, studentName });
    }

    static TestCaseSourceList& getTestCaseSourceList()
    {
        static TestCaseSourceList impls;
        return impls;
    }

    static void sortTestCaseSourceList()
    {
        TestCaseSourceList& list = getTestCaseSourceList();
        std::sort(list.begin(), list.end(), [](auto& l, auto& r) { return l.m_sourceName < r.m_sourceName; });
    }

    static void registerTestSet(const TestCaseList* list, std::string_view source)
    {
        getTestCaseSourceList().push_back({ list, source });
    }

    /// Run test or benchmark with parameters.
    /// Returns false on first failure.
    static bool run(const CLIParams& params)
    {
        if (params.isFilteredProblem(s_problemName))
            return true;

        auto& solutions = getSolutions();
        std::sort(solutions.begin(), solutions.end(), [&params](const Solution& l, const Solution& r) {
            return params.makeOrderingTuple(l.m_studentName, l.m_implName)
                   < params.makeOrderingTuple(r.m_studentName, r.m_implName);
        });
        std::ostream& logger = *params.m_loggingStream;

        for (const Solution& solution : solutions) {
            if (params.isFilteredImpl(solution.m_implName) || params.isFilteredStudent(solution.m_studentName))
                continue;

            if (params.m_task == CLIParams::Task::CheckOutput && !runTests(params, solution, true))
                return false;
            if (params.m_task == CLIParams::Task::PrintOutput && !runTests(params, solution, false))
                return false;

            if (params.m_task == CLIParams::Task::Benchmark && !runBenchmark(params, solution))
                return false;
        }
        logger << "Problem '" << s_problemName;
        if (params.m_task == CLIParams::Task::CheckOutput)
            logger << "' - all tests passed!\n";
        if (params.m_task == CLIParams::Task::PrintOutput)
            logger << "' - finished!\n";
        if (params.m_task == CLIParams::Task::Benchmark)
            logger << "' - end of benchmark\n";
        logger << std::flush;
        return true;
    }

    static void registerCustomSource(const CLIParams& params)
    {
        static TestCaseList customSource(1);
        bool                useCustomSource = false;
        if (params.m_testInputStream) {
            useCustomSource = true;
            customSource[0].m_input.readFrom(*params.m_testInputStream);
        }
        if (params.m_testOutputStream) {
            useCustomSource = true;
            customSource[0].m_output.readFrom(*params.m_testOutputStream);
        }
        if (useCustomSource) {
            auto& customList = getTestCaseSourceList();
            customList.resize(1);
            customList[0].m_sourceName = params.useStdin() ? "cli-stdin" : "cli-file";
            customList[0].m_cases      = &customSource;
        }
    }

    static bool runTests(const CLIParams& params, const Solution& solution, bool needCheck)
    {
        std::ostream& logger = *params.m_loggingStream;

        logger << "Starting problem '" << s_problemName
               << "' student '" << solution.m_studentName
               << "' solution '" << solution.m_implName << "' tests...\n"
               << std::flush;

        registerCustomSource(params);

        PerformanceCounter topCounter(std::array{ Perf::ExecTime, Perf::CpuClock });

        if (params.m_enableAllocTrace)
            topCounter.enablePerf(std::array{ Perf::NewCalls, Perf::DeleteCalls, Perf::TimeSpentAlloc });
        size_t count = 0;
        sortTestCaseSourceList();
        for (const TestCaseSource& tcaseSource : getTestCaseSourceList()) {
            for (int tcaseIndex = -1; const TestCase& tcase : *tcaseSource.m_cases) {
                tcaseIndex++;
                count++;
                const std::string tcaseIndexStr = "[" + std::string(tcaseSource.m_sourceName) + "/" + std::to_string(tcaseIndex) + "]";

                PerformanceCounter caseCounter(std::array{ Perf::ExecTime });
                if (params.m_enableAllocTrace)
                    caseCounter.enablePerf(std::array{ Perf::NewCalls, Perf::DeleteCalls });
                const auto calculatedOutput = solution.m_transform(tcase.m_input);

                if (needCheck) {
                    if (params.m_printAllCases) {
                        logger << "Case " << tcaseIndexStr;
                        caseCounter.printTo(logger, true);
                    }
                    if (calculatedOutput != tcase.m_output) {
                        const std::string tcaseIndexPad(tcaseIndexStr.size(), ' ');
                        logger << "For problem input " << tcaseIndexStr << ": ";
                        tcase.m_input.log(logger);
                        logger << "\n";
                        logger << "expected output" << tcaseIndexPad << " is: ";
                        tcase.m_output.log(logger);
                        logger << "\n";
                        logger << " but calculated" << tcaseIndexPad << " is: ";
                        calculatedOutput.log(logger);
                        logger << "\n"
                               << std::flush;
                        return false;
                    }
                } else {
                    tcase.m_output.writeTo(*params.m_printStream);
                    *params.m_printStream << "\n"
                                          << std::flush;
                }
            }
        }
        if (!needCheck)
            return true;
        logger << "Solutions are correct, total cases: " << count;
        topCounter.printTo(logger, true);
        return true;
    }

    static bool runBenchmark(const CLIParams& params, const Solution& solution)
    {
        std::ostream& logger = *params.m_loggingStream;

        logger << "Starting problem '" << s_problemName
               << "' student '" << solution.m_studentName
               << "' solution '" << solution.m_implName
               << "' benchmark (" << params.m_benchmarkTimeLimitMS << " ms limit)...\n"
               << std::flush;
        registerCustomSource(params);

        PerformanceCounter topCounter(std::array{ Perf::ExecTime });
        if (params.m_enableAllocTrace)
            topCounter.enablePerf(Perf::TimeSpentAlloc);
        int iterationCount = 0;
        for (; iterationCount < 10'000'000; ++iterationCount) {
            for (const TestCaseSource& tcaseSource : getTestCaseSourceList()) {
                for (const TestCase& tcase : *tcaseSource.m_cases) {
                    solution.m_transform(tcase.m_input);
                }
            }

            if (topCounter.isTimedOut(params.m_benchmarkTimeLimitMS * 1000))
                break;
        }

        logger << "Benchmark ended, iterations: " << iterationCount;
        topCounter.printTo(logger, true);
        return true;
    }
};
