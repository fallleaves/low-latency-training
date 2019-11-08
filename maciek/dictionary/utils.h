#pragma once

#include <vector>
#include <string>
#include <unordered_set>

class Dictionary
{
public:
  Dictionary(const std::vector<std::string> &words)
  {
    for (const auto &str : words)
    {
      container.insert(str);
    }
  }

  bool isInDictionary(std::string_view word) const
  {
    return container.find(word.data()) != container.end();
  }

  std::size_t size() const
  {
    return container.size();
  }

private:
  std::unordered_set<std::string> container;
};
