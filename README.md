# CS361 - Homework 8
- Name: Samii Shabuse
- Email: sus24@drexel.edu
- Professor: Mark Boady

# Overview
This program generates all valid NS1D(n) sequences as described in the homework assignment specification. Given an odd integer `n > 1`, it searches the space of integer seqeunces of length: ` (n - 1) / 2 + 1` and we need to write all sequences that satisfy Rules 1-6 from the assignment.

The program uses multi-threaded parallel search with a thread pool, channels, and atomic counters to efficiently explore the search space and write valid sequences to an output file given by the user.

# How To Build
```bash

make
```

This will compile the program into
```bash

bin/sequence
```

# How To Run
```
./bin/sequence n output_file
```
Where:
- `n` is an odd integer > 1
- `output_file` is the file where the program wil store one NS1D0(n) sequences per lien.

**Example**
```bash
./bin/sequence 7 seq7.txt
```

Using the provided Makefile shortcuts:
```bash
make test7
make test9
make test11
make test13
```

Each runs:
```bash
./bin/sequence {n} seq{n}.txt
```

# Short Essay Questions

## Short Essay 1: How did you use concurrency to solve the problem?
I approached this assignment by treating NS1D0 sequence generation as a parallel search problem. By this I mean the search space splits naturally at the 
second element of the sequence of the first sequence. This is because the first element is always 0 by the rules. I used this structure to assign different 
subtrees of the search to different worker threads.

My concurrency design includes:

1. A Thread pool of workers
- I spawn `hardware_concurrency()` thread (minimum of 2 as required by the homework assignment).
- Each worker takes responsibility for certain second-element starting values using round-robin partitioning.
- Each worker performs a DFS on its subset of the search tree.
This all together help keep the worklaod balanced and ensures threads don't interfere with one another's partial sequences.

2. A channel for sending completed valid sequences
Thread do not write the file directly. Instead the workers push completed seuqences into a thread-safe `Channel<std::vector<int>>`, which I implemented using:
- a mutex
- a conditional variable
- a queue
- a `closed` flag
This matches the Producer-Consumer pattern and avoids contention on the output file.

3. A dedicated output thread (reduce step)
A single output thread consumes sequences from the channel and writes them to the outputfile.
This avoids:
- races on the file handle
- excessive locking
- interleaved or corrupted outputs

4. Atomic counters for progress tracking
I used:
- `std::atomic<size_t> nodesExpanded
- `std::atomic<size_t> sequencesFound

These track global progress while avoiding unnecessary locks.

5. CSP & Map-Reduce style concurrency
The final architecture resembles
- CSP (Communicating Sequential Processes): Workers communicate only through channels, not shared memory.
- Map-Reduce: Worker "map" the search space, the output thread "reduces" final results.

Overall concurrency allowed me to:
- parallelize the heavy search computation
- eliminate race conditions
- structure the program cleanly
- scale performance with available CPU cores
It helps transofrm what would be an extremely slow brute-force search into a VERY much more efficient multicore computation.

## Short Essay 2: How do your threads commmunicate? Why?
My threads communicate exclusively through a custom channel abstraction that is built using:
- `std::mutex`
- `std::condition_variable`
- `std::queue`
- a `closed` flag to signal shutdown.

Why I chose a channel instead of shared state:

1. Simplicity and correctness
Channels help enforce a clear protocol:
- Workers push completed sequences into the channel.
- The output threads pop sequences.
- No threads need to know about or protect shared data beyond the queue.
This significantly reduces chances of:
- race conditions
- deadlocks
- inconsistent state

2. Aligns with CS361 Topics
This communciation style was inspired directly by:
- Producer-Consumer problem
- Thread-safe queues
- Channels & CSP from Week 8 and Week 9
- Threaded Server architecture from Week 10

3. Decoupling
Workers do not block I/O. The output thread handles all the file operations. 
This helps prevent contention and keeps worker threaders focuse don CPU-heavy DFS search.

4. Controlled Shutdown
When all workers finish, the main thread calls `channel.close()`.
This wakes the output thread, lets it drain remaining work, and then exits cleanly. 

Using channels provided high-level, safe, and idiomatic communications between threads.
It prevents most concurrent pitfalls wile demonstrating the course's advanced messaging concepts.

## Short Essay 3: Waht is the largest value of `n` my program runs in a reasonable time?
My definition of "reasonable" is Completing in under 30 seconds on a standard multi-core machine.

Experiment Results
- `n = 7`: instant, produces exactly the two known sequences.

- `n = 9`: Computes very quickly (< 1s).

- `n = 11`: Runs in a reasonable time due to parallelism. Still manageable because pruning rules eliminate large parts of the search space.

- `n = 13`: This is near the complexity limit. Multithreading makes it feasible, but it may approach or exceed 30 seconds depending on machine speed and thread count.

So based on my results are the largest value of n that runs in a reasonable time is n=11 on my machine. 

The performance drops because the search treep expands rapidly as `n` grows.
Even with:
- pruning from Rule 5 and Rule 6
- multi-threading
- load-balancing DFS

The numbers of possible sequences grow combinatorially. However, parallelism mitigates the issue but cannot fully overcome exponential growth from the program.