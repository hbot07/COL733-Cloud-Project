#ifndef GSET_H
#define GSET_H

#include <unordered_set>
#include <string>

class GSet {
public:
    GSet();
    void add(const std::string& item);
    GSet merge(const GSet& other);
    std::unordered_set<std::string> get() const;

private:
    std::unordered_set<std::string> items;
};

#endif // GSET_H
