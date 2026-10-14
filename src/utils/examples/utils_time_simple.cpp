/**
 * @file utils_time_simple.cpp
 * @brief Demonstrates elapsed-time and timestamp utilities.
 *
 * PrecisionTimer uses a monotonic clock for elapsed measurements, while its
 * timestamp helper uses the system clock for human-readable local time.
 */

#include <cstdint>
#include <iomanip>
#include <iostream>

#include "utils/precise_timer.h"
#include "utils/time_utils.h"

int main()
{
    bitgraph::utils::PrecisionTimer timer;

    // Use a volatile accumulator so the small workload cannot be removed by
    // the optimizer before the timer reads the elapsed interval.
    volatile std::uint64_t checksum = 0;

    timer.wall_tic();
    for (std::uint64_t value = 0; value < 1000000; ++value) {
        checksum += value;
    }
    const double wall_seconds = timer.wall_toc();

    // cpu_tic()/cpu_toc() are retained compatibility names.  In the current
    // implementation they also measure elapsed monotonic time, not CPU usage.
    timer.cpu_tic();
    for (std::uint64_t value = 0; value < 1000000; ++value) {
        checksum += value;
    }
    const double cpu_seconds = timer.cpu_toc();

    std::cout << std::fixed << std::setprecision(9)
              << "wall elapsed seconds: " << wall_seconds << '\n'
              << "cpu elapsed seconds: " << cpu_seconds << '\n'
              << "checksum: " << checksum << '\n'
              << "local timestamp: "
              << bitgraph::utils::PrecisionTimer::local_timestamp()
              << '\n';

    // time_utils also converts standard chrono durations to seconds.
    const auto duration = std::chrono::milliseconds(125);
    std::cout << std::setprecision(3)
              << "125 milliseconds in seconds: "
              << bitgraph::utils::to_seconds(duration) << '\n';

    return 0;
}
