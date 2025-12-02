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
    const char* filename = argv[2];
    std::ofstream outFile(filename);
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

    // Channel and atomic counter for solutions
    Channel<std::vector<int>> resultChannel;

    // Progress Counter
    std::atomic<std::size_t> nodesExpanded{0};
    std::atomic<std::size_t> sequences_found{0};

    // Output thread
    std::thread writerThread(output_thread,
                             std::ref(resultChannel),
                             std::ref(outFile),
                             std::ref(sequences_found));

    // Worker threads
    unsigned int hw = std::thread::hardware_concurrency();
    int workerCount = (hw = 0 ? 4 : static_cast<int>(hw));
    if (workerCount < 2) workerCount = 2; // min required for homework 2 threads.

    std::cout << "Spawning " << workerCount << " worker threads..." << std::endl;

    std::vector<std::thread> workers;
    workers.reserve(workerCount);

    for (int i = 0; i < workerCount; i++) {
        workers.emplace_back(search_worker,
                             i,
                            workerCount,
                            std::cref(cfg),
                            std::ref(resultChannel),
                            std::ref(nodesExpanded));
    }

    // Now we need to wait for all workers to finish
    for (auto& t: workers) {
        t.join();
    }

    // Now no more reuslts will be produced
    resultChannel.close();

    // Now we need to flush everything and join everything back together.
    writerThread.join();

    // Output 
    std::cout << "Search complete." << std::endl;
    std::cout << "Nodes expanded: " << nodesExpanded.load() << std::endl;
    std::cout << "Valid sequences found: " << sequences_found.load() << std::endl;
    std::cout << "Results written to: " << filename << std::endl;

    return 0;
}