/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include "Problems/ArraySum/ProblemArraySum.h"

#include <numeric>

namespace {
using namespace ArraySumProblemDetails;

Output solution(const Input& input)
{
    const int64_t result = std::accumulate(input.m_data.cbegin(), input.m_data.cend(), int64_t(0), std::plus<int64_t>());

    return { .m_count = result };
}

}
