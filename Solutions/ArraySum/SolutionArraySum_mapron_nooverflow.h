#pragma once

#include "Problems/ArraySum/ProblemArraySum.h"

#include <numeric>

namespace {

Output solution(const Input& input)
{
    const int64_t result = std::accumulate(input.m_data.cbegin(), input.m_data.cend(), int64_t(0), std::plus<int64_t>());

    return { .m_value = result };
}

}
