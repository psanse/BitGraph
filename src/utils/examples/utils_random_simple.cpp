/**
 * @file utils_random_simple.cpp
 * @brief Demonstrates the UTILS uniform random generators.
 *
 * The integer and real generator handles share state for their distribution
 * type.  Seeding a handle makes the following sequence reproducible, which is
 * useful for examples, tests, and repeatable experiments.
 */

#include <iomanip>
#include <iostream>
#include <vector>

#include "utils/random_utils.h"

int main()
{
    /** @brief Exposes the integer uniform random generator in the current namespace. */
    using bitgraph::utils::detail::iugen;

    /** @brief Exposes the real uniform random generator in the current namespace. */
    using bitgraph::utils::detail::rugen;

    // The current public random facade exposes probability trials directly;
    // the typed generator handles provide integer and real samples.
    iugen integer_generator;
    iugen::set_range(1, 6);
    iugen::seed(2026);

    std::vector<int> first_rolls;
    for (int count = 0; count < 5; ++count) {
        first_rolls.push_back(integer_generator());
    }

    // Reseeding the shared integer generator reproduces the same sequence
    iugen::seed(2026);
    std::vector<int> repeated_rolls;
    for (int count = 0; count < 5; ++count) {
        repeated_rolls.push_back(integer_generator());
    }

    std::cout << "integer samples in [1, 6]: ";
    for (const auto value : first_rolls) {
        std::cout << value << ' ';
    }
    std::cout << "\nrepeated with the same seed: ";
    for (const auto value : repeated_rolls) {
        std::cout << value << ' ';
    }
    std::cout << "\nsequences match: "
              << std::boolalpha << (first_rolls == repeated_rolls) << "\n\n";

    // Uniform real distributions use a half-open interval [min, max).
    rugen real_generator;
    rugen::set_range(0.0, 1.0);
    rugen::seed(2026);

    std::cout << std::fixed << std::setprecision(4)
              << "real samples in [0, 1): ";
    for (int count = 0; count < 3; ++count) {
        std::cout << real_generator() << ' ';
    }
    std::cout << "\n\n";

    // uniform_dist(p) performs a Bernoulli trial using the shared real
    // generator and returns true when the sampled value is below p.
    rugen::seed(2026);
    int successes = 0;
    for (int count = 0; count < 10; ++count) {
        successes += bitgraph::utils::uniform_dist(0.5) ? 1 : 0;
    }

    std::cout << "successes in 10 trials with p = 0.5: "
              << successes << '\n';

    return 0;
}
