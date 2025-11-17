//
// Created by Caelum Ellis on 17/11/2025.
//
#include "GraphState.h"
#include "FlipCriteria.h"   // Required so FlipResult members are visible!
#include <algorithm>
#include <iostream>



void GraphState::buildAdjacency() {
    for (auto &e : edges) {
        adjacency[e.u].insert(e.v);
        adjacency[e.v].insert(e.u);
    }
}

void GraphState::buildEdgeTriangleMap() {
    for (size_t i = 0; i < triangles.size(); ++i) {
        auto &t = triangles[i];
        addTriangleEdge(t.a, t.b, i);
        addTriangleEdge(t.b, t.c, i);
        addTriangleEdge(t.c, t.a, i);
    }
}

void GraphState::addTriangleEdge(int x, int y, int triIdx) {
    EdgeKey key(x, y);
    edgeToTriangles[key].push_back(triIdx);
}

void GraphState::removeEdge(int u, int v) {
    EdgeKey key(u, v);

    adjacency[u].erase(v);
    adjacency[v].erase(u);

    // Remove from hash tables
    edgeTable.erase(key);

    auto it = edgeIndex.find(key);
    if (it != edgeIndex.end()) {
        size_t idx = it->second;
        edgeIndex.erase(it);

        // swap and pop trick to maintain dense array
        if (idx != edges.size() - 1) {
            std::swap(edges[idx], edges.back());
            edgeIndex[EdgeKey(edges[idx].u, edges[idx].v)] = idx;
        }
        edges.pop_back();
    }

    // remove triangle mapping
    edgeToTriangles.erase(key);
}

void GraphState::addEdge(int u, int v) {
    EdgeKey k(u, v);

    adjacency[u].insert(v);
    adjacency[v].insert(u);

    auto &p1 = points[u];
    auto &p2 = points[v];
    double dx = p1.first - p2.first;
    double dy = p1.second - p2.second;

    Edge e{u, v, std::sqrt(dx*dx + dy*dy)};

    size_t idx = edges.size();
    edges.push_back(e);

    edgeTable[k] = e;
    edgeIndex[k] = idx;
}


  //    Perform the legal diagonal flip:
  //    Before:
  //        triangles (a,b,d) and (a,d,c)
  //    After:
  //        triangles (a,b,c) and (b,d,c)

void GraphState::FlipEdge(const FlipResult& f)
{
    if (!f.legal) {
        std::cerr << "[FlipEdge] Attempted illegal flip.\n";
        return;
    }

    int a = f.a;
    int b = f.b;
    int c = f.c;
    int d = f.d;

    EdgeKey oldKey(b, d);

    // Fetch triangles involving (b,d)
    auto &triList = edgeToTriangles[oldKey];
    if (triList.size() != 2) {
        std::cerr << "[FlipEdge] Unexpected: flip target not shared by 2 triangles!\n";
        return;
    }

    int t0 = triList[0];
    int t1 = triList[1];

    // Replace the triangles
    triangles[t0] = Triangle{a, b, c};
    triangles[t1] = Triangle{a, c, d};

    // Update edges, remove (b,d), add (a,c)
    removeEdge(b, d);
    addEdge(a, c);

    // Rebuild mapping for these triangles only so that graph state is preserved
    edgeToTriangles.erase(oldKey);

    addTriangleEdge(a, b, t0);
    addTriangleEdge(b, c, t0);
    addTriangleEdge(c, a, t0);

    addTriangleEdge(a, c, t1);
    addTriangleEdge(c, d, t1);
    addTriangleEdge(d, a, t1);
}
