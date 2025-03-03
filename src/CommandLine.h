/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include <climits>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct CLIParams {
public:
    enum class Task
    {
        CheckOutput,
        PrintOutput,
        Benchmark,
    };
    struct Ordering {
        std::map<std::string_view, int> m_order;

        int get(std::string_view value) const
        {
            auto it = m_order.find(value);
            return it == m_order.end() ? INT_MAX : it->second;
        }
    };
    struct Impl;

    // magic constants for file parameters.
    static constexpr const std::string_view g_stdin  = "stdin";
    static constexpr const std::string_view g_stdout = "stdout";
    static constexpr const std::string_view g_stderr = "stderr";
    static constexpr const std::string_view g_null   = "null";

public:
    std::istream* m_testInputStream  = nullptr;
    std::istream* m_testOutputStream = nullptr;
    std::ostream* m_printStream      = nullptr;
    std::ostream* m_loggingStream    = nullptr;

    std::string m_testInputFile;
    std::string m_testOutputFile;
    std::string m_printFile;
    std::string m_logFile;

    std::string m_problemNameFilter;
    std::string m_implNameFilter;
    std::string m_studentFilter;

    Ordering m_problemNameOrdering;
    Ordering m_implNameOrdering;
    Ordering m_studentOrdering;

    Task m_task = Task::CheckOutput;

    int64_t m_benchmarkTimeLimitMS = 10000; // 10 sec.
    bool    m_printAllCases        = false;
    bool    m_enableAllocTrace     = false;

public:
    CLIParams();
    ~CLIParams();

    auto makeOrderingTuple(std::string_view student, std::string_view impl) const
    {
        return std::tuple{ m_studentOrdering.get(student), student, m_implNameOrdering.get(impl), impl };
    }

    bool parseArgs(std::ostream& logStream, int argc, char** argv);

    bool parseArgs(std::ostream& logStream, const std::vector<std::string>& commandLineArgs);

    bool parseArgs(std::ostream& logStream, const std::map<std::string, std::string>& argsMap);

    bool parseArg(std::ostream& logStream, const std::string& option, const std::string& value);

    bool isFilteredProblem(std::string_view problem) const;
    bool isFilteredImpl(std::string_view implName) const;
    bool isFilteredStudent(std::string_view name) const;

    bool useStdin() const { return m_testInputFile == g_stdin; }

    void createStreams();

private:
    std::unique_ptr<Impl> m_impl;
};
