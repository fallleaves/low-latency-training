#include <benchmark/benchmark.h>

#include <cstdlib>
#include <list>
#include <numeric>
#include <vector>

void fill(std::list<int> &container, std::size_t elements)
{
  for (int i = 0; i < elements; ++i)
  {
    container.push_back(std::rand());
  }
}

void fill(std::vector<int> &container, std::size_t elements)
{
  container.reserve(elements);

  for (int i = 0; i < elements; ++i)
  {
    container.push_back(std::rand());
  }
}

void fillIndex(std::vector<int> &container, std::size_t elements)
{
  container.reserve(elements);

  for (int i = 0; i < elements; ++i)
  {
    container.push_back(i);
  }
}

void shuffleIndex(std::vector<int> &container)
{
  std::random_shuffle(container.begin(), container.end());
}

template <typename Container>
static void accumulate(benchmark::State &state)
{
  Container container;
  Container index;

  auto elements = state.range(0);

  fill(container, elements);
  fillIndex(index, elements);

  int sum = 0;

  for (auto _ : state)
  {
    for (auto i : index)
    {
      sum += container[i];
    }

    benchmark::DoNotOptimize(&sum);
  }

  state.SetBytesProcessed(elements * sizeof(int) * state.iterations() * 2);
  state.counters["data"] = elements * sizeof(int);
}

template <typename Container>
static void accumulateRandom(benchmark::State &state)
{
  Container container;
  Container index;

  auto elements = state.range(0);

  fill(container, elements);
  fillIndex(index, elements);
  shuffleIndex(index);

  int sum = 0;

  for (auto _ : state)
  {
    for (auto i : index)
    {
      sum += container[i];
    }

    benchmark::DoNotOptimize(&sum);
  }

  state.SetBytesProcessed(elements * sizeof(int) * state.iterations() * 2);
  state.counters["data"] = elements * sizeof(int);
}

// BENCHMARK_TEMPLATE(accumulate, std::list<int>)
//     ->RangeMultiplier(1024)
//     ->Range(1ull << 10, 2ull << 10 << 10);
BENCHMARK_TEMPLATE(accumulate, std::vector<int>)
    ->RangeMultiplier(2)
    ->Range(1ull << 10, 1ull << 20);

BENCHMARK_TEMPLATE(accumulateRandom, std::vector<int>)
    ->RangeMultiplier(2)
    ->Range(1ull << 10, 1ull << 20);

BENCHMARK_MAIN();
