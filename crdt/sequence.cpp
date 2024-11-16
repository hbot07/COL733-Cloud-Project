#include "sequence.h"
#include <algorithm>
#include <functional>

OpId::OpId(const std::string& node, int id) : node(node), id(id) {}

int OpId::compare(const OpId& other) const {
    if (id < other.id) {
        return -1;
    } else if (id > other.id) {
        return 1;
    } else {
        // Conflicts are resolved arbitrarily by comparing the owner IDs
        if (node < other.node) {
            return -1;
        } else if (node > other.node) {
            return 1;
        } else {
            return 0;
        }
    }
}

bool OpId::isEarlier(const OpId& other) const {
    int cmp = compare(other);
    if (cmp == 0) {
        throw std::runtime_error("Found conflicting Operations with the same OpId");
    }
    return cmp < 0;
}

size_t OpId::getHash() const {
    return std::hash<std::string>{}(node) ^ std::hash<int>{}(id);
}

bool OpId::operator==(const OpId& other) const {
    return node == other.node && id == other.id;
}

bool OpId::operator<(const OpId& other) const {
    return isEarlier(other);
}

Operation::Operation(const OpId& owner, OperationType action, const Operation* target, const std::string& payload)
    : owner(owner), action(action), target(target), payload(payload) {}

void Operation::apply(ObjectTree& objects) const {
    Object obj(*this);
    if (action == OperationType::INSERT_BEFORE) {
        objects.insert(target, &obj, true);
    } else if (action == OperationType::INSERT_AFTER) {
        objects.insert(target, &obj, false);
    } else if (action == OperationType::REMOVE) {
        for (Object* o : objects) {
            if (o->operation == *this) {
                o->tombstone = true;
                break;
            }
        }
    } else {
        throw std::invalid_argument("Invalid operation type");
    }
}

bool Operation::operator==(const Operation& other) const {
    return owner == other.owner;
}

bool Operation::operator<(const Operation& other) const {
    return owner < other.owner;
}

size_t Operation::hash() const {
    return owner.getHash();
}

std::string Operation::toString() const {
    std::string target_str = target ? target->owner.toString() : "null";
    return "owner: " + owner.toString() + ", action: " + std::to_string(static_cast<int>(action)) + ", target: " + target_str + ", payload: " + payload;
}

Object::Object(const Operation& operation) : operation(operation), tombstone(false) {}

std::string Object::toString() const {
    return "owner: " + operation.owner.toString() + ", payload: " + operation.payload + ", tombstone: " + std::to_string(tombstone);
}

Sequence::Sequence(const std::string& id) : id(id), clock(id) {}

int Sequence::compareOperations(const Operation& a, const Operation& b) const {
    return a.owner.compare(b.owner);
}

void Sequence::append(const std::string& item) {
    clock.add(1);
    const Operation* target = nullptr;
    OperationType action = OperationType::INSERT_BEFORE;
    if (!sequence.enumerate().hasNext()) {
        // Special case: there is no operation to reference
    } else {
        // Insert after the last object in the sequence
        std::vector<Object*> objects = getObjects();
        if (!objects.empty()) {
            target = &objects.back()->operation;
            action = OperationType::INSERT_AFTER;
        }
    }
    OpId owner(id, clock.get());
    Operation op(owner, action, target, item);
    operations.add(op);
    op.apply(sequence);
}

void Sequence::appendMany(const std::vector<std::string>& items) {
    for (const auto& item : items) {
        append(item);
    }
}

Object* Sequence::objectAtPosition(int position) {
    std::vector<Object*> objects = getObjects();
    if (position < 0 || position >= static_cast<int>(objects.size())) {
        throw std::out_of_range("Position " + std::to_string(position) + " out of range of sequence with length " + std::to_string(objects.size()));
    }
    return objects[position];
}

void Sequence::insert(int position, const std::string& item) {
    clock.add(1);
    Object* target = objectAtPosition(position);
    OpId owner(id, clock.get());
    Operation op(owner, OperationType::INSERT_BEFORE, &target->operation, item);
    operations.add(op);
    op.apply(sequence);
}

void Sequence::insertMany(int position, const std::vector<std::string>& items) {
    for (const auto& item : items) {
        insert(position, item);
        position++;
    }
}

void Sequence::remove(int position) {
    clock.add(1);
    Object* target = objectAtPosition(position);
    OpId owner(id, clock.get());
    Operation op(owner, OperationType::REMOVE, &target->operation);
    operations.add(op);
    op.apply(sequence);
}

void Sequence::removeMany(int position, int count) {
    for (int i = 0; i < count; i++) {
        remove(position);
    }
}

Sequence Sequence::merge(const Sequence& other) {
    mergeOperations(other);
    other.mergeOperations(*this);

    std::vector<std::string> this_sequence = get();
    std::vector<std::string> other_sequence = other.get();
    for (size_t i = 0; i < this_sequence.size(); i++) {
        if (i < other_sequence.size() && this_sequence[i] != other_sequence[i]) {
            if (this_sequence[i] == other_sequence[i]) {
                continue;
            } else if (this_sequence[i] == other_sequence[i]) {
                this_sequence[i] = other_sequence[i];
            } else {
                // Merge the sub-sequences
                if (this_sequence[i] == other_sequence[i]) {
                    continue;
                } else if (this_sequence[i] == other_sequence[i]) {
                    this_sequence[i] = other_sequence[i];
                } else {
                    Sequence& this_sub = dynamic_cast<Sequence&>(this_sequence[i]);
                    Sequence& other_sub = dynamic_cast<Sequence&>(other_sequence[i]);
                    this_sub.merge(other_sub);
                    this_sub.id = id;
                }
            }
        }
    }

    return *this;
}

void Sequence::mergeOperations(const Sequence& other) {
    clock = clock.merge(other.clock);

    std::unordered_set<Operation, std::function<size_t(const Operation&)>> patch_ops(
        other.operations.get().begin(), other.operations.get().end(),
        [](const Operation& op) { return op.hash(); });
    patch_ops.erase(operations.get().begin(), operations.get().end());

    std::vector<Operation> patch_log(patch_ops.begin(), patch_ops.end());
    std::sort(patch_log.begin(), patch_log.end(), [this](const Operation& a, const Operation& b) {
        return compareOperations(a, b) < 0;
    });

    for (const auto& op : patch_log) {
        op.apply(sequence);
    }

    operations = operations.merge(other.operations);
}

std::vector<std::string> Sequence::get() const {
    std::vector<std::string> result;
    for (const Object* obj : getObjects()) {
        result.push_back(obj->operation.payload);
    }
    return result;
}

std::vector<Object*> Sequence::getObjects() const {
    std::vector<Object*> result;
    for (Object* obj : sequence) {
        if (!obj->tombstone) {
            result.push_back(obj);
        }
    }
    return result;
}
