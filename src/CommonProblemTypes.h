/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include <concepts>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

/// Print integer array as {1, -2, 3}
template<std::integral T>
inline std::string prettyArray(const std::vector<T>& values)
{
    if (values.empty())
        return "{}";
    std::string out = "{" + std::to_string(values[0]);
    for (size_t i = 1; i < values.size(); ++i)
        out += ", " + std::to_string(values[i]);
    return out + "}";
}

}

/// Common problem inputs and outputs that can be reused between problems.
namespace CommonDetails {

template<std::integral T>
using IntegralArray = std::vector<T>;

template<std::integral T>
struct IntegralArrayIO {
    IntegralArray<T> m_data;

    auto operator<=>(const IntegralArrayIO&) const = default;

    void log(std::ostream& os) const { os << prettyArray(m_data); }
    void writeTo(std::ostream& os) const
    {
        os << m_data.size();
        for (size_t i = 0; i < m_data.size(); ++i)
            os << m_data[i];
    }

    void readFrom(std::istream& is) &
    {
        size_t count = 0;
        is >> count;
        m_data.resize(count);
        for (size_t i = 0; i < count; ++i)
            is >> m_data[i];
    }
};

template<std::integral T>
struct IntegralScalar {
    T m_count = 0;

    auto operator<=>(const IntegralScalar&) const = default;

    void log(std::ostream& os) const { os << m_count; }
    void writeTo(std::ostream& os) const { os << m_count; }
    void readFrom(std::istream& is) & { is >> m_count; }
};

template<std::integral T>
struct IntegralRange {
    T m_start = std::numeric_limits<T>::max();
    T m_end   = std::numeric_limits<T>::max();

    auto operator<=>(const IntegralRange&) const = default;

    std::string toString() const { return "[" + std::to_string(m_start) + ", " + std::to_string(m_end) + "]"; }

    void log(std::ostream& os) const { os << toString(); }
    void writeTo(std::ostream& os) const { os << m_start << m_end; }
    void readFrom(std::istream& is) & { is >> m_start >> m_end; }
};

template<std::integral T>
struct IntegralPoint {
    T m_x = 0;
    T m_y = 0;

    auto operator<=>(const IntegralPoint&) const = default;

    std::string toString() const { return "(" + std::to_string(m_x) + ", " + std::to_string(m_y) + ")"; }

    void log(std::ostream& os) const { os << toString(); }
    void writeTo(std::ostream& os) const { os << m_x << m_y; }
    void readFrom(std::istream& is) & { is >> m_x >> m_y; }
};

template<class InputTypeT, class OutputTypeT>
struct IOTransform final {
    using InputType  = InputTypeT;
    using OutputType = OutputTypeT;

    struct TestCase {
        InputType  m_input;
        OutputType m_output;
    };
    using TestCaseList = std::vector<TestCase>;
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
};

}
