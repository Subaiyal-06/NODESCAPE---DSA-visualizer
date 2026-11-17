// Graph with an adjacency list (every vertex keeps a sorted linked list of its neighbours).
// Vertices are numbered 0 to vertexCount - 1. Edges have a weight of 1 or more.

#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

class GNode {
public:
    int to;      // the neighbour
    int weight;
    GNode* next;

    GNode(int to, int weight) {
        this->to = to;
        this->weight = weight;
        this->next = nullptr;
    }
};

class Graph {
public:
    static const int MAXV = 12;

private:
    int vertices;
    bool directed;
    int edges;
    GNode* adj[MAXV];

    bool insertSorted(int u, int v, int weight);
    bool removeFromList(int u, int v);
    void dfsFrom(int vertex, bool visited[], int order[], int& count) const;
    // recording: one picture of the graph (roles of vertices and edges, a strip of numbers, extra lines)
    void snap(const std::string& message, int line, const int vertexRole[], int edgeRole[][MAXV],
              const std::vector<int>& strip, const std::string& stripLabel, const std::vector<std::string>& info,
              const std::vector<std::string>& vertexTag) const;

public:
    Graph(int vertexCount, bool isDirected);
    ~Graph();

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    int vertexCount() const;
    int edgeCount() const;
    bool isDirected() const;

    bool addEdge(int u, int v, int weight);  // false for bad vertices, self loops, duplicates, weight < 1
    bool removeEdge(int u, int v);
    bool hasEdge(int u, int v) const;
    void clearEdges();

    void displayList() const;
    void displayMatrix() const;

    // Traversals write the visit order into order[] (size MAXV) and return how many vertices were reached.
    // Neighbours are always visited from the smallest number to the largest.
    int bfs(int start, int order[]) const;           // uses a queue
    int dfs(int start, int order[]) const;           // recursion
    int dfsIterative(int start, int order[]) const;  // uses a stack, same order as dfs

    int connectedComponents() const;                 // undirected graphs

    // Directed graphs only. Returns false when the graph has a cycle (no valid order exists).
    bool topologicalSort(int order[], int& count) const;

    // Minimum spanning tree of an undirected weighted graph. The chosen edges are written to
    // eu[], ev[], ew[] (size MAXV). Returns the total weight. connected is false when the graph
    // has more than one component, then the result is only a partial tree.
    int primMST(int start, int eu[], int ev[], int ew[], int& edgeCountOut, bool& connected) const;
    void showGraph(const std::string& message) const; // records one calm picture of the graph (no algorithm)
    int kruskalMST(int eu[], int ev[], int ew[], int& edgeCountOut, bool& connected) const;
};

#endif
