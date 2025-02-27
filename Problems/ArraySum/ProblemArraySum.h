/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#pragma once

#include "CommonProblemTypes.h"

namespace ArraySumProblemDetails {

using Input = CommonDetails::IntegralArrayIO<int>;

using Output = CommonDetails::IntegralScalar<int64_t>;

using TestCaseList = CommonDetails::IOTransform<Input, Output>::TestCaseList;

}
