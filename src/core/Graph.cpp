#include "Graph.h"
#include "LinkedQueue.h"
#include "LinkedStack.h"
#include "Trace.h"
#include "Recorder.h"
#include <iostream>
#include <string>
using namespace std;


// ---------- recording helpers (used only when the recorder is switched on) ----------

namespace {
    const vector<string> CODE_BFS = {
        "queue.push(start);  visited[start] = true;",
        "while (!queue.empty()) {",
        "  v = queue.pop();   visit(v);",
        "  for (each neighbour w of v)",
        "    if (!visited[w]) { visited[w] = true; queue.push(w); }",
        "}"
    };
    const vector<string> CODE_DFS = {
        "dfs(v) {",
        "  visited[v] = true;   visit(v);",
        "  for (each neighbour w of v)",
        "    if (!visited[w]) dfs(w);      // go deeper first",
        "}   // when the loop ends we backtrack"
    };
    const vector<string> CODE_TOPO = {
        "compute indegree[v] for every vertex;",
        "queue = all vertices with indegree 0;",
        "while (!queue.empty()) {",
        "  v = queue.pop();   order.add(v);",
        "  for (each edge v -> w) if (--indegree[w] == 0) queue.push(w);",
        "}  // fewer than V vertices placed means a cycle"
    };
    const vector<string> CODE_PRIM = {
        "key[start] = 0;   all other keys = infinity;",
        "repeat V times:",
        "  best = vertex outside the tree with the smallest key;",
        "  add best (and the edge to its parent) to the tree;",
        "  for (each neighbour w of best outside the tree)",
        "    if (weight(best, w) < key[w]) { key[w] = weight; parent[w] = best; }"
    };
    const vector<string> CODE_KRUSKAL = {
        "sort all edges by weight;",
        "each vertex is its own group;",
        "for (each edge (u, v) in that order) {",
        "  if (group(u) != group(v)) { accept the edge; merge the groups; }",
        "  else skip it;     // it would close a cycle",
        "}"
    };

    // vertex and edge roles while one algorithm runs
    struct GState {
        int v[Graph::MAXV];
        int e[Graph::MAXV][Graph::MAXV];
        vector<int> path;       // recursion stack of the depth first search
        vector<int> order;
        GState() { reset(); }
        void reset() {
            for (int i = 0; i < Graph::MAXV; i++) {
                v[i] = ROLE_NORMAL;
                for (int j = 0; j < Graph::MAXV; j++) e[i][j] = ROLE_NORMAL;
            }
            path.clear();
            order.clear();
        }
        void setEdge(int a, int b, int role) {
            e[a][b] = role;
            e[b][a] = role;
        }
    };
    GState gs;

    string orderText(const vector<int>& order, const char* label) {
        string t = string(label) + ": ";
        for (size_t i = 0; i < order.size(); i++) t += to_string(order[i]) + " ";
        return t;
    }
}

void Graph::snap(const string& message, int line, const int vertexRole[], int edgeRole[][MAXV], const vector<int>& strip,
                 const string& stripLabel, const vector<string>& info, const vector<string>& vertexTag) const {
    if (!Recorder::enabled()) return;
    Frame f;
    f.kind = KIND_GRAPH;
    f.message = message;
    f.codeLine = line;
    f.directed = directed;
    f.auxLabel = stripLabel;
    f.info = info;
    for (int v = 0; v < vertices; v++) {
        f.vertices.push_back(GraphVertex(v, (Role)vertexRole[v]));
        if (v < (int)vertexTag.size() && !vertexTag[v].empty()) f.markers.push_back(Marker(v, vertexTag[v]));
    }
    for (int u = 0; u < vertices; u++) {
        for (GNode* t = adj[u]; t != nullptr; t = t->next) {
            if (!directed && u > t->to) continue;
            int role = edgeRole[u][t->to];
            if (!directed && edgeRole[t->to][u] > role) role = edgeRole[t->to][u];
            f.edges.push_back(GraphEdge(u, t->to, t->weight, (Role)role));
        }
    }
    for (size_t i = 0; i < strip.size(); i++) f.cells.push_back(Cell(strip[i], ROLE_NORMAL));
    Recorder::add(f);
}

void Graph::showGraph(const string& message) const {
    Recorder::begin("Graph", vector<string>());
    gs.reset();
    snap(message, -1, gs.v, gs.e, vector<int>(), "", vector<string>(), vector<string>());
}

Graph::Graph(int vertexCount, bool isDirected) {
    if (vertexCount < 1) vertexCount = 1;
    if (vertexCount > MAXV) vertexCount = MAXV;
    vertices = vertexCount;
    directed = isDirected;
    edges = 0;
    for (int i = 0; i < MAXV; i++) {
        adj[i] = nullptr;
    }
}

Graph::~Graph() {
    clearEdges();
}

int Graph::vertexCount() const {
    return vertices;
}

int Graph::edgeCount() const {
    return edges;
}

bool Graph::isDirected() const {
    return directed;
}

// adds v to the list of u, keeping the list sorted by vertex number
bool Graph::insertSorted(int u, int v, int weight) {
    GNode* newNode = new GNode(v, weight);
    if (adj[u] == nullptr || adj[u]->to > v) {
        newNode->next = adj[u];
        adj[u] = newNode;
        return true;
    }
    GNode* temp = adj[u];
    while (temp->next != nullptr && temp->next->to < v) {
        temp = temp->next;
    }
    newNode->next = temp->next;
    temp->next = newNode;
    return true;
}

bool Graph::removeFromList(int u, int v) {
    GNode* prev = nullptr;
    GNode* temp = adj[u];
    while (temp != nullptr && temp->to != v) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == nullptr) {
        return false;
    }
    if (prev == nullptr) {
        adj[u] = temp->next;
    }
    else {
        prev->next = temp->next;
    }
    delete temp;
    return true;
}

bool Graph::hasEdge(int u, int v) const {
    if (u < 0 || u >= vertices || v < 0 || v >= vertices) {
        return false;
    }
    GNode* temp = adj[u];
    while (temp != nullptr) {
        if (temp->to == v) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

bool Graph::addEdge(int u, int v, int weight) {
    if (u < 0 || u >= vertices || v < 0 || v >= vertices || u == v || weight < 1) {
        return false;
    }
    if (hasEdge(u, v)) {
        return false;
    }
    insertSorted(u, v, weight);
    if (!directed) {
        insertSorted(v, u, weight); // an undirected edge is stored in both lists
    }
    edges++;
    return true;
}

bool Graph::removeEdge(int u, int v) {
    if (!hasEdge(u, v)) {
        return false;
    }
    removeFromList(u, v);
    if (!directed) {
        removeFromList(v, u);
    }
    edges--;
    return true;
}

void Graph::clearEdges() {
    for (int i = 0; i < MAXV; i++) {
        while (adj[i] != nullptr) {
            GNode* temp = adj[i];
            adj[i] = adj[i]->next;
            delete temp;
        }
    }
    edges = 0;
}

void Graph::displayList() const {
    for (int u = 0; u < vertices; u++) {
        cout << u << " : ";
        GNode* temp = adj[u];
        while (temp != nullptr) {
            cout << temp->to << "(" << temp->weight << ") -> ";
            temp = temp->next;
        }
        cout << "NULL" << endl;
    }
}

void Graph::displayMatrix() const {
    cout << "    ";
    for (int v = 0; v < vertices; v++) {
        cout << v << (v < 10 ? "   " : "  ");
    }
    cout << endl;
    for (int u = 0; u < vertices; u++) {
        cout << u << (u < 10 ? "   " : "  ");
        for (int v = 0; v < vertices; v++) {
            int w = 0;
            GNode* temp = adj[u];
            while (temp != nullptr) {
                if (temp->to == v) {
                    w = temp->weight;
                }
                temp = temp->next;
            }
            cout << w << (w < 10 ? "   " : "  ");
        }
        cout << endl;
    }
}

int Graph::bfs(int start, int order[]) const {
    if (start < 0 || start >= vertices) {
        return 0;
    }
    Recorder::begin("Breadth first search", CODE_BFS);
    gs.reset();
    bool visited[MAXV];
    for (int i = 0; i < MAXV; i++) visited[i] = false;
    LinkedQueue queue;
    vector<int> shown; // the queue as a plain list, for the pictures
    int count = 0;
    visited[start] = true;
    { RecordPause pause; queue.enqueue(start); }
    shown.push_back(start);
    gs.v[start] = ROLE_NEW;
    snap("Start at vertex " + to_string(start) + ": mark it and put it in the queue.", 0, gs.v, gs.e, shown, "Queue", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
    int v;
    while (true) {
        bool got;
        { RecordPause pause; got = queue.dequeue(v); }
        if (!got) break;
        shown.erase(shown.begin());
        order[count++] = v;
        gs.order.push_back(v);
        gs.v[v] = ROLE_CURRENT;
        Trace::say("visit " + to_string(v));
        snap("Take " + to_string(v) + " from the front of the queue and visit it.", 2, gs.v, gs.e, shown, "Queue", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
        for (GNode* temp = adj[v]; temp != nullptr; temp = temp->next) {
            if (!visited[temp->to]) {
                visited[temp->to] = true; // mark when queued, so a vertex is never queued twice
                { RecordPause pause; queue.enqueue(temp->to); }
                shown.push_back(temp->to);
                gs.v[temp->to] = ROLE_NEW;
                gs.setEdge(v, temp->to, ROLE_NEW);
                snap("Neighbour " + to_string(temp->to) + " is new: mark it and put it at the back of the queue.", 4, gs.v, gs.e, shown, "Queue", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
            }
            else {
                int old = gs.e[v][temp->to];
                gs.setEdge(v, temp->to, ROLE_KEPT);
                snap("Neighbour " + to_string(temp->to) + " was already discovered: skip it.", 4, gs.v, gs.e, shown, "Queue", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
                gs.setEdge(v, temp->to, old);
            }
        }
        gs.v[v] = ROLE_DONE;
    }
    snap("The queue is empty. " + to_string(count) + " vertices were visited, in the order shown.", 5, gs.v, gs.e, shown, "Queue", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
    return count;
}

void Graph::dfsFrom(int vertex, bool visited[], int order[], int& count) const {
    visited[vertex] = true;
    order[count++] = vertex;
    Trace::say("visit " + to_string(vertex));
    if (Recorder::enabled()) {
        gs.order.push_back(vertex);
        gs.path.push_back(vertex);
        gs.v[vertex] = ROLE_CURRENT;
        snap("Visit " + to_string(vertex) + ". It goes on the call stack.", 1, gs.v, gs.e, gs.path, "Call stack", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
    }
    for (GNode* temp = adj[vertex]; temp != nullptr; temp = temp->next) {
        if (!visited[temp->to]) {
            if (Recorder::enabled()) {
                gs.setEdge(vertex, temp->to, ROLE_CURRENT);
                snap("Neighbour " + to_string(temp->to) + " is new: go deeper.", 3, gs.v, gs.e, gs.path, "Call stack", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
                gs.setEdge(vertex, temp->to, ROLE_DONE);
            }
            dfsFrom(temp->to, visited, order, count);
            if (Recorder::enabled()) {
                gs.v[vertex] = ROLE_CURRENT;
                snap("Back at " + to_string(vertex) + ". Look at its next neighbour.", 3, gs.v, gs.e, gs.path, "Call stack", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
            }
        }
        else if (Recorder::enabled()) {
            int old = gs.e[vertex][temp->to];
            gs.setEdge(vertex, temp->to, ROLE_KEPT);
            snap("Neighbour " + to_string(temp->to) + " was already visited: skip it.", 3, gs.v, gs.e, gs.path, "Call stack", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
            gs.setEdge(vertex, temp->to, old);
        }
    }
    if (Recorder::enabled()) {
        gs.v[vertex] = ROLE_DONE;
        gs.path.pop_back();
        snap("All neighbours of " + to_string(vertex) + " are done. Backtrack.", 4, gs.v, gs.e, gs.path, "Call stack", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
    }
}

int Graph::dfs(int start, int order[]) const {
    if (start < 0 || start >= vertices) {
        return 0;
    }
    Recorder::begin("Depth first search", CODE_DFS);
    gs.reset();
    bool visited[MAXV];
    for (int i = 0; i < MAXV; i++) visited[i] = false;
    int count = 0;
    dfsFrom(start, visited, order, count);
    snap("The search is complete. " + to_string(count) + " vertices were visited.", -1, gs.v, gs.e, vector<int>(), "Call stack", vector<string>{ orderText(gs.order, "Visited") }, vector<string>());
    return count;
}

int Graph::dfsIterative(int start, int order[]) const {
    if (start < 0 || start >= vertices) {
        return 0;
    }
    bool visited[MAXV];
    for (int i = 0; i < MAXV; i++) visited[i] = false;

    LinkedStack stack;
    int count = 0;
    stack.push(start);

    int v;
    while (stack.pop(v)) {
        if (visited[v]) {
            continue; // it was reached by another path while waiting on the stack
        }
        visited[v] = true;
        order[count++] = v;
        Trace::say("visit " + to_string(v));

        int neighbours[MAXV];
        int n = 0;
        for (GNode* temp = adj[v]; temp != nullptr; temp = temp->next) {
            neighbours[n++] = temp->to;
        }
        for (int i = n - 1; i >= 0; i--) { // pushed in reverse so the smallest neighbour is popped first
            if (!visited[neighbours[i]]) {
                stack.push(neighbours[i]);
            }
        }
    }
    return count;
}

int Graph::connectedComponents() const {
    bool visited[MAXV];
    for (int i = 0; i < MAXV; i++) visited[i] = false;
    int components = 0;
    int order[MAXV];
    for (int v = 0; v < vertices; v++) {
        if (!visited[v]) {
            int count = 0;
            dfsFrom(v, visited, order, count);
            components++;
        }
    }
    return components;
}

bool Graph::topologicalSort(int order[], int& count) const {
    count = 0;
    if (!directed) {
        return false;
    }
    Recorder::begin("Topological sort", CODE_TOPO);
    gs.reset();
    int indegree[MAXV];
    for (int i = 0; i < MAXV; i++) indegree[i] = 0;
    for (int u = 0; u < vertices; u++) {
        for (GNode* temp = adj[u]; temp != nullptr; temp = temp->next) {
            indegree[temp->to]++;
        }
    }
    auto tags = [&]() {
        vector<string> t(vertices);
        for (int v = 0; v < vertices; v++) t[v] = "in " + to_string(indegree[v]);
        return t;
    };
    snap("Count the incoming edges (indegree) of every vertex.", 0, gs.v, gs.e, vector<int>(), "Queue", vector<string>(), tags());
    LinkedQueue queue;
    vector<int> shown;
    for (int v = 0; v < vertices; v++) {
        if (indegree[v] == 0) {
            { RecordPause pause; queue.enqueue(v); } // vertices nobody depends on can go first
            shown.push_back(v);
            gs.v[v] = ROLE_NEW;
        }
    }
    snap("Vertices with indegree 0 have no prerequisites. They go in the queue first.", 1, gs.v, gs.e, shown, "Queue", vector<string>{ orderText(gs.order, "Order") }, tags());
    int v;
    while (true) {
        bool got;
        { RecordPause pause; got = queue.dequeue(v); }
        if (!got) break;
        shown.erase(shown.begin());
        order[count++] = v;
        gs.order.push_back(v);
        gs.v[v] = ROLE_CURRENT;
        Trace::say("vertex " + to_string(v) + " has no remaining prerequisites, placed in the order");
        snap("Take " + to_string(v) + " from the queue and place it in the order.", 3, gs.v, gs.e, shown, "Queue", vector<string>{ orderText(gs.order, "Order") }, tags());
        for (GNode* temp = adj[v]; temp != nullptr; temp = temp->next) {
            indegree[temp->to]--;
            gs.e[v][temp->to] = ROLE_DONE;
            bool ready = indegree[temp->to] == 0;
            if (ready) {
                { RecordPause pause; queue.enqueue(temp->to); }
                shown.push_back(temp->to);
                gs.v[temp->to] = ROLE_NEW;
            }
            snap("Remove the edge " + to_string(v) + " -> " + to_string(temp->to) + ": the indegree of " + to_string(temp->to) + " is now " + to_string(indegree[temp->to]) + (ready ? ". It has no prerequisites left, so it joins the queue." : "."), 4, gs.v, gs.e, shown, "Queue", vector<string>{ orderText(gs.order, "Order") }, tags());
        }
        gs.v[v] = ROLE_DONE;
    }
    bool ok = (count == vertices);
    if (!ok) {
        for (int i = 0; i < vertices; i++) if (indegree[i] > 0) gs.v[i] = ROLE_KEPT;
    }
    snap(ok ? "Every vertex was placed. This order respects all the edges." : "Some vertices never reached indegree 0: the graph has a cycle, so no valid order exists.", 5, gs.v, gs.e, shown, "Queue", vector<string>{ orderText(gs.order, "Order") }, tags());
    return ok; // fewer means a cycle blocked some vertices
}

int Graph::primMST(int start, int eu[], int ev[], int ew[], int& edgeCountOut, bool& connected) const {
    edgeCountOut = 0;
    connected = false;
    if (directed || start < 0 || start >= vertices) {
        return -1;
    }
    Recorder::begin("Prim: minimum spanning tree", CODE_PRIM);
    gs.reset();
    const int INF = 2000000000;
    bool inTree[MAXV];
    int key[MAXV];
    int parent[MAXV];
    for (int i = 0; i < MAXV; i++) {
        inTree[i] = false;
        key[i] = INF;
        parent[i] = -1;
    }
    key[start] = 0;
    int total = 0;
    int added = 0;
    auto tags = [&]() {
        vector<string> t(vertices);
        for (int v = 0; v < vertices; v++) if (!inTree[v] && key[v] < INF) t[v] = "key " + to_string(key[v]);
        return t;
    };
    auto info = [&]() { return vector<string>{ "Tree weight so far: " + to_string(total) }; };
    snap("Start the tree at vertex " + to_string(start) + " (key 0). Every other vertex has key = infinity.", 0, gs.v, gs.e, vector<int>(), "", info(), tags());
    for (int round = 0; round < vertices; round++) {
        int best = -1;
        for (int v = 0; v < vertices; v++) { // cheapest vertex that is not in the tree yet
            if (!inTree[v] && key[v] < INF && (best == -1 || key[v] < key[best])) {
                best = v;
            }
        }
        if (best == -1) {
            break; // the rest cannot be reached
        }
        gs.v[best] = ROLE_CURRENT;
        snap("Vertex " + to_string(best) + " has the smallest key (" + to_string(key[best]) + ") outside the tree. Add it.", 2, gs.v, gs.e, vector<int>(), "", info(), tags());
        inTree[best] = true;
        added++;
        if (parent[best] != -1) {
            eu[edgeCountOut] = parent[best];
            ev[edgeCountOut] = best;
            ew[edgeCountOut] = key[best];
            edgeCountOut++;
            total += key[best];
            gs.setEdge(parent[best], best, ROLE_DONE);
            Trace::say("edge " + to_string(parent[best]) + " - " + to_string(best) + " (weight " + to_string(key[best]) + ") added to the tree");
        }
        gs.v[best] = ROLE_DONE;
        snap(parent[best] != -1 ? "The edge " + to_string(parent[best]) + " - " + to_string(best) + " (weight " + to_string(key[best]) + ") joins the tree." : "The tree starts with vertex " + to_string(best) + ".", 3, gs.v, gs.e, vector<int>(), "", info(), tags());
        for (GNode* temp = adj[best]; temp != nullptr; temp = temp->next) {
            if (!inTree[temp->to] && temp->weight < key[temp->to]) {
                key[temp->to] = temp->weight;
                parent[temp->to] = best;
                int old = gs.e[best][temp->to];
                gs.setEdge(best, temp->to, ROLE_CURRENT);
                snap("Edge " + to_string(best) + " - " + to_string(temp->to) + " (weight " + to_string(temp->weight) + ") is cheaper: key of " + to_string(temp->to) + " becomes " + to_string(temp->weight) + ".", 5, gs.v, gs.e, vector<int>(), "", info(), tags());
                gs.setEdge(best, temp->to, old);
            }
        }
    }
    connected = (added == vertices);
    snap(connected ? "Done. The minimum spanning tree has weight " + to_string(total) + "." : "The graph is not connected: only the part reachable from the start was spanned (weight " + to_string(total) + ").", -1, gs.v, gs.e, vector<int>(), "", info(), vector<string>());
    return total;
}

static int findRoot(int parent[], int v) {
    while (parent[v] != v) {
        parent[v] = parent[parent[v]]; // path compression: point closer to the root while walking up
        v = parent[v];
    }
    return v;
}

int Graph::kruskalMST(int eu[], int ev[], int ew[], int& edgeCountOut, bool& connected) const {
    edgeCountOut = 0;
    connected = false;
    if (directed) {
        return -1;
    }
    Recorder::begin("Kruskal: minimum spanning tree", CODE_KRUSKAL);
    gs.reset();
    int allU[MAXV * MAXV], allV[MAXV * MAXV], allW[MAXV * MAXV];
    int total = 0;
    for (int u = 0; u < vertices; u++) {
        for (GNode* temp = adj[u]; temp != nullptr; temp = temp->next) {
            if (u < temp->to) {
                allU[total] = u;
                allV[total] = temp->to;
                allW[total] = temp->weight;
                total++;
            }
        }
    }
    for (int i = 1; i < total; i++) {
        int u = allU[i], v = allV[i], w = allW[i];
        int j = i - 1;
        while (j >= 0 && allW[j] > w) {
            allU[j + 1] = allU[j];
            allV[j + 1] = allV[j];
            allW[j + 1] = allW[j];
            j--;
        }
        allU[j + 1] = u;
        allV[j + 1] = v;
        allW[j + 1] = w;
    }
    int parent[MAXV];
    for (int i = 0; i < MAXV; i++) parent[i] = i;
    int weightSum = 0;
    string sorted = "Edges by weight: ";
    for (int i = 0; i < total; i++) sorted += to_string(allU[i]) + "-" + to_string(allV[i]) + " (" + to_string(allW[i]) + ")  ";
    auto info = [&]() { return vector<string>{ sorted, "Tree weight so far: " + to_string(weightSum) }; };
    snap("Sort all edges by weight. Then take them from the cheapest one.", 0, gs.v, gs.e, vector<int>(), "", info(), vector<string>());
    for (int i = 0; i < total; i++) {
        int rootU = findRoot(parent, allU[i]);
        int rootV = findRoot(parent, allV[i]);
        gs.setEdge(allU[i], allV[i], ROLE_CURRENT);
        snap("Look at the edge " + to_string(allU[i]) + " - " + to_string(allV[i]) + " (weight " + to_string(allW[i]) + "). Are its ends already connected?", 2, gs.v, gs.e, vector<int>(), "", info(), vector<string>());
        if (rootU != rootV) { // different groups, so this edge cannot form a cycle
            parent[rootU] = rootV;
            eu[edgeCountOut] = allU[i];
            ev[edgeCountOut] = allV[i];
            ew[edgeCountOut] = allW[i];
            edgeCountOut++;
            weightSum += allW[i];
            gs.setEdge(allU[i], allV[i], ROLE_DONE);
            gs.v[allU[i]] = ROLE_DONE;
            gs.v[allV[i]] = ROLE_DONE;
            Trace::say("edge " + to_string(allU[i]) + " - " + to_string(allV[i]) + " (weight " + to_string(allW[i]) + ") accepted");
            snap("No. They are in different groups, so the edge is accepted and the groups merge.", 3, gs.v, gs.e, vector<int>(), "", info(), vector<string>());
        }
        else {
            gs.setEdge(allU[i], allV[i], ROLE_KEPT);
            Trace::say("edge " + to_string(allU[i]) + " - " + to_string(allV[i]) + " (weight " + to_string(allW[i]) + ") skipped, it would make a cycle");
            snap("Yes. Adding this edge would close a cycle, so it is skipped.", 4, gs.v, gs.e, vector<int>(), "", info(), vector<string>());
            gs.setEdge(allU[i], allV[i], ROLE_NORMAL);
        }
    }
    connected = (edgeCountOut == vertices - 1);
    snap(connected ? "Done. The minimum spanning tree has weight " + to_string(weightSum) + "." : "The graph is not connected, so this is a spanning forest of weight " + to_string(weightSum) + ".", -1, gs.v, gs.e, vector<int>(), "", info(), vector<string>());
    return weightSum;
}
