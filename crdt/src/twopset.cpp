#include <algorithm>
#include <unordered_set>
#include <string>
#include <stdexcept>
#include "headers/twopset.h"
#include "headers/gset.h"


    TwoPhaseSet::TwoPhaseSet(){
        return;
    }


    void TwoPhaseSet::add(const std::string& item){
        this->added.insert(item);
    }

    void remove(const std::string& item){
        this->removed.insert(item);
    }
    
    TwoPhaseSet& merge(const TwoPhaseSet& other){
        this->added = this->added.merge(other.added);
        this->removed = this->removed.merge(other.removed);
        return *this;
    }
    std::unordered_set<std::string> get(){
        std::unordered_set<std::string> difference;
        set_difference(
        this->added.begin(), this->added.end(), this->removed.begin(), this->removed.end(),
        inserter(difference, difference.begin()));
        return difference;
    }