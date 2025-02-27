/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include "Problems/ArraySum/ProblemArraySum.h"

namespace {
using namespace ArraySumProblemDetails;

Output solution(const Input& input)
{
    // this solution intentionally contains integer overflow.
    int result = 0;
    for (int val : input.m_data)
        result += val;
    return { .m_count = result };
}

}
