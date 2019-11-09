#include <benchmark/benchmark.h>

#include <array>
#include <cstdlib>
#include <thread>

struct alignas(64) Data
{
    std::array<int, 8> a;
    std::array<int, 8> b;
};

Data generateData()
{
    Data data;
    std::for_each(data.a.begin(), data.a.end(), [](auto &element) { element = std::rand(); });
    std::for_each(data.b.begin(), data.b.end(), [](auto &element) { element = std::rand(); });

    return data;
}

struct DataModifier
{
    DataModifier(Data &data)
        : data{data}
    {
    }

    void work()
    {
        while (!stop)
        {
            data.a[4] = std::rand();
        }
    }

    Data &data;
    bool stop = false;
};

static void cachelineBench(benchmark::State &state)
{
    auto data = generateData();

    for (auto _ : state)
    {
        auto value = data.b[4];
        benchmark::DoNotOptimize(value);
    }
}

static void cachelineBenchModification(benchmark::State &state)
{
    auto data = generateData();

    DataModifier modifier{data};
    std::thread worker{&DataModifier::work, &modifier};

    worker.detach();

    for (auto _ : state)
    {
        auto value = data.b[4];
        benchmark::DoNotOptimize(value);
    }

    modifier.stop = true;
}

BENCHMARK(cachelineBench);
BENCHMARK(cachelineBenchModification);

BENCHMARK_MAIN();
