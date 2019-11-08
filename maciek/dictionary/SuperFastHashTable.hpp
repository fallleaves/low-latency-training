#pragma once

#include <functional>
#include <cstring>

template <typename Value, typename Hash = std::hash<Value>>
class SuperFastHashTable
{
    static constexpr auto LOAD_FACTOR = 3;

private:
    struct Node
    {
        bool isInitialized;
        std::size_t valueHash;
        Value value;
    };

public:
    template <typename InputIterator>
    SuperFastHashTable(InputIterator begin, InputIterator end)
    {
        allocateNewMemory(std::distance(begin, end) * LOAD_FACTOR);

        std::for_each(begin, end, [this](const auto &value) { insert(value); });
    }

    void insert(const Value &value)
    {
        const auto expectedIndex = generateIndex(value);
        const auto index = getNextAvailableIndex(expectedIndex);

        container[index].value = value;
        container[index].isInitialized = true;
        container[index].valueHash = expectedIndex;
    }

    //bool erase(const Value &value);

    const Value *find(const Value &value) const
    {
        const auto expectedIndex = generateIndex(value);

        for (auto index = expectedIndex; index < container.capacity(); ++index)
        {
            if (container[index].isInitialized && container[index].valueHash == expectedIndex && container[index].value == value)
            {
                return &(container[index].value);
            }

            if (!container[index].isInitialized)
            {
                return nullptr;
            }
        }

        return nullptr;
    }

private:
    std::size_t generateIndex(const Value &value) const
    {
        return Hash{}(value) % container.capacity();
    }

    std::size_t getNextAvailableIndex(std::size_t index)
    {
        auto expectedIndex = index;

        for (; expectedIndex < container.capacity() && container[expectedIndex].isInitialized; ++expectedIndex)
        {
        }

        if (expectedIndex == container.capacity())
        {
            reHash();
            return getNextAvailableIndex(index);
        }

        return expectedIndex;
    }

    void allocateNewMemory(std::size_t newSize)
    {
        container.reserve(newSize);
        std::memset(container.data(), 0, newSize * sizeof(Node));
    }

    void reHash()
    {
        // TODO
    }

private:
    std::vector<Node> container;
    std::size_t size = 0;
};