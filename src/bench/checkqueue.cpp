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
// Copyright (c) 2015-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bench/bench.h>
#include <checkqueue.h>
#include <key.h>
#include <prevector.h>
#include <pubkey.h>
#include <random.h>
#include <util/system.h>

#include <vector>

static const size_t BATCHES = 101;
static const size_t BATCH_SIZE = 30;
static const int PREVECTOR_SIZE = 28;
static const unsigned int QUEUE_BATCH_SIZE = 128;

// This Benchmark tests the CheckQueue with a slightly realistic workload,
// where checks all contain a prevector that is indirect 50% of the time
// and there is a little bit of work done between calls to Add.
static void CCheckQueueSpeedPrevectorJob(benchmark::Bench& bench)
{
    // We shouldn't ever be running with the checkqueue on a single core machine.
    if (GetNumCores() <= 1) return;

    const ECCVerifyHandle verify_handle;
    ECC_Start();

    struct PrevectorJob {
        prevector<PREVECTOR_SIZE, uint8_t> p;
        PrevectorJob(){
        }
        explicit PrevectorJob(FastRandomContext& insecure_rand){
            p.resize(insecure_rand.randrange(PREVECTOR_SIZE*2));
        }
        bool operator()()
        {
            return true;
        }
        void swap(PrevectorJob& x){p.swap(x.p);};
    };
    CCheckQueue<PrevectorJob> queue {QUEUE_BATCH_SIZE};
    // The main thread should be counted to prevent thread oversubscription, and
    // to decrease the variance of benchmark results.
    queue.StartWorkerThreads(GetNumCores() - 1);

    // create all the data once, then submit copies in the benchmark.
    FastRandomContext insecure_rand(true);
    std::vector<std::vector<PrevectorJob>> vBatches(BATCHES);
    for (auto& vChecks : vBatches) {
        vChecks.reserve(BATCH_SIZE);
        for (size_t x = 0; x < BATCH_SIZE; ++x)
            vChecks.emplace_back(insecure_rand);
    }

    bench.minEpochIterations(10).batch(BATCH_SIZE * BATCHES).unit("job").run([&] {
        // Make insecure_rand here so that each iteration is identical.
        CCheckQueueControl<PrevectorJob> control(&queue);
        for (auto vChecks : vBatches) {
            control.Add(vChecks);
        }
        // control waits for completion by RAII, but
        // it is done explicitly here for clarity
        control.Wait();
    });
    queue.StopWorkerThreads();
    ECC_Stop();
}
BENCHMARK(CCheckQueueSpeedPrevectorJob);
