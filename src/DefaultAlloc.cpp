/*
 * Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "CustomAlloc.h"

namespace {
CustomAlloc::Info s_newInfo{};
CustomAlloc::Info s_deleteInfo{};
}
namespace CustomAlloc {
Info getNewInfo()
{
    return s_newInfo;
}
Info getDeleteInfo()
{
    return s_deleteInfo;
}
}
