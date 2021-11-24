#include <chrono>
#include <iostream>
#include <math.h>
#include <random>
#include <unistd.h>

#include "stages/test/fixtures.h"

using namespace std;
using namespace chrono;
using namespace stages;

using timer = high_resolution_clock;

template<typename T>
void use(T &&t) {
  __asm__ __volatile__ ("" :: "g" (t));
}

template<typename T>
time_t time(T code, size_t iterations) {
  auto start = timer::now();
  while (iterations--) {
    use(code());
  }
  auto end = timer::now();
  return duration_cast<nanoseconds>(end - start).count();
}

template<typename T>
size_t pick_iters(T code) {
  size_t iterations;
  for (iterations = 1; iterations < 1e9; iterations *= 10) {
    time_t t = time(code, iterations);
    if (t > 10e6) {
      return 10 * iterations;
    }
  }
  return iterations;
}

template<typename T>
void timeit(T code, size_t runs) {
  double min = INT64_MAX;
  double mean = 0;
  size_t iterations = pick_iters(code);
  for (size_t i=0; i<runs; i++) {
    time_t t = time(code, iterations);
    printf("run %ld = %ld\n", i, t);
    mean += t;
    if (t < min) {
      min = t;
    }
  }
  min /= iterations;
  mean = mean / iterations / runs;

  if (min < 1e3) {
    printf("%ld runs of %ld iterations; mean %0.3fns, min %0.3fns\n", runs, iterations, mean, min);
  } else if (min < 1e6) {
    printf("%ld runs of %ld iterations; mean %0.3fus, min %0.3fus\n", runs, iterations, mean / 1e3, min / 1e3);
  } else if (min < 1e9) {
    printf("%ld runs of %ld iterations; mean %0.3fms, min %0.3fms\n", runs, iterations, mean / 1e6, min / 1e6);
  } else {
    printf("%ld runs of %ld iterations; mean %0.3fs, min %0.3fs\n", runs, iterations, mean / 1e9, min / 1e9);

  }
}

void TimeTapLFO() {
  cout << "Tap LFO" << endl;
  timeit([] {
    SegmentGeneratorTest t;
    segment::Configuration configuration = { segment::TYPE_RAMP, true };
    t.generator()->Configure(true, &configuration, 1);
    for (int i = 0; i < 24; ++i) {
      t.pulses()->AddPulses(1500, 500, 6);
      t.pulses()->AddPulses(3000, 500, 2);
    }

    t.set_segment_parameters(0, 0.5f, 0.5f);
    const size_t size = 8;
    while (!t.pulses()->empty()) {
      GateFlags flags[size];
      t.pulses()->Render(flags, 1);
      SegmentGenerator::Output out[size];

      t.generator()->Process(flags, out, size);
    }
    return 0;
  }, 3);

}

int main() {
  TimeTapLFO();
}