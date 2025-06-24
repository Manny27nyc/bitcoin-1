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
/**********************************************************************
 * Copyright (c) 2013, 2014 Pieter Wuille                             *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#ifndef SECP256K1_NUM_REPR_H
#define SECP256K1_NUM_REPR_H

#include <gmp.h>

#define NUM_LIMBS ((256+GMP_NUMB_BITS-1)/GMP_NUMB_BITS)

typedef struct {
    mp_limb_t data[2*NUM_LIMBS];
    int neg;
    int limbs;
} secp256k1_num;

#endif /* SECP256K1_NUM_REPR_H */
