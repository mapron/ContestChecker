/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include <algorithm>
#include <climits>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <span>
#include <sstream>
#include <string>
#include <vector>

namespace CommonTypes::Details {

template<size_t N>
struct CompileTimeLiteral {
    char m_chars[N] = {};

    consteval CompileTimeLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, m_chars);
    }
};

template<class P>
concept Loggable = requires(P p, std::ostream& os) {
                       requires std::same_as<decltype(p.log(os)), void>;
                   };

template<class T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<class P>
concept OstreamWriteable = requires(P p, std::ostream& os) {
                               requires std::same_as<decltype(p.writeTo(os)), void>;
                           };
template<class P>
concept IstreamReadable = requires(P p, std::istream& is) {
                              requires std::same_as<decltype(p.readFrom(is)), void>;
                          };

template<Numeric T>
inline void logValue(std::ostream& os, const T& value)
{
    os << std::to_string(value);
}
template<Loggable T>
inline void logValue(std::ostream& os, const T& value)
{
    value.log(os);
}
inline void logValue(std::ostream& os, const std::string& value)
{
    os << '"' << value << '"';
}
template<typename T>
inline std::string logValueAsString(const T& value)
{
    std::ostringstream os;
    logValue(os, value);
    return os.str();
}
template<Numeric T>
inline void writeToImpl(std::ostream& os, const T& value)
{
    os << value;
}
template<Numeric T>
inline void readFromImpl(std::istream& is, T& value)
{
    is >> value;
}

inline void writeToImpl(std::ostream& os, const std::string& value)
{
    if (value.empty())
        os << "\"\"";
    else
        os << value;
}

inline void readFromImpl(std::istream& is, std::string& value)
{
    is >> value;
    if (value == "\"\"")
        value.clear();
}

template<OstreamWriteable T>
inline void writeToImpl(std::ostream& os, const T& value)
{
    value.writeTo(os);
}
template<IstreamReadable T>
inline void readFromImpl(std::istream& is, T& value)
{
    value.readFrom(is);
}

/// Print integer array as {1, -2, 3}
template<typename T>
inline void logArray(std::ostream& os, const std::span<const T>& values)
{
    if (values.empty()) {
        os << "{}";
        return;
    }
    os << "{";
    logValue(os, values[0]);
    for (size_t i = 1; i < values.size(); ++i) {
        os << ", ";
        logValue(os, values[i]);
    }
    os << "}";
}

template<typename T>
inline void logArray(std::ostream& os, const std::vector<T>& values)
{
    return logArray(os, std::span<const T>(values.data(), values.size()));
}

}
