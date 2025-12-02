#include "ns1d0.h"
#include <iostream>
#include <algorithm>

// Rules

void rule5();

void rule6();

bool is_valid_prefix(const std::vector<int>& seq, const NS1D0Config& cfg) {
    const int n = cfg.n;

    if (seq.empty()) {
        return false; // we always start with 0
    }

    // Rule 2: sequence must start with 0
    if (seq[0] != 0) {
        return false;
    }

    // Rule 1: length cannot exceed targetLength
    if (static_cast<int>(seq.size()) > cfg.targetLength) {
        return false;
    }

    // All integers must be unique
    for (std::size_t i = 0; i < seq.size(); i++) {

        if (seq[i] < 0 || seq[i] >= n) return false; // out of range
        
        for (std::size_t j = i + 1; j < seq.size(); j++) {
            if (seq[i] == seq[j]) return false;
        }
    }

    // Rule 4: cannot include ceil(n/2)
    for (int v : seq) {
        if (v == cfg.forbidden) return false;
    }

    // Rule 5

    // Rule 6

    return true;
}

static bool is_complete_valid_sequence(const std::vector<int>& seq,
                                       const NS1D0Config& cfg) {
    if (static_cast<int>)
}