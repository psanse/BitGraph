# Third-party notices

BitGraph is released under the terms described in the [License section of the README](README.md#license). The following third-party work is used in, or alongside, BitGraph.

| Component | Where | Source | Terms | Used for |
|---|---|---|---|---|
| **Logy** | adapted in [src/utils/logger.h](src/utils/logger.h); the previous version is kept in [src/utils/deprecated/logger_old.h](src/utils/deprecated/logger_old.h) | Giovanni Squillero, Politecnico di Torino, 2018. <https://github.com/squillero/logy> | CC0 1.0 (public domain dedication) | the logging macros |
| **Matrix Market I/O library** | adapted in [src/graph/formats/detail/mmio.h](src/graph/formats/detail/mmio.h) and [mmio.cpp](src/graph/formats/detail/mmio.cpp) | National Institute of Standards and Technology (NIST). <https://math.nist.gov/MatrixMarket/> | no license statement in the upstream sources; NIST states that software developed by its employees is not subject to copyright protection within the United States | reading and writing Matrix Market graph files |
| **GoogleTest** | not distributed; downloaded at build time by CMake | Google. <https://github.com/google/googletest> | BSD 3-Clause | the unit tests only, enabled by `BUILD_TESTING` |

## Notes

- **Logy.** The original is dedicated to the public domain. BitGraph's `logger.h` adapts Logy v1.2 and adds BitGraph-specific changes. Giovanni Squillero is credited in the file header.
- **Matrix Market I/O.** NIST asks users to keep its notice intact and to acknowledge NIST as the source of the software. NIST is acknowledged in the headers of `mmio.h` and `mmio.cpp`. NIST's statement concerns the United States only, and the upstream files carry no license text of their own.
- **GoogleTest** is a build-time dependency of the tests and is not part of the library or of any installed file.
- Last verified: 14 October 2026.
