#pragma once

#include "ProblemArraySum.h"

namespace {

const TestCaseList& getTests()
{
    static const TestCaseList s_tests{
        {
            .m_input{
                .m_data{ 1, 2, -4 },
            },
            .m_output{
                .m_value = -1,
            },
        },
        {
            .m_input{
                .m_data{ INT_MAX, INT_MAX, INT_MAX },
            },
            .m_output{
                .m_value = int64_t(INT_MAX) * 3,
            },
        },
    };
    return s_tests;
}
}
