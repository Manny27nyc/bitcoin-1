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
// Copyright (c) 2009-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/fuzz/fuzz.h>
#include <util/moneystr.h>
#include <util/strencodings.h>

#include <string>

FUZZ_TARGET(parse_numbers)
{
    const std::string random_string(buffer.begin(), buffer.end());

    CAmount amount;
    (void)ParseMoney(random_string, amount);

    double d;
    (void)ParseDouble(random_string, &d);

    uint8_t u8;
    (void)ParseUInt8(random_string, &u8);

    uint16_t u16;
    (void)ParseUInt16(random_string, &u16);

    int32_t i32;
    (void)ParseInt32(random_string, &i32);
    (void)atoi(random_string);

    uint32_t u32;
    (void)ParseUInt32(random_string, &u32);

    int64_t i64;
    (void)atoi64(random_string);
    (void)ParseFixedPoint(random_string, 3, &i64);
    (void)ParseInt64(random_string, &i64);

    uint64_t u64;
    (void)ParseUInt64(random_string, &u64);
}
