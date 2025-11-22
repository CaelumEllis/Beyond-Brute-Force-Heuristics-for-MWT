using namespace std;

#ifndef ADJ_LIST_GRAPH_H
#define ADJ_LIST_GRAPH_H
#include "Point.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cassert>
class AdjListGraph {
	using PointPairType = std::vector<std::pair<double, double>>;
	// Disjoint set data struture
	public:
	// custom Point class hash fn
	struct PointHash {
		size_t operator()(const Point& p) const {
			std::hash<double> hashFn;
			size_t h1 = hashFn(p.x);
			size_t h2 = hashFn(p.y);
			return h1 ^ (h2 << 1); // combine x and y
		}
	};

	AdjListGraph() = default;
	// generates a complete graph automatically
	AdjListGraph(PointPairType coordinates) {
		mapCoordinates(coordinates);
		adj_.resize(nodes_);
		for (size_t i = 0; i < nodes_; i++) {
			for (size_t j = 0; j < nodes_; j++) {
				if (i == j) continue;
				adj_.at(i).push_back(j);
			}
		}
	}

	std::vector<Point> getPoints() const {
		return pointList_;
	}

	std::unordered_map<Point, size_t, AdjListGraph::PointHash> getPointNodeTable() {
		return PointNodeTable_;
	}

	size_t getNodeForPoint(const std::pair<double, double> &p) const {
    	return getNodeForPoint(Point(p.first, p.second));
	}

	size_t getNodeForPoint(const Point& p) const {
    	return PointNodeTable_.at(p);
	}

	Point getPointForNode(size_t nodeNo) const {
		return pointList_.at(nodeNo);
	}

	struct Edge {
		size_t u, v;
		double weight;

		bool operator<(const Edge& other) const {
			return weight < other.weight;
		}
	};

	/** Kruskal funcs */
	class DSU {
	public:
		std::vector<size_t> parent;
		std::vector<size_t> rank;

		DSU(size_t n) {
			parent.resize(n);
			rank.resize(n, 0);
			for (size_t i = 0; i < n; i++) parent[i] = i;
		}

		size_t find(size_t x) {
			if (parent[x] != x)
				parent[x] = find(parent[x]);
			return parent[x];
		}

		void unite(size_t x, size_t y) {
			x = find(x);
			y = find(y);
			if (x == y) return;

			if (rank[x] < rank[y]) std::swap(x, y);
			parent[y] = x;

			if (rank[x] == rank[y]) rank[x]++;
		}
	};

	double euclideanDistance(const Point& a, const Point& b) {
		double dx = a.x - b.x;
		double dy = a.y - b.y;
		return std::sqrt(dx * dx + dy * dy);
	}

	std::vector<std::vector<size_t>> kruskalMST() const {
		std::vector<Edge> edges;

		std::vector<Point> pts = getPoints();
		size_t n = pts.size();

		// build complete edge list
		for (size_t i = 0; i < n; i++) {
			for (size_t j = i + 1; j < n; j++) {
				// note the comparator is square distance
				edges.push_back({i, j, Point::distSq(pts[i], pts[j])});
			}
		}

		// sort edges by square weight
		std::sort(edges.begin(), edges.end());

		DSU dsu(n);
		std::vector<Edge> mst;
		mst.reserve(n - 1);

		for (const Edge& e : edges) {
			if (dsu.find(e.u) != dsu.find(e.v)) {
				dsu.unite(e.u, e.v);
				mst.push_back(e);
				if (mst.size() == n - 1) break;
			}
		}

		return edgesToAdjList(mst);
	}

	/**
	 * numNodes = total nodes in the complete graph
	 * might overload if we ever have a situation thats diff
	 */
	std::vector<std::vector<size_t>> edgesToAdjList(
		const std::vector<Edge>& edges
		) const
	{
		std::vector<std::vector<size_t>> adj(nodes_);

		for (const auto& e : edges) {
			adj[e.u].push_back(e.v);
			adj[e.v].push_back(e.u);  //u ndirected
		}

		return adj;
	}

/**
 * merges an adjacency list and a convex hull pt list
 */
std::vector<std::vector<size_t>> mergeAdjListAndConvex(
    const std::vector<std::vector<size_t>>& adj,
    const std::vector<size_t>& hullNodes) const
{
    std::vector<std::vector<size_t>> merged = adj;
    merged.resize(adj.size()); // ensure enough space

    for (size_t i = 0; i < hullNodes.size(); ++i) {
        size_t u = hullNodes[i];
        size_t v = hullNodes[(i + 1) % hullNodes.size()];

        assert(u < merged.size() && v < merged.size()); // debug

        if (std::find(merged[u].begin(), merged[u].end(), v) == merged[u].end())
            merged[u].push_back(v);

        if (std::find(merged[v].begin(), merged[v].end(), u) == merged[v].end())
            merged[v].push_back(u);
    }

    return merged;
}

/**
joins deg 1 mst points to the convex hull
 */


 void fixTUC(std::vector<std::vector<size_t>>& adj, const std::vector<size_t> &hullNodes) {
	std::vector<size_t> leaves;
		auto N = adj.size();
    leaves.reserve(N);
		// leaf finder
    for (size_t v = 0; v < N; ++v) {
        if (adj[v].size() == 1) {     
            leaves.push_back(v);
        }
    }

		// finds nearest hull pt
    for (size_t leaf : leaves) {

        double bestDist = std::numeric_limits<double>::max();
        size_t bestHull = std::numeric_limits<size_t>::max();

        const auto& p = pointList_.at(leaf);

        for (size_t h : hullNodes) {
            const auto& q = pointList_.at(h);
						// distsquared is computationally cheaper
            double dist2 = Point::distSq(p, q);

            if (dist2 < bestDist) {
                bestDist = dist2;
                bestHull = h;
            }
        }

        // add
        if (bestHull != std::numeric_limits<size_t>::max() && bestHull != leaf) {
            adj[leaf].push_back(bestHull);
            adj[bestHull].push_back(leaf);
						std::cout << "added extra edge " << leaf << " - " << bestHull << "\n";
        }
    }
 }

double computeAdjListWeight(
    const std::vector<std::vector<size_t>>& mstAdj
) const
{
    double total = 0.0;

    size_t n = mstAdj.size();

    for (size_t u = 0; u < n; u++) {
        Point pu = pointList_.at(u);

        for (size_t v : mstAdj[u]) {
            // Only count each edge once --> u < v
            if (u < v) {
                Point pv = pointList_.at(v);
                total += std::sqrt(Point::distSq(pu, pv));
            }
        }
    }
    return total;
}



	private:
	void mapCoordinates(PointPairType &coordinates) {
		for (auto c : coordinates) {
			Point p = Point(c.first, c.second);
			pointList_.emplace_back(p);
			PointNodeTable_[p] = nodes_;
			nodes_++;
		}
	}
	// adjacency list of edges
	std::vector<std::vector<size_t>> adj_;

	// adjacency matrix of edges
	// std::vector<std::vector<size_t>> matrix;
	// mappings from Point to node
	std::vector<Point> pointList_;
	std::unordered_map<Point, size_t, PointHash> PointNodeTable_;
	size_t nodes_ = 0;
};
#endif