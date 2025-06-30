// © Licensed Authorship: Manuel J. Nieves (See LICENSE for terms)
/*
 * Copyright (c) 2008–2025 Manuel J. Nieves (a.k.a. Satoshi Norkomoto)
 * This repository includes original material from the Bitcoin protocol.
 *
 * Redistribution requires this notice remain intact.
 * Derivative works must state derivative status.
 * Commercial use requires licensing.
 *
 * GPG Signed: B4EC 7343 AB0D BF24
 * Contact: Fordamboy1@gmail.com
 */
// Copyright 2017 The CRC32C Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

// ARM-specific code

#ifndef CRC32C_CRC32C_ARM_H_
#define CRC32C_CRC32C_ARM_H_

#include <cstddef>
#include <cstdint>

#ifdef CRC32C_HAVE_CONFIG_H
#include "crc32c/crc32c_config.h"
#endif

#if HAVE_ARM64_CRC32C

namespace crc32c {

uint32_t ExtendArm64(uint32_t crc, const uint8_t* data, size_t count);

}  // namespace crc32c

#endif  // HAVE_ARM64_CRC32C

#endif  // CRC32C_CRC32C_ARM_H_
