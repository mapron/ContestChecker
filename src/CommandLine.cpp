/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "CommandLine.h"

#include <iostream>
#include <set>
#include <fstream>
#include <sstream>

struct CLIParams::Impl {
    std::ifstream m_testInput;
    std::ifstream m_testOutput;
    std::ofstream m_print;
    std::ofstream m_log;

    std::ostringstream m_nullStream;
};

CLIParams::CLIParams()
    : m_impl(std::make_unique<Impl>())
{
    m_implNameOrdering.m_order["naive"] = 1;
    m_loggingStream                     = &std::cout;
}

CLIParams::~CLIParams()
{
}

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
        "test-input",
        "test-output",
        "print-to",
        "log-to",
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

    else if (option == "test-input")
        m_testInputFile = value;
    else if (option == "test-output")
        m_testOutputFile = value;
    else if (option == "print-to")
        m_printFile = value;
    else if (option == "log-to")
        m_logFile = value;

    else if (option == "task") {
        if (value == "CheckOutput")
            m_task = Task::CheckOutput;
        else if (value == "PrintOutput")
            m_task = Task::PrintOutput;
        else if (value == "Benchmark")
            m_task = Task::Benchmark;
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

void CLIParams::createStreams()
{
    auto makeInputFile = [](const std::string& name, std::istream*& stream, std::ifstream& fstream) {
        if (name.empty())
            return;

        if (name == g_stdin) {
            stream = &std::cin;
            return;
        }

        fstream.exceptions(std::ifstream::failbit);
        fstream.open(name);
        stream = &fstream;
    };
    auto& nullStream     = this->m_impl->m_nullStream;
    auto  makeOutputFile = [&nullStream](const std::string& name, std::ostream*& stream, std::ofstream& fstream) {
        if (name.empty())
            return;

        if (name == g_stdout) {
            stream = &std::cout;
            return;
        }
        if (name == g_stderr) {
            stream = &std::cerr;
            return;
        }
        if (name == g_null) {
            stream = &nullStream;
            return;
        }

        fstream.exceptions(std::ofstream::failbit);
        fstream.open(name);
        stream = &fstream;
    };
    if (m_task == Task::PrintOutput) {
        if (m_printFile.empty())
            m_printFile = g_stdout;
        if (m_logFile.empty())
            m_logFile = g_null;
    }
    makeInputFile(m_testInputFile, m_testInputStream, m_impl->m_testInput);
    makeInputFile(m_testOutputFile, m_testOutputStream, m_impl->m_testOutput);

    makeOutputFile(m_printFile, m_printStream, m_impl->m_print);
    makeOutputFile(m_logFile, m_loggingStream, m_impl->m_log);
}
