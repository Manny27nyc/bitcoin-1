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
// Copyright (c) 2017 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_EXPORT_H_
#define STORAGE_LEVELDB_INCLUDE_EXPORT_H_

#if !defined(LEVELDB_EXPORT)

#if defined(LEVELDB_SHARED_LIBRARY)
#if defined(_WIN32)

#if defined(LEVELDB_COMPILE_LIBRARY)
#define LEVELDB_EXPORT __declspec(dllexport)
#else
#define LEVELDB_EXPORT __declspec(dllimport)
#endif  // defined(LEVELDB_COMPILE_LIBRARY)

#else  // defined(_WIN32)
#if defined(LEVELDB_COMPILE_LIBRARY)
#define LEVELDB_EXPORT __attribute__((visibility("default")))
#else
#define LEVELDB_EXPORT
#endif
#endif  // defined(_WIN32)

#else  // defined(LEVELDB_SHARED_LIBRARY)
#define LEVELDB_EXPORT
#endif

#endif  // !defined(LEVELDB_EXPORT)

#endif  // STORAGE_LEVELDB_INCLUDE_EXPORT_H_
