#include "ns1d0.h"
#include <iostream>
#include <algorithm>
#include <vector>

// Helpers: modular arithemtic

static int mod(int a, int n) {
    int r = a % n;
    return r < 0 ? r + n : r;
}


// Rule helpers 


/**
 *  Rule 1 (length) is just two general checks:
 *  prefix: length <= targetLength
 *  complete: length == targetLength
 * 
 * So we can keep that logic in is_valid_prefix and is_complete_valid_sequence for now
 */
void rule1();

// Extra Rule: uniqueness & range requirement
// "Ordered list of unique integers in [0, n-1]"
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


// Rule 2: sequence must start with 0
static bool rule2_starts_with_zero(const std::vector<int>& seq) {
    return !seq.empty() && seq[0] == 0;
};

/**
 * Rule 3: must end in 1.
 * For *complete* sequences only, last element mus be 1.
 * For prefixes, we must nesure we don't place 1 too early, ecause
 * all the values are unique. If 1 appears before teh last position, we cannot
 * place it at the end later.
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
 * Rule 4: cannot include ceil(n/2)
 */
static bool rule4_no_forbidden(const std::vector<int>& seq, const NS1D0Config& cfg) {
    for (int v : seq) {
        if (v == cfg.forbidden) return false;
    }
    return true;
} ;

/**
 * Rule 5: For any 1 < x < n you can have either x or (1-x mod n) but not both.
 *
 * Implementation idea:
 *  - Mark which values appear in the sequence.
 *  - For each value v>1, compute partner = (1 - v) mod n.
 *  - If both v and partner are present in the sequence → invalid.
 *
 * Note: v = 0 or v = 1 are ignored for this rule (as in the spec).
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
 * Rule 6: differences between consecutive elements.
 *
 * Define differences d_k = (a_k - a_{k-1}) mod n.
 * For each j = 1..n-1, only one of {j, -j mod n} may appear as a difference,
 * and each such pair may appear at most once.
 *
 * Implementation idea:
 *  - For each difference d, compute its "pair representative":
 *        neg = (-d) mod n = n-d
 *        rep = min(d, neg)
 *  For example (n=7):
 *      d=2 or d=5 both map to rep=2 (pair {2,5})
 *      d=3 or d=4 both map to rep=3 (pair {3,4})
 *      d=1 or d=6 both map to rep=1 (pair {1,6})
 *  - Maintain an array usedPair[0..n-1]; if rep has been seen before, rule 6
 *    is violated. 
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

// Check whether a *prefix* of a sequence is valid so far.
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
// Check whether a *complete* sequence is valid.
static bool is_complete_valid_sequence(const std::vector<int>& seq,
                                       const NS1D0Config& cfg) {
    if (static_cast<int>(seq.size()) != cfg.targetLength) {
        return false;
    }
    return is_valid_prefix(seq, cfg);
}

// DFS Search
static void dfs_search(std::vector<int>& currentSeq,
                       const NS1D0Config& cfg,
                       std::vector< std::vector<int> >& resultsChannel,
                       std::atomic<std::size_t>& nodesExpanded) {
    
}