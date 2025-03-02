/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include "CommonProblemTypesDetails.h"

#include <limits>

/// Common problem inputs and outputs that can be reused between problems.
namespace CommonTypes {

template<Details::Numeric T>
struct NumericScalarIO {
    T m_value = 0;

    auto operator<=>(const NumericScalarIO&) const = default;

    void log(std::ostream& os) const { Details::logValue(os, m_value); }
    void writeTo(std::ostream& os) const { Details::writeToImpl(os, m_value); }
    void readFrom(std::istream& is) & { Details::readFromImpl(is, m_value); }
};

template<Details::Numeric T>
struct NumericRangeIO {
    T m_start = std::numeric_limits<T>::max();
    T m_end   = std::numeric_limits<T>::max();

    auto operator<=>(const NumericRangeIO&) const = default;

    std::string toString() const { return Details::logValueAsString(*this); }

    void log(std::ostream& os) const
    {
        os << "[";
        Details::logValue(os, m_start);
        os << ", ";
        Details::logValue(os, m_end);
        os << "]";
    }
    void writeTo(std::ostream& os) const
    {
        Details::writeToImpl(os, m_start);
        Details::writeToImpl(os, m_end);
    }
    void readFrom(std::istream& is) &
    {
        Details::readFromImpl(is, m_start);
        Details::readFromImpl(is, m_end);
    }
};

template<Details::Numeric T>
struct NumericPointIO {
    T m_x = 0;
    T m_y = 0;

    auto operator<=>(const NumericPointIO&) const = default;

    std::string toString() const { return Details::logValueAsString(*this); }

    void log(std::ostream& os) const
    {
        os << "(";
        Details::logValue(os, m_x);
        os << ", ";
        Details::logValue(os, m_y);
        os << ")";
    }
    void writeTo(std::ostream& os) const
    {
        Details::writeToImpl(os, m_x);
        Details::writeToImpl(os, m_y);
    }
    void readFrom(std::istream& is) &
    {
        Details::readFromImpl(is, m_x);
        Details::readFromImpl(is, m_y);
    }
};

struct StringScalarIO {
    std::string m_text;

    auto operator<=>(const StringScalarIO&) const = default;

    void log(std::ostream& os) const
    {
        Details::logValue(os, m_text);
    }
    void writeTo(std::ostream& os) const
    {
        Details::writeToImpl(os, m_text);
    }
    void readFrom(std::istream& is) &
    {
        Details::readFromImpl(is, m_text);
    }
};

template<typename T>
struct ArrayIO {
    std::vector<T> m_data;

    bool operator==(const ArrayIO&) const = default;

    void log(std::ostream& os) const { Details::logArray(os, m_data); }

    void writeTo(std::ostream& os) const
    {
        os << m_data.size() << "\n";
        for (size_t i = 0; i < m_data.size(); ++i) {
            Details::writeToImpl(os, m_data[i]);
            os << "\n";
        }
    }

    void readFrom(std::istream& is) &
    {
        size_t count = 0;
        is >> count;
        m_data.resize(count);
        for (size_t i = 0; i < count; ++i) {
            Details::readFromImpl(is, m_data[i]);
        }
    }
};

template<typename ArrayElemType, typename ValueType, Details::CompileTimeLiteral valueName>
struct ArrayWithValueIO {
    std::vector<ArrayElemType> m_data;

    ValueType m_value;

    bool operator==(const ArrayWithValueIO&) const = default;

    void log(std::ostream& os) const
    {
        Details::logArray(os, m_data);
        os << ", " << valueName.m_chars << "=";
        Details::logValue(os, m_value);
    }

    void writeTo(std::ostream& os) const
    {
        os << m_data.size();
        Details::writeToImpl(os, m_value);
        os << "\n";
        for (size_t i = 0; i < m_data.size(); ++i) {
            Details::writeToImpl(os, m_data[i]);
            os << "\n";
        }
    }

    void readFrom(std::istream& is) &
    {
        size_t count = 0;
        is >> count;
        Details::readFromImpl(is, m_value);
        m_data.resize(count);
        for (size_t i = 0; i < count; ++i) {
            Details::readFromImpl(is, m_data[i]);
        }
    }
};

template<typename T>
struct MatrixIO {
    size_t         m_rows = 0;
    size_t         m_cols = 0;
    std::vector<T> m_data;

    T  get(size_t row, size_t col) const& { return m_data[row * m_cols + col]; }
    T& get(size_t row, size_t col) & { return m_data[row * m_cols + col]; }

    bool operator==(const MatrixIO&) const = default;

    void log(std::ostream& os) const
    {
        os << m_rows << "x" << m_cols << "\n";
        for (size_t i = 0; i < m_rows; ++i) {
            Details::logArray(os, std::span<const T>(m_data.data() + i * m_cols, m_cols));
            os << "\n";
        }
    }
    void writeTo(std::ostream& os) const
    {
        os << m_rows << m_cols << "\n";
        for (auto&& row : m_data) {
            for (auto&& cell : row) {
                Details::writeToImpl(os, cell);
            }
            os << "\n";
        }
    }

    void readFrom(std::istream& is) &
    {
        is >> m_rows >> m_cols;
        m_data.resize(m_rows * m_cols);
        for (size_t i = 0; i < m_rows; ++i) {
            for (size_t j = 0; j < m_cols; ++j) {
                Details::readFromImpl(is, m_data[i * m_cols + j]);
            }
        }
    }
};

template<class InputTypeT, class OutputTypeT>
struct TestCase {
    InputTypeT  m_input;
    OutputTypeT m_output;
};
template<class InputTypeT, class OutputTypeT>
using TestCaseList = std::vector<TestCase<InputTypeT, OutputTypeT>>;

}
