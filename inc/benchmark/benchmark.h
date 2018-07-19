//
// Created by Daniel on 18/5/2018.
//

#ifndef BENCHMARK_BENCHMARK_H
#define BENCHMARK_BENCHMARK_H

#include <libsc/system.h>

/**
 * A class to test the speed of a routine or procedure.
 *
 * @example
 * @code
 * Benchmark benchmark(myFunction);
 * benchmark.test(10000);
 *
 * uint32_t total_run_time = benchmark.stopwatch; //e.g. 10ms for 10000 runs
 * uint32_t average_run_time = benchmark.average; //e.g. 1ns per run
 * @endcode
 */
class Benchmark {
public:
    /**
     * Record of total run time in ms of running the routine a provided number of times after running test().
     */
    libsc::Timer::TimerInt stopwatch;

    /**
     * Average run time in ns of running the routine once after running test().
     */
    uint32_t average;

    /**
     * Constructor of Benchmark
     * @param routine function pointer to a void function.
     */
    explicit Benchmark(void (*routine)());

    /**
     * Test the routine and save result to stopwatch
     * @param iterations the number of times the routine is ran.
     */
    void test(uint32_t iterations);

private:
    /**
     * Function pointer to the provided void function
     */
    void (*routine)() = nullptr;
};

#endif //BENCHMARK_BENCHMARK_H
