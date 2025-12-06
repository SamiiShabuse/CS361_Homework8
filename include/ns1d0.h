/**
 * @file include/ns1d0.h
 * 
 * @brief Implementation of NS1D0 sequence search functions.
 * 
 * @details This file contains the core logic for validating and searching NS1D0 sequences,
 * including rule checks and worker thread functions. It implements the core algorithmic
 * components required for the NS1D0 sequence search. It uses multiple threads to perform 
 * the search concurrently.
 */

#pragma once

#include <vector>
#include <atomic>
#include "channel.h"

/**
 * @struct NS1D0Config
 * 
 * @brief Configuration parameters for the NS1D0 problem.
 * 
 * @var n The modulus used in the problem.
 * @var targetLength The target length of the sequence, typically (n-1)/2 + 1.
 * @var forbidden The forbidden value, usually ceil(n/2), which cannot appear in the sequence.
 * 
 * @details This struct holds the configuration parameters for the NS1D0 problem,
 * including the modulus 'n', the target length of the sequence, and the forbidden value.
 */
struct NS1D0Config {
    int n;              // modulus
    int targetLength;   // (n-1)/2+1
    int forbidden;      // ceil(n/2), which cannot appear
};

/**
 * @brief Check whether a *prefix* of a sequence is valid so far.
 * 
 * @param seq The sequence of integers to check.
 * @param cfg Configuration containing the target length and other parameters.
 * 
 * @return true If the prefix is valid according to all rules.
 * 
 * @details This function checks if the given sequence prefix satisfies all the rules defined in the configuration.
 */
bool is_valid_prefix(const std::vector<int>& seq, const NS1D0Config& cfg);

/**
 * @brief Worker function for searching valid sequences.
 * 
 * @param workerIndex The index of this worker thread.
 * @param workerCount The total number of worker threads.
 * @param cfg Configuration containing the target length and other parameters.
 * @param resultChannel Channel to send valid sequences found.  
 * @param nodesExpanded Atomic counter for the number of nodes expanded during the search.
 * 
 * @return void
 * 
 * @details This function is executed by each worker thread to search for valid sequences. 
 */
void search_worker(
    int workerIndex,
    int workerCount,
    const NS1D0Config& cfg,
    Channel<std::vector<int>>& resultChannel,
    std::atomic<std::size_t>& nodesExpanded
);

/**
 * @brief Thread function for outputting valid sequences.
 * 
 * @param resultChannel Channel from which to receive valid sequences.
 * @param out Output stream to write the sequences.
 * @param sequencesFound Atomic counter for the number of sequences found.
 * 
 * @return void
 * 
 * @details This function runs in a separate thread to output valid sequences as they are found.
 */
void output_thread(
    Channel<std::vector<int>>& resultChannel,
    std::ostream& out,
    std::atomic<std::size_t>& sequencesFound
);