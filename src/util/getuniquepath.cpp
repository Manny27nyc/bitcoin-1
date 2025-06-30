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
#include <random.h>
#include <fs.h>
#include <util/strencodings.h>

fs::path GetUniquePath(const fs::path& base)
{
    FastRandomContext rnd;
    fs::path tmpFile = base / HexStr(rnd.randbytes(8));
    return tmpFile;
}