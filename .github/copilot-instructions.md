# Project overview

This is a C++ project implementing exact algorithms for the
Maximum Clique Problem.

## Architecture

- BITSCAN provides bit-parallel data structures.
- GRAPH provides graph representations.
- BBMCX implements branch-and-bound maximum clique.
- CliSAT extends BBMC with SAT-based pruning.
- pmSAT implements partial MaxSAT reasoning.

## Coding conventions

- Use C++14.
- Code must compile with MSVC and GCC.
- Prefer bit-parallel operations.
- Avoid dynamic memory allocation inside the search.
- Preserve the existing naming conventions.
- Do not modify public APIs unless explicitly requested.

## Build

The project uses CMake.

Visual Studio builds should use MSVC.

## Tests

Use GoogleTest for unit tests.