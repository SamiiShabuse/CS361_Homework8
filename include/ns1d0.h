// the math and search logic 

#pragma once

#include <vector>
#include <atomic>
#include "channel.h"

// Configurations for NS1D0(n)
struct NS1D0Config {
    int n;              // modulus
    int targetLength;   // (n-1)/2+1
    int forbidden;      // ceil(n/2), which cannot appear
};

// Check whether a partial sequence is still valid w.r.t rules 2-6.
// logic is inside .cpp for now
bool is_valid_prefix(const std::vector<int>& seq, const NS1D0Config& cfg);

// A worker function: each thread searches a subset of the space
// and pushes completed valid sequences into resultChannel
void search_worker(
    int workerIndex,
    int workerCount,
    const NS1D0Config& cfg,
    Channel<std::vector<int>>& resultChannel,
    std::atomic<std::size_t>& nodesExpanded
);

// Output thread: consumes sequences from resultChannel and writes to file.
void output_thread(
    Channel<std::vector<int>>& resultChannel,
    std::ostream& out,
    std::atomic<std::size_t>& sequencesFound
);