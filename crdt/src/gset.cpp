#include <unordered_set>
#include <string>
#include <stdexcept>
#include "headers/gset.h"


Gset::Gset(){

return;
}

    void Gset::add(const std::string& item){
        this->items.insert(item);
    }

GSet& merge(const GSet& other){

std::unordered_set<std::string> n_set;

nset = merge(this->items,other.items);
this->items = nset;
return *this;

}

std::unordered_set<std::string> get(){
    return this->items;
}