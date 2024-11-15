#include <unordered_map>
#include <string>
#include <stdexcept>
#include "headers/gcounter.h"


Gcounter::Gcounter(const std::string& id = std::to_string(std::hash<std::string>{}(std::to_string(reinterpret_cast<size_t>(this))))){
   this->id = id;
   this->counts[id] = 0;
}

void Gcounter::add(int value){
    if(value < 0){
         throw std::invalid_argument( "received negative value" );
    }

    this->counts[this->id] += value;
}

int Gcounter::get(){
   int answer = 0;
   for(auto element: this->counts){
          answer += element.second;
   }
   return answer;
}

 Gcounter& Gcounter::merge(const GCounter& other){

    for(auto element:other->counts){
        int cand = 0;
        if(this->counts.find(element.first) != this->counts.end()){
         cand = this->counts[element.first];
        }
        this->counts[element.first] = max(cand,element.second);
    }
   return *this;
 }