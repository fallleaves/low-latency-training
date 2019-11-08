#include "../dictionary/utils.h"

#include <benchmark/benchmark.h>

#include <vector>
#include <string_view>
#include <string>
#include <fstream>

std::vector<std::string> generateWords()
{
    std::ifstream input{"/etc/dictionaries-common/words"};

    if (!input.is_open())
    {
        throw std::runtime_error{"can't open file"};
    }

    std::vector<std::string> result;

    for (std::string line; std::getline(input, line);)
    {
        result.emplace_back(std::move(line));
    }

    return result;
}

void benchmarkDictionary(benchmark::State &state)
{
    const auto words = generateWords();
    const auto wordsCount = words.size();

    std::vector<std::string> wordsIn;
    std::vector<std::string> wordsNotIn;

    wordsIn.assign(words.begin(), words.begin() + wordsCount / 2);
    wordsNotIn.assign(words.begin() + wordsCount / 2, words.end());

    Dictionary dict{wordsIn};
    bool notIn = true;

    auto it = wordsNotIn.begin();

    for (auto _ : state)
    {
        notIn = notIn && !dict.isInDictionary(*it);
        ++it;

        if (it == wordsNotIn.end())
        {
            it = wordsNotIn.begin();
        }

        if (!notIn)
        {
            throw std::runtime_error{"Error! some words found"};
        }

        benchmark::DoNotOptimize(notIn);
    }

    state.counters["dict_size"] = dict.size();
}

BENCHMARK(benchmarkDictionary);

BENCHMARK_MAIN();