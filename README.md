# BitGraph

This repo contains a refactoring of the **BITSCAN** and **GRAPH** libraries for efficient operations with bitsets and graphs in modern C++. The **BITSCAN** library is devoted to **bitset manipulation**. The **GRAPH** library is dedicated to **efficient graph operations** and uses BITSCAN to represent graphs in memory. The repo contains additional utilities (library UTILS) and **many tests and examples**.

The C++ BITSCAN and GRAPH libraries result from **more than 20 years of research in combinatorial optimization**. They are at the core of many exact state-of-the-art algorithms for hard combinatorial problems and their applications, such as:

*   [CliReg: Clique-based robust Point Cloud Registration, 2025](https://ieeexplore.ieee.org/abstract/document/10892261).
    
*   [CliSAT: A new exact algorithm for hard maximum clique problems, 2023](https://www.sciencedirect.com/science/article/pii/S0377221722008165).
    
*   [A new branch-and-filter exact algorithm for binary constraint satisfaction problems, 2022](https://www.sciencedirect.com/science/article/pii/S0377221722008165).
    
*   [A new combinatorial branch-and-bound algorithm for the Knapsack Problem with Conflicts, 2021](https://www.sciencedirect.com/science/article/pii/S0377221720306342).
    
*   [A branch-and-cut algorithm for the Edge Interdiction Clique Problem, 2021](https://www.sciencedirect.com/science/article/pii/S0377221721000606).
    
*   [The maximum clique interdiction problem, 2019](https://www.sciencedirect.com/science/article/pii/S0377221719301572).
    
*   [A new branch-and-bound algorithm for the maximum edge-weighted clique problem, 2019](https://www.sciencedirect.com/science/article/pii/S0377221719303054).
    
*   [A new branch-and-bound algorithm for the Maximum Weighted Clique Problem, 2019](https://www.sciencedirect.com/science/article/pii/S0305054819301303?casa_token=G7GLiCVG0xsAAAAA:pEzTarl2ldBHgkG7PSp9s4Mg-idPrMXgaBkD22mTy9Xn20YrBd7rEPA7Xuoiljq6aekFwrlEeQ).
    
*   [Efficiently enumerating all maximal cliques with bit-parallelism, 2018](https://www.sciencedirect.com/science/article/pii/S0305054817302988?casa_token=OqTK-OLIIhgAAAAA:aL1pUr8qPNJeYqPqJn6xLiWeEcsBQxeaPL7Sev3MJa3Dk0Mm_ZxjXxVd9XDKkqL8RsUMIxjK6A).
    
*   [A new exact maximum clique algorithm for large and massive sparse graphs, 2016](https://www.sciencedirect.com/science/article/pii/S0305054815001884).
    
*   [A novel clique formulation for the visual feature matching problem,2015](https://link.springer.com/article/10.1007/s10489-015-0646-1).
    
*   [A new DSATUR-based algorithm for exact vertex coloring, 2012](https://www.sciencedirect.com/science/article/pii/S0305054811002966?casa_token=5udkYhvrMEsAAAAA:ygKzz7_FVTd832UxriFawzx1E26R6g6vWGb-Nx5gUMHZpOofl4GnZURf0pEMMMs6JEkBPilZqA).
    
*   [An exact bit-parallel algorithm for the maximum clique problem, 2011](https://www.sciencedirect.com/science/article/pii/S0305054810001504?casa_token=ss__gBPlWVIAAAAA:-slFv5Gkx-DGHDovu8oguVEqO-eP9dl8xQtQjuUKP8VoOmANOVMh4DoJi6Jj_kkC5xmQ-fIJNg).
    

and many others.

### System platforms

The repo has been tested on Linux (24.x.x) and Windows 10 OS.

## Releases 
The current release is v1.1.0 and is still compatible with C++11 except for googletest, which requires C++14. In future releases compilation will be restricted to C++14.

## Acknowledgements

This research has been partially funded by the Spanish Ministry of Economy and Competitiveness (MINECO), national grants DPI 2010-21247-C02-01, DPI 2014-53525-C3-1-R and DPI2017-86915-C3-3-R.
