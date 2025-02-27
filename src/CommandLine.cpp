/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "CommandLine.h"

#include <iostream>
#include <set>

bool CLIParams::parseArgs(std::ostream& logStream, int argc, char** argv)
{
    std::vector<std::string> argsVector;
    for (int i = 1; i < argc; ++i)
        argsVector.push_back(argv[i]);
    return parseArgs(logStream, argsVector);
}

bool CLIParams::parseArgs(std::ostream& logStream, const std::vector<std::string>& commandLineArgs)
{
    std::map<std::string, std::string> argsMap;
    if (commandLineArgs.size() % 2 == 1) {
        logStream << "Arguments count should be even. Argument '" << commandLineArgs.back()
                  << "' provided without option key.\n";
        return false;
    }
    for (size_t i = 0; i < commandLineArgs.size(); i += 2) {
        std::string arg(commandLineArgs[i]);
        if (arg[0] != '-') {
            logStream << "Argument '" << arg << "' provided without option key.\n";
            return false;
        }
        argsMap[arg.substr(2)] = commandLineArgs[i + 1];
    }
    return parseArgs(logStream, argsMap);
}

bool CLIParams::parseArgs(std::ostream& logStream, const std::map<std::string, std::string>& argsMap)
{
    static std::set<std::string> s_optionNames{
        "problem",
        "impl",
        "student",
        "task",
        "file",
    };
    bool result = true;
    for (const auto& [key, value] : argsMap) {
        if (!s_optionNames.contains(key)) {
            logStream << "Unknown parameter '" << key << "'\n";
            result = false;
            continue;
        }

        if (!parseArg(logStream, key, value))
            result = false;
    }
    return result;
}

bool CLIParams::parseArg(std::ostream& logStream, const std::string& option, const std::string& value)
{
    if (option == "problem")
        m_problemNameFilter = value;
    else if (option == "impl")
        m_implNameFilter = value;
    else if (option == "student")
        m_studentFilter = value;
    else if (option == "file")
        m_filename = value;
    else if (option == "task") {
        if (value == "CheckFromFile")
            m_task = Task::CheckFromFile;
        if (value == "RunBenchmark")
            m_task = Task::RunBenchmark;
    }
    return true;
}

bool CLIParams::isFilteredProblem(std::string_view problem) const
{
    return (!m_problemNameFilter.empty() && problem != m_problemNameFilter);
}

bool CLIParams::isFilteredImpl(std::string_view implName) const
{
    return (!m_implNameFilter.empty() && implName != m_implNameFilter);
}

bool CLIParams::isFilteredStudent(std::string_view name) const
{
    return (!m_studentFilter.empty() && name != m_studentFilter);
}
