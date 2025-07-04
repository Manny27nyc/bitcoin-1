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
# Copyright (c) 2015-2020 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
'''
Test script for security-check.py
'''
import os
import subprocess
import unittest

def write_testcode(filename):
    with open(filename, 'w', encoding="utf8") as f:
        f.write('''
    #include <stdio.h>
    int main()
    {
        printf("the quick brown fox jumps over the lazy god\\n");
        return 0;
    }
    ''')

def clean_files(source, executable):
    os.remove(source)
    os.remove(executable)

def call_security_check(cc, source, executable, options):
    subprocess.run([cc,source,'-o',executable] + options, check=True)
    p = subprocess.run(['./contrib/devtools/security-check.py',executable], stdout=subprocess.PIPE, universal_newlines=True)
    return (p.returncode, p.stdout.rstrip())

class TestSecurityChecks(unittest.TestCase):
    def test_ELF(self):
        source = 'test1.c'
        executable = 'test1'
        cc = 'gcc'
        write_testcode(source)

        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-zexecstack','-fno-stack-protector','-Wl,-znorelro','-no-pie','-fno-PIE', '-Wl,-z,separate-code']),
                (1, executable+': failed PIE NX RELRO Canary'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-znoexecstack','-fno-stack-protector','-Wl,-znorelro','-no-pie','-fno-PIE', '-Wl,-z,separate-code']),
                (1, executable+': failed PIE RELRO Canary'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-znoexecstack','-fstack-protector-all','-Wl,-znorelro','-no-pie','-fno-PIE', '-Wl,-z,separate-code']),
                (1, executable+': failed PIE RELRO'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-znoexecstack','-fstack-protector-all','-Wl,-znorelro','-pie','-fPIE', '-Wl,-z,separate-code']),
                (1, executable+': failed RELRO'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-znoexecstack','-fstack-protector-all','-Wl,-zrelro','-Wl,-z,now','-pie','-fPIE', '-Wl,-z,noseparate-code']),
                (1, executable+': failed separate_code'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-znoexecstack','-fstack-protector-all','-Wl,-zrelro','-Wl,-z,now','-pie','-fPIE', '-Wl,-z,separate-code']),
                (0, ''))

        clean_files(source, executable)

    def test_PE(self):
        source = 'test1.c'
        executable = 'test1.exe'
        cc = 'x86_64-w64-mingw32-gcc'
        write_testcode(source)

        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,--no-nxcompat','-Wl,--no-dynamicbase','-Wl,--no-high-entropy-va','-no-pie','-fno-PIE']),
            (1, executable+': failed DYNAMIC_BASE HIGH_ENTROPY_VA NX RELOC_SECTION'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,--nxcompat','-Wl,--no-dynamicbase','-Wl,--no-high-entropy-va','-no-pie','-fno-PIE']),
            (1, executable+': failed DYNAMIC_BASE HIGH_ENTROPY_VA RELOC_SECTION'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,--nxcompat','-Wl,--dynamicbase','-Wl,--no-high-entropy-va','-no-pie','-fno-PIE']),
            (1, executable+': failed HIGH_ENTROPY_VA RELOC_SECTION'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,--nxcompat','-Wl,--dynamicbase','-Wl,--high-entropy-va','-no-pie','-fno-PIE']),
            (1, executable+': failed RELOC_SECTION'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,--nxcompat','-Wl,--dynamicbase','-Wl,--high-entropy-va','-pie','-fPIE']),
            (0, ''))

        clean_files(source, executable)

    def test_MACHO(self):
        source = 'test1.c'
        executable = 'test1'
        cc = 'clang'
        write_testcode(source)

        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-no_pie','-Wl,-flat_namespace','-Wl,-allow_stack_execute','-fno-stack-protector']),
            (1, executable+': failed PIE NOUNDEFS NX LAZY_BINDINGS Canary CONTROL_FLOW'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-no_pie','-Wl,-flat_namespace','-Wl,-allow_stack_execute','-fstack-protector-all']),
            (1, executable+': failed PIE NOUNDEFS NX LAZY_BINDINGS CONTROL_FLOW'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-no_pie','-Wl,-flat_namespace','-fstack-protector-all']),
            (1, executable+': failed PIE NOUNDEFS LAZY_BINDINGS CONTROL_FLOW'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-no_pie','-fstack-protector-all']),
            (1, executable+': failed PIE LAZY_BINDINGS CONTROL_FLOW'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-no_pie','-Wl,-bind_at_load','-fstack-protector-all']),
            (1, executable+': failed PIE CONTROL_FLOW'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-no_pie','-Wl,-bind_at_load','-fstack-protector-all', '-fcf-protection=full']),
            (1, executable+': failed PIE'))
        self.assertEqual(call_security_check(cc, source, executable, ['-Wl,-pie','-Wl,-bind_at_load','-fstack-protector-all', '-fcf-protection=full']),
            (0, ''))

        clean_files(source, executable)

if __name__ == '__main__':
    unittest.main()

