
#include "triangulate.h"
#include <algorithm>

static void addEdge(std::vector<Edge> &edges, int a, int b) {
    if (a > b) std::swap(a, b);
    edges.push_back({a, b});
}


double DOGT(const std::vector<Point> &P) {

    int n = P.size();
    std::vector<Edge> edges;

    std::vector<int> H = convexHullIndices(P);

    // Case 1 — convex polygon
    if ((int)H.size() == n) {
        for (int i = 1; i < n - 1; i++) {
            addEdge(edges, H[0], H[i]);
            addEdge(edges, H[i], H[i+1]);
            addEdge(edges, H[i+1], H[0]);
        }
    }
    else {
        // interior points
        std::vector<int> I;
        std::vector<bool> isHull(n, false);
        for (int h : H) isHull[h] = true;
        for (int i = 0; i < n; i++)
            if (!isHull[i]) I.push_back(i);

        // centroid
        std::vector<Point> hullPts;
        for (int h : H) hullPts.push_back(P[h]);
        Point c = polygonCentroid(hullPts);

        // sort interior
        std::sort(I.begin(), I.end(), [&](int a, int b){
            double da = (P[a].x - c.x)*(P[a].x - c.x) + 
                        (P[a].y - c.y)*(P[a].y - c.y);
            double db = (P[b].x - c.x)*(P[b].x - c.x) + 
                        (P[b].y - c.y)*(P[b].y - c.y);
            return da < db;
        });

        // list of current triangles (needed only for search)
        struct Tri { int a,b,c; };
        std::vector<Tri> T;

        // fan first interior point
        int q1 = I[0];
        for (int i = 0; i < (int)H.size(); i++) {
            int a = H[i];
            int b = H[(i+1)%H.size()];
            T.push_back({q1,a,b});
            addEdge(edges, q1, a);
            addEdge(edges, a, b);
            addEdge(edges, b, q1);
        }

        // remaining interior points
        for (int k = 1; k < (int)I.size(); k++) {
            int q = I[k];

            int idx = -1;
            for (int t = 0; t < (int)T.size(); t++) {
                Tri tr = T[t];
                if (pointInTriangle(P[q], P[tr.a], P[tr.b], P[tr.c])) {
                    idx = t;
                    break;
                }
            }

            if (idx == -1) continue;

            Tri F = T[idx];
            T.erase(T.begin() + idx);

            // subdivide
            T.push_back({q, F.a, F.b});
            T.push_back({q, F.b, F.c});
            T.push_back({q, F.c, F.a});

            addEdge(edges, q, F.a);
            addEdge(edges, q, F.b);
            addEdge(edges, q, F.c);
        }
    }

    // dedupe edges
    std::sort(edges.begin(), edges.end(), 
        [](const Edge &e1, const Edge &e2){
            return (e1.u < e2.u) || (e1.u == e2.u && e1.v < e2.v);
        }
    );
    edges.erase(std::unique(edges.begin(), edges.end(), 
        [](const Edge &e1, const Edge &e2){
            return e1.u == e2.u && e1.v == e2.v;
        }
    ), edges.end());

    // sum length
    double total = 0.0;
    for (auto &e : edges)
        total += edgeLength(P[e.u], P[e.v]);

    return total;
}
