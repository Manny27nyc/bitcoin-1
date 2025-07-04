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
#!/usr/bin/env python3
# Copyright (c) 2017-2020 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import argparse

parser = argparse.ArgumentParser(description='Remove the coverage data from a tracefile for all files matching the pattern.')
parser.add_argument('--pattern', '-p', action='append', help='the pattern of files to remove', required=True)
parser.add_argument('tracefile', help='the tracefile to remove the coverage data from')
parser.add_argument('outfile', help='filename for the output to be written to')

args = parser.parse_args()
tracefile = args.tracefile
pattern = args.pattern
outfile = args.outfile

in_remove = False
with open(tracefile, 'r', encoding="utf8") as f:
    with open(outfile, 'w', encoding="utf8") as wf:
        for line in f:
            for p in pattern:
                if line.startswith("SF:") and p in line:
                    in_remove = True
            if not in_remove:
                wf.write(line)
            if line == 'end_of_record\n':
                in_remove = False
