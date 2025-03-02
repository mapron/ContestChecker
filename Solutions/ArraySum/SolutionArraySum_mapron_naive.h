#pragma once

#include "Problems/ArraySum/ProblemArraySum.h"

namespace {

Output solution(const Input& input)
{
    // this solution intentionally contains integer overflow.
    int result = 0;
    for (int val : input.m_data)
        result += val;
    return { .m_value = result };
}

}
