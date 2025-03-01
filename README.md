# BITSCAN_plus
Refactoring the BITSCAN and GRAPH libraries for efficient operations with bitsets in modern C++.
The BITSCAN library is dedicated to bitset manipulation. The GRAPH library is dedicated to efficient graph operations, and uses BITSCAN to represent graphs in memory.  The repo also contains additional utilities (library UTILS), which have also been refactored, and many tests and examples.

BITSCAN and GRAPH have been tested in LINUX and Windows OS.  The current versions are: BITSCAN 1.0, UTILS 1.0 and GRAPH 1.0.

The BITSCAN and GRAPH libraries are at the core of many exact state-of-the-art algorithms for hard combinatorial problems and their applications, such as:
- [CliReg: Clique-based robust Point Cloud Registration, 2025](https://ieeexplore.ieee.org/abstract/document/10892261).
- [CliSAT: A new exact algorithm for hard maximum clique problems, 2023](https://www.sciencedirect.com/science/article/pii/S0377221722008165).
- [A new branch-and-filter exact algorithm for binary constraint satisfaction problems, 2022](https://www.sciencedirect.com/science/article/pii/S0377221722008165).
- [A new combinatorial branch-and-bound algorithm for the Knapsack Problem with Conflicts, 2021](https://www.sciencedirect.com/science/article/pii/S0377221720306342).
- [A branch-and-cut algorithm for the Edge Interdiction Clique Problem, 2021](https://www.sciencedirect.com/science/article/pii/S0377221721000606).
- [The maximum clique interdiction problem, 2019](https://www.sciencedirect.com/science/article/pii/S0377221719301572).
- [A new branch-and-bound algorithm for the maximum edge-weighted clique problem, 2019](https://www.sciencedirect.com/science/article/pii/S0377221719303054).
- [A new branch-and-bound algorithm for the Maximum Weighted Clique Problem, 2019](https://www.sciencedirect.com/science/article/pii/S0305054819301303?casa_token=G7GLiCVG0xsAAAAA:pEzTarl2ldBHgkG7PSp9s4Mg-idPrMXgaBkD22mTy9Xn20YrBd7rEPA7Xuoiljq6aekFwrlEeQ).
- [Efficiently enumerating all maximal cliques with bit-parallelism, 2018](https://www.sciencedirect.com/science/article/pii/S0305054817302988?casa_token=OqTK-OLIIhgAAAAA:aL1pUr8qPNJeYqPqJn6xLiWeEcsBQxeaPL7Sev3MJa3Dk0Mm_ZxjXxVd9XDKkqL8RsUMIxjK6A).

and many others. 


