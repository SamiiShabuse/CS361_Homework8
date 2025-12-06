#include "ns1d0.h"
#include <iostream>
#include <algorithm>
#include <vector>

/**
 * @brief Compute a modulo n, ensuring a non-negative result.
 * 
 * @param a The integer to be reduced.
 * @param n The modulus.
 * 
 * @return int 
 * 
 * @details This function ensures that the result of the modulo operation is always non-negative,
 * even if the input integer 'a' is negative. This is useful in contexts where a negative
 * modulo result would be undesirable or incorrect.
 */
static int mod(int a, int n) {
    int r = a % n;
    return r < 0 ? r + n : r;
}

/**
 *  Rule 1 (length) is just two general checks:
 *  prefix: length <= targetLength
 *  complete: length == targetLength
 * 
 * So we can keep that logic in is_valid_prefix and is_complete_valid_sequence for now
 */



/**
 * @brief Check if the sequence contains unique integers within the range [0, n-1].
 * 
 * @param seq The sequence of integers to check.
 * @param cfg Configuration containing the range limit 'n'.
 * 
 * @return true If all integers are unique and within the range [0, n-1].
 * 
 * @details This function checks that all integers in the sequence are unique and fall within the specified range [0, n-1]. 
 */
static bool rule_unique_and_in_range(const std::vector<int>& seq,
                                     const NS1D0Config& cfg) {
    const int n = cfg.n;
    std::vector<bool> seen(n, false);

    for (int v : seq) {
        if (v < 0 || v >= n) return false;  // out of range
        if (seen[v]) return false;          // duplicate
        seen[v] = true;
    }
    return true;
}


/**
 * @brief Check if the sequence starts with zero.
 * 
 * @param seq The sequence of integers to check.
 * @return true If the sequence starts with zero.
 * 
 * @details This function checks if the first element of the sequence is zero.
 */
static bool rule2_starts_with_zero(const std::vector<int>& seq) {
    return !seq.empty() && seq[0] == 0;
};


/**
 * @brief Check if the sequence ends with one (for complete sequences) or does not contain one prematurely (for prefixes).
 * 
 * @param seq The sequence of integers to check.
 * @param cfg Configuration containing the target length.
 * 
 * @return true If the sequence satisfies the rule.
 * 
 * @details For complete sequences, the last element must be 1.
 * For prefixes, the sequence must not contain 1 before the last position to ensure it can be placed at the end later.
 */
static bool rule3_one_at_end(const std::vector<int>& seq, const NS1D0Config& cfg) {
    if (static_cast<int>(seq.size()) == cfg.targetLength) {
        return !seq.empty() && seq.back() == 1;
    } else {
        // prefix case
        for (std::size_t i = 0; i + 1 < seq.size(); i++) {
            if (seq[i] == 1) return false;
        }
        return true;
    }
} ;

/**
 * @brief Check if the sequence does not contain the forbidden value.
 * 
 * @param seq The sequence of integers to check.
 * @param cfg Configuration containing the forbidden value.
 * 
 * @return true If the sequence does not contain the forbidden value.
 * 
 * @details This function checks if the sequence contains the forbidden value specified in the configuration.
 */
static bool rule4_no_forbidden(const std::vector<int>& seq, const NS1D0Config& cfg) {
    for (int v : seq) {
        if (v == cfg.forbidden) return false;
    }
    return true;
} ;

/**
 * @brief Check if the sequence satisfies the pair exclusion rule.
 * 
 * @param seq The sequence of integers to check.
 * @param cfg Configuration containing the range limit 'n'.
 * 
 * @return true If the sequence satisfies the pair exclusion rule.
 * 
 * @details For any 1 < x < n, the sequence can have either x or (1 - x) mod n, but not both.
 */
static bool rule5_pair_exclusion(const std::vector<int>& seq, const NS1D0Config& cfg) {
    const int n = cfg.n;
    std::vector<bool> present(n, false);

    for (int v : seq) {
        present[v] = true;
    }

    for (int v = 2; v < n; v++) {
        int partner = mod(1 - v, n);
        if (present[v] && present[partner]) {
            return false;
        }
    }
    return true;
} ;

/**
 * @brief Check if the sequence satisfies the unique differences pairs rule.
 * 
 * @param seq The sequence of integers to check.
 * @param cfg Configuration containing the range limit 'n'.
 * 
 * @return true If the sequence satisfies the unique differences pairs rule.
 * 
 * @details Define differences d_k = (a_k - a_{k-1}) mod n.
 * For each j = 1..n-1, only one of {j, -j mod n} may appear as a difference,
 * and each such pair may appear at most once.
 */
static bool rule6_differences_unique_pairs(const std::vector<int>& seq, const NS1D0Config& cfg) {
    const int n = cfg.n;
    std::vector<bool> usedPair(n, false); // only indices 1..(n-1)/2 will be used

    for (std::size_t i = 1; i < seq.size(); i++) {
        int diff = mod(seq[i] - seq[i - 1], n);
        if (diff == 0) {
            return false; // difference cannot be 0
        }
        int neg = mod(-diff, n);
        int rep = std::min(diff, neg);
        if (usedPair[rep]) {
            return false; // pair already used
        }
        usedPair[rep] = true;
    }
    return true;
} ;

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
bool is_valid_prefix(const std::vector<int>& seq, const NS1D0Config& cfg) {
    if (seq.empty()) {
        return false;
    }

    // Rule 1 (prefix length): cannot exceed target.
    if (static_cast<int>(seq.size()) > cfg.targetLength) {
        return false;
    }

    // Apply each rule / constraint:
    if (!rule2_starts_with_zero(seq)) return false;
    if (!rule_unique_and_in_range(seq, cfg)) return false;
    if (!rule3_one_at_end(seq, cfg)) return false;
    if (!rule4_no_forbidden(seq, cfg)) return false;
    if (!rule5_pair_exclusion(seq, cfg)) return false;
    if (!rule6_differences_unique_pairs(seq, cfg)) return false;

    return true;
}


/**
 * @brief Check whether a *complete* sequence is valid.
 * 
 * @param seq The sequence of integers to check.
 * @param cfg Configuration containing the target length and other parameters.
 * 
 * @return true If the sequence is complete and valid according to all rules.
 * 
 * @details This function checks if the given sequence is complete and satisfies all the rules defined in the configuration.
 */
static bool is_complete_valid_sequence(const std::vector<int>& seq,
                                       const NS1D0Config& cfg) {
    if (static_cast<int>(seq.size()) != cfg.targetLength) {
        return false;
    }
    return is_valid_prefix(seq, cfg);
}

/**
 * @brief Perform a depth-first search to find valid sequences.
 * 
 * @param currentSeq The current sequence being constructed.
 * @param cfg Configuration containing the target length and other parameters.
 * @param resultChannel Channel to send valid sequences found.
 * @param nodesExpanded Atomic counter for the number of nodes expanded during the search.
 * 
 * @return void
 * 
 * @details This function performs a depth-first search to find all valid sequences according to the rules defined in the configuration.
 */
static void dfs_search(std::vector<int>& currentSeq,
                       const NS1D0Config& cfg,
                       Channel<std::vector<int>>& resultChannel,
                       std::atomic<std::size_t>& nodesExpanded) {

    nodesExpanded.fetch_add(1, std::memory_order_relaxed);

    if (!is_valid_prefix(currentSeq, cfg)) {
        return; // prune
    }

    if (is_complete_valid_sequence(currentSeq, cfg)) {
        resultChannel.push(currentSeq);
        return;
    }

    // Still need more elements; try all candidates 0..n-1
    for (int candidate = 0; candidate < cfg.n; ++candidate) {
        // Enforce "1 only at the very end" here as well (extra safety)
        if (candidate == 1 &&
            static_cast<int>(currentSeq.size()) < cfg.targetLength - 1) {
            continue;
        }

        // Uniqueness + range + other rules will be enforced by is_valid_prefix,
        // but skipping obvious duplicates early is a cheap optimization
        if (std::find(currentSeq.begin(), currentSeq.end(), candidate) != currentSeq.end()) {
            continue;
        }

        currentSeq.push_back(candidate);
        dfs_search(currentSeq, cfg, resultChannel, nodesExpanded);
        currentSeq.pop_back();
    }
}

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
void search_worker(int workerIndex,
                   int workerCount,
                   const NS1D0Config& cfg,
                   Channel<std::vector<int>>& resultChannel,
                   std::atomic<std::size_t>& nodesExpanded) {
    std::vector<int> secondCandidates;

    for (int v = 0; v < cfg.n; ++v) {
        if (v == 0) continue;             // already at position 0
        if (v == cfg.forbidden) continue; // Rule 4
        if (v == 1 && cfg.targetLength > 2) {
            // Don't place 1 too early
            continue;
        }
        secondCandidates.push_back(v);
    }

    for (std::size_t i = 0; i < secondCandidates.size(); ++i) {
        if (static_cast<int>(i % workerCount) != workerIndex) continue;

        std::vector<int> seq;
        seq.reserve(cfg.targetLength);
        seq.push_back(0);                  // Rule 2
        seq.push_back(secondCandidates[i]);

        dfs_search(seq, cfg, resultChannel, nodesExpanded);
    }
}

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
void output_thread(Channel<std::vector<int>>& resultChannel,
                   std::ostream& out,
                   std::atomic<std::size_t>& sequencesFound) {
    std::vector<int> seq;
    while (resultChannel.pop(seq)) {
        sequencesFound.fetch_add(1, std::memory_order_relaxed);

        for (std::size_t i = 0; i < seq.size(); ++i) {
            out << seq[i];
            if (i + 1 < seq.size()) {
                out << ", ";
            }
        }
        out << '\n';
    }
}