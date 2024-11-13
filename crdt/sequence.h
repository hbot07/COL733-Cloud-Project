#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <string>
#include <vector>
#include <unordered_set>
#include <functional>
#include "gcounter.h"
#include "gset.h"
#include "objecttree.h"

enum class OperationType { INSERT_BEFORE, INSERT_AFTER, REMOVE };

class OpId {
public:
    OpId(const std::string& node, int id);
    int compare(const OpId& other) const;
    bool isEarlier(const OpId& other) const;
    size_t getHash() const;
    bool operator==(const OpId& other) const;
    bool operator<(const OpId& other) const;

    std::string node;
    int id;
};

class Operation {
public:
    Operation(const OpId& owner, OperationType action, const Operation* target, const std::string& payload = "");
    void apply(ObjectTree& objects) const;
    bool operator==(const Operation& other) const;
    bool operator<(const Operation& other) const;
    size_t hash() const;
    std::string toString() const;

    OpId owner;
    OperationType action;
    const Operation* target;
    std::string payload;
};

class Object {
public:
    Object(const Operation& operation);
    std::string toString() const;

    Operation operation;
    bool tombstone;
};

class Sequence {
public:
    Sequence(const std::string& id = std::to_string(std::hash<std::string>{}(std::to_string(reinterpret_cast<size_t>(this)))));
    int compareOperations(const Operation& a, const Operation& b) const;
    void append(const std::string& item);
    void appendMany(const std::vector<std::string>& items);
    Object* objectAtPosition(int position);
    void insert(int position, const std::string& item);
    void insertMany(int position, const std::vector<std::string>& items);
    void remove(int position);
    void removeMany(int position, int count);
    Sequence merge(const Sequence& other);

    std::vector<std::string> get() const;
    std::vector<Object*> getObjects() const;

private:
    void mergeOperations(const Sequence& other);

    std::string id;
    GSet operations;
    GCounter clock;
    ObjectTree sequence;
};

#endif // SEQUENCE_H
