// Tests for the graph: representation, traversals, spanning trees and topological sort

#include "TestUtils.h"
#include "../src/core/Graph.h"
#include <vector>
using namespace std;

static bool same(const int a[], int n, const vector<int>& expected) {
    if (n != (int)expected.size()) return false;
    for (int i = 0; i < n; i++) {
        if (a[i] != expected[i]) return false;
    }
    return true;
}

int main() {
    int order[Graph::MAXV];
    int eu[Graph::MAXV], ev[Graph::MAXV], ew[Graph::MAXV];

    // building a graph
    {
        Graph g(5, false);
        check(g.vertexCount() == 5 && g.edgeCount() == 0 && !g.isDirected(), "new graph");
        check(g.addEdge(0, 1, 4) && g.addEdge(1, 2, 2), "add edges");
        check(g.hasEdge(0, 1) && g.hasEdge(1, 0), "undirected edge goes both ways");
        check(!g.addEdge(0, 1, 9) && !g.addEdge(1, 0, 9), "duplicate edge");
        check(!g.addEdge(2, 2, 1), "self loop");
        check(!g.addEdge(0, 7, 1) && !g.addEdge(-1, 0, 1), "vertex out of range");
        check(!g.addEdge(0, 3, 0), "weight below 1");
        check(g.edgeCount() == 2, "edge count");
        check(g.removeEdge(1, 0) && !g.hasEdge(0, 1) && !g.hasEdge(1, 0) && g.edgeCount() == 1, "remove an edge");
        check(!g.removeEdge(3, 4), "remove a missing edge");
        g.clearEdges();
        check(g.edgeCount() == 0 && !g.hasEdge(1, 2), "clear edges");

        Graph d(3, true);
        d.addEdge(0, 1, 1);
        check(d.hasEdge(0, 1) && !d.hasEdge(1, 0), "directed edge goes one way");
    }

    // BFS and DFS on a tree-shaped graph
    {
        Graph g(5, false);
        g.addEdge(0, 1, 1); g.addEdge(0, 2, 1); g.addEdge(1, 3, 1); g.addEdge(2, 4, 1);
        check(same(order, g.bfs(0, order), {0, 1, 2, 3, 4}), "BFS goes level by level");
        check(same(order, g.dfs(0, order), {0, 1, 3, 2, 4}), "DFS goes deep first");
        check(same(order, g.dfsIterative(0, order), {0, 1, 3, 2, 4}), "iterative DFS gives the same order");
        check(same(order, g.bfs(3, order), {3, 1, 0, 2, 4}), "BFS from another start");
        check(g.bfs(9, order) == 0 && g.dfs(-1, order) == 0, "bad start vertex");
    }

    // a graph with a cycle: every vertex is visited once
    {
        Graph g(4, false);
        g.addEdge(0, 1, 1); g.addEdge(1, 2, 1); g.addEdge(2, 3, 1); g.addEdge(3, 0, 1);
        check(g.bfs(0, order) == 4 && g.dfs(0, order) == 4 && g.dfsIterative(0, order) == 4, "cycle does not cause endless loops");
    }

    // connected components
    {
        Graph g(6, false);
        g.addEdge(0, 1, 1); g.addEdge(1, 2, 1); g.addEdge(3, 4, 1);
        check(g.connectedComponents() == 3, "three components (0-1-2, 3-4, 5)");
        check(g.bfs(0, order) == 3, "BFS only reaches its own component");
        g.addEdge(2, 3, 1); g.addEdge(4, 5, 1);
        check(g.connectedComponents() == 1, "joined into one component");
    }

    // minimum spanning tree
    {
        Graph g(6, false);
        g.addEdge(0, 1, 4); g.addEdge(0, 2, 3); g.addEdge(1, 2, 1); g.addEdge(1, 3, 2);
        g.addEdge(2, 3, 4); g.addEdge(3, 4, 2); g.addEdge(4, 5, 6);
        int count;
        bool connected;
        int kruskal = g.kruskalMST(eu, ev, ew, count, connected);
        check(kruskal == 14 && count == 5 && connected, "Kruskal total weight");
        int prim = g.primMST(0, eu, ev, ew, count, connected);
        check(prim == 14 && count == 5 && connected, "Prim total weight");
        check(g.primMST(3, eu, ev, ew, count, connected) == 14, "Prim gives the same weight from another start");

        Graph split(4, false);
        split.addEdge(0, 1, 5); split.addEdge(2, 3, 7);
        split.kruskalMST(eu, ev, ew, count, connected);
        check(!connected && count == 2, "Kruskal on a disconnected graph is a forest");
        split.primMST(0, eu, ev, ew, count, connected);
        check(!connected && count == 1, "Prim on a disconnected graph reaches only its component");

        Graph directed(3, true);
        directed.addEdge(0, 1, 1);
        check(directed.kruskalMST(eu, ev, ew, count, connected) == -1, "spanning trees need an undirected graph");
    }

    // topological sort
    {
        Graph g(6, true);
        g.addEdge(5, 2, 1); g.addEdge(5, 0, 1); g.addEdge(4, 0, 1);
        g.addEdge(4, 1, 1); g.addEdge(2, 3, 1); g.addEdge(3, 1, 1);
        int count;
        check(g.topologicalSort(order, count) && same(order, count, {4, 5, 0, 2, 3, 1}), "topological order");
        g.addEdge(1, 5, 1); // 5 -> 2 -> 3 -> 1 -> 5 is a cycle
        check(!g.topologicalSort(order, count), "a cycle has no topological order");

        Graph undirected(3, false);
        check(!undirected.topologicalSort(order, count), "undirected graphs have no topological order");
    }

    return finishTests();
}
