/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

struct CLIParams {
    enum class Task
    {
        RunTests,
        RunBenchmark,
        CheckFromFile,
    };
    std::ostream* m_loggingStream = nullptr;

    std::string m_problemNameFilter;
    std::string m_implNameFilter;
    std::string m_studentFilter;

    struct Ordering {
        std::map<std::string_view, int> m_order;

        int get(std::string_view value) const
        {
            auto it = m_order.find(value);
            return it == m_order.end() ? INT_MAX : it->second;
        }
    };

    Ordering m_problemNameOrdering;
    Ordering m_implNameOrdering;
    Ordering m_studentOrdering;

    auto makeOrderingTuple(std::string_view student, std::string_view impl) const
    {
        return std::tuple{ m_studentOrdering.get(student), student, m_implNameOrdering.get(impl), impl };
    }

    std::string m_filename;

    Task m_task = Task::RunTests;

    int64_t m_benchmarkTimeLimitMS = 10000; // 10 sec.

    bool parseArgs(std::ostream& logStream, int argc, char** argv);

    bool parseArgs(std::ostream& logStream, const std::vector<std::string>& commandLineArgs);

    bool parseArgs(std::ostream& logStream, const std::map<std::string, std::string>& argsMap);

    bool parseArg(std::ostream& logStream, const std::string& option, const std::string& value);

    bool isFilteredProblem(std::string_view problem) const;
    bool isFilteredImpl(std::string_view implName) const;
    bool isFilteredStudent(std::string_view name) const;
};
