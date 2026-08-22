# CS361 Homework 8: Parallel NS1D Sequence Search

A C++ concurrency assignment that generates valid NS1D(n) sequences using parallel depth-first search, worker threads, a channel abstraction, and producer-consumer output.

## Overview

Given an odd integer `n > 1`, the program searches for integer sequences of length `(n - 1) / 2 + 1` that satisfy the assignment's NS1D rules. The search space grows quickly, so the implementation parallelizes the work across multiple threads and writes valid sequences to an output file.

## Concurrency Design

The program is structured around a map/reduce-style workflow:

1. Worker threads split the search space by assigning different starting branches.
2. Each worker performs DFS over its subset of the search tree.
3. Valid sequences are sent through a thread-safe channel.
4. A dedicated output thread consumes completed sequences and writes them to disk.
5. Atomic counters track global progress without unnecessary locking.

Key concurrency concepts demonstrated:

- Worker thread pool
- Producer-consumer communication
- Channel abstraction using `std::mutex`, `std::condition_variable`, and `std::queue`
- Atomic counters for shared progress tracking
- Reduced file I/O contention through a single writer thread

## Repository Layout

```text
.
|-- Makefile
|-- include/
|   |-- channel.h
|   `-- ns1d0.h
|-- src/
|   |-- main.cpp
|   `-- ns1d0.cpp
`-- sequence.pdf
```

## Build

```bash
make
```

This compiles the program to:

```text
bin/sequence
```

## Run

```bash
./bin/sequence n output_file
```

Example:

```bash
./bin/sequence 7 seq7.txt
```

Makefile shortcuts:

```bash
make test7
make test9
make test11
make test13
```

## Performance Notes

The largest value that ran in a reasonable time on the original test machine was `n = 11`. The search space grows combinatorially, so parallelism helps but does not eliminate the underlying exponential growth.

## Why This Matters

This project is useful portfolio evidence for systems and quant-oriented engineering because it demonstrates practical C++ concurrency: splitting CPU-heavy work, avoiding shared-state races, coordinating threads through channels, and isolating file output behind a single consumer.