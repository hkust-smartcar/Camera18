//
// Created by Daniel on 18/5/2018.
//

#include <benchmark/benchmark.h>

Benchmark::Benchmark(void (*routine)()) {
    this->routine = routine;
}

void Benchmark::test(uint32_t iterations) {
    this->stopwatch = libsc::System::Time();

    for (uint32_t i = iterations; i > 0; i--) {
        this->routine();
    }

    this->stopwatch = libsc::System::Time() - this->stopwatch;
    this->average = this->stopwatch * 1000 / iterations;
}