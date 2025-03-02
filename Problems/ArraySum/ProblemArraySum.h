#pragma once

#include "CommonProblemTypes.h"

inline namespace PROBLEM_NAMESPACE {

using Input = CommonTypes::ArrayIO<int>;

using Output = CommonTypes::NumericScalarIO<int64_t>;

using TestCaseList = CommonTypes::TestCaseList<Input, Output>;

}
