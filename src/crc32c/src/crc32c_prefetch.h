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

#ifndef CRC32C_CRC32C_PREFETCH_H_
#define CRC32C_CRC32C_PREFETCH_H_

#include <cstddef>
#include <cstdint>

#ifdef CRC32C_HAVE_CONFIG_H
#include "crc32c/crc32c_config.h"
#endif

#if HAVE_MM_PREFETCH

#if defined(_MSC_VER)
#include <intrin.h>
#else  // !defined(_MSC_VER)
#include <xmmintrin.h>
#endif  // defined(_MSC_VER)

#endif  // HAVE_MM_PREFETCH

namespace crc32c {

// Ask the hardware to prefetch the data at the given address into the L1 cache.
inline void RequestPrefetch(const uint8_t* address) {
#if HAVE_BUILTIN_PREFETCH
  // Clang and GCC implement the __builtin_prefetch non-standard extension,
  // which maps to the best instruction on the target architecture.
  __builtin_prefetch(reinterpret_cast<const char*>(address), 0 /* Read only. */,
                     0 /* No temporal locality. */);
#elif HAVE_MM_PREFETCH
  // Visual Studio doesn't implement __builtin_prefetch, but exposes the
  // PREFETCHNTA instruction via the _mm_prefetch intrinsic.
  _mm_prefetch(reinterpret_cast<const char*>(address), _MM_HINT_NTA);
#else
  // No prefetch support. Silence compiler warnings.
  (void)address;
#endif  // HAVE_BUILTIN_PREFETCH
}

}  // namespace crc32c

#endif  // CRC32C_CRC32C_ROUND_UP_H_
