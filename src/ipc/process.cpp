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
// Copyright (c) 2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <fs.h>
#include <ipc/process.h>
#include <ipc/protocol.h>
#include <mp/util.h>
#include <tinyformat.h>
#include <util/strencodings.h>

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <system_error>
#include <unistd.h>
#include <utility>
#include <vector>

namespace ipc {
namespace {
class ProcessImpl : public Process
{
public:
    int spawn(const std::string& new_exe_name, const fs::path& argv0_path, int& pid) override
    {
        return mp::SpawnProcess(pid, [&](int fd) {
            fs::path path = argv0_path;
            path.remove_filename();
            path.append(new_exe_name);
            return std::vector<std::string>{path.string(), "-ipcfd", strprintf("%i", fd)};
        });
    }
    int waitSpawned(int pid) override { return mp::WaitProcess(pid); }
    bool checkSpawned(int argc, char* argv[], int& fd) override
    {
        // If this process was not started with a single -ipcfd argument, it is
        // not a process spawned by the spawn() call above, so return false and
        // do not try to serve requests.
        if (argc != 3 || strcmp(argv[1], "-ipcfd") != 0) {
            return false;
        }
        // If a single -ipcfd argument was provided, return true and get the
        // file descriptor so Protocol::serve() can be called to handle
        // requests from the parent process. The -ipcfd argument is not valid
        // in combination with other arguments because the parent process
        // should be able to control the child process through the IPC protocol
        // without passing information out of band.
        if (!ParseInt32(argv[2], &fd)) {
            throw std::runtime_error(strprintf("Invalid -ipcfd number '%s'", argv[2]));
        }
        return true;
    }
};
} // namespace

std::unique_ptr<Process> MakeProcess() { return std::make_unique<ProcessImpl>(); }
} // namespace ipc
