#ifndef TWOPSET_H
#define TWOPSET_H

#include "gset.h"

class TwoPhaseSet {
public:
    TwoPhaseSet();
    void add(const std::string& item);
    void remove(const std::string& item);
    TwoPhaseSet& merge(const TwoPhaseSet& other);
    std::unordered_set<std::string> get() const;

private:
    GSet added;
    GSet removed;
};

#endif // TWOPSET_H
