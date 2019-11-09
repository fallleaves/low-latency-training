#include <benchmark/benchmark.h>

#include <array>
#include <cstdlib>
#include <thread>
#include <stdexcept>
#include <sched.h>
#include <string.h>
#include <errno.h>

void pin_to_cpu(int cpu)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);

    int r = ::sched_setaffinity(0, sizeof(set), &set);
    if (r == -1)
    {
        throw std::runtime_error(::strerror(errno));
    }
}

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
    DataModifier(Data &data, bool setAffinity = false)
        : data{data}, setAffinity{setAffinity}
    {
    }

    void work()
    {
        if (setAffinity)
            pin_to_cpu(2);

        while (!stop)
        {
            data.a[4] = std::rand();
        }
    }

    Data &data;
    bool stop = false;
    bool setAffinity;
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

static void cachelineBenchAffinity(benchmark::State &state)
{
    auto data = generateData();

    pin_to_cpu(1);

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

static void cachelineBenchModificationAffinity(benchmark::State &state)
{
    auto data = generateData();

    DataModifier modifier{data};
    std::thread worker{&DataModifier::work, &modifier};
    worker.detach();

    pin_to_cpu(1);

    for (auto _ : state)
    {
        auto value = data.b[4];
        benchmark::DoNotOptimize(value);
    }

    modifier.stop = true;
}

BENCHMARK(cachelineBench);
//BENCHMARK(cachelineBenchAffinity);
BENCHMARK(cachelineBenchModification);
BENCHMARK(cachelineBenchModificationAffinity);

BENCHMARK_MAIN();
