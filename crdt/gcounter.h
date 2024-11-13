#ifndef GCOUNTER_H
#define GCOUNTER_H

#include <unordered_map>
#include <string>

class GCounter {
public:
    GCounter(const std::string& id = std::to_string(std::hash<std::string>{}(std::to_string(reinterpret_cast<size_t>(this)))));
    void add(int value);
    GCounter merge(const GCounter& other);
    int get() const;

private:
    std::string id;
    std::unordered_map<std::string, int> counts;
};

#endif // GCOUNTER_H
