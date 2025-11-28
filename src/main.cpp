#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <atomic>

#include "../include/ns1d0.h"
#include "../include/channel.h"

int main(int argc, char* argv[]) {

    // Input checkers
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <n> <output_file>" << std::endl;
        return 1;
    }
    // Parse n input
    int n = std::atoi(argv[1]);
    if (n <= 1 || (n % 2) != 1) {
        std::cerr << "Error: n must be an odd integer greater than 1." << std::endl;
        return 1;
    }
    // Open output file
    std::ofstream outFile(argv[2]);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return 1;
    }

    // Configuration setup
    NS1D0Config cfg;
    cfg.n = n;
    cfg.targetLength = (n - 1) / 2 + 1;
    cfg.forbidden = (n + 1) / 2; // ceil(n/2) since n is odd

    std::cout << "NS1D0(" << n << ") search" << std::endl;
    std::cout << "Target sequence length: " << cfg.targetLength << std::endl;
    std::cout << "Forbidden value (ceil(n/2)): " << cfg.forbidden << std::endl;
}