//
// Created by Caelum Ellis on 17/11/2025.
//

//
// Created by Caelum Ellis on 17/11/2025.
//
#ifndef EDGEKEY_H
#define EDGEKEY_H

#include <functional>

struct EdgeKey {
    int u, v;
    EdgeKey(int a, int b) {
        if (a < b) { u = a; v = b; }
        else { u = b; v = a; }
    }
    bool operator==(const EdgeKey &o) const {
        return u == o.u && v == o.v;
    }
};

// Custom hash so EdgeKey can be used in both std::unordered_set and unordered_map
struct EdgeKeyHash {
    size_t operator()(const EdgeKey& e) const {
        return std::hash<int>()(e.u) ^ (std::hash<int>()(e.v) << 1);
    }
};
#endif // EDGEKEY_H

