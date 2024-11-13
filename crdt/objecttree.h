#ifndef OBJECTTREE_H
#define OBJECTTREE_H

#include <vector>
#include "sequence.h"

class ObjectRoot {
public:
    ObjectRoot(const Object& obj);
    std::vector<Object*> nodes;
    Object obj;
};

class ObjectTree {
public:
    ObjectTree();
    void insert(const Operation* target, Object* object, bool before = true);

    class Enumerator {
    public:
        Enumerator(const ObjectTree& tree, bool reverse = false);
        std::tuple<ObjectRoot*, int, Object*> next();
        bool hasNext() const;

    private:
        std::vector<ObjectRoot*>::const_iterator rootIter;
        std::vector<ObjectRoot*>::const_iterator rootEnd;
        std::vector<Object*>::const_iterator nodeIter;
        std::vector<Object*>::const_iterator nodeEnd;
        bool reverse;
    };

    Enumerator enumerate(bool reverse = false) const;

private:
    std::vector<ObjectRoot*> roots;
};

#endif // OBJECTTREE_H
