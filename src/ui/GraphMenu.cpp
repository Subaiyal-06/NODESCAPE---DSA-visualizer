#include "GraphMenu.h"
#include "ConsoleUtils.h"
#include "../core/Graph.h"
#include <iostream>
using namespace std;

static void printOrder(const char* title, const int order[], int count) {
    cout << title << ": ";
    for (int i = 0; i < count; i++) {
        cout << order[i];
        if (i < count - 1) cout << " -> ";
    }
    cout << endl;
}

static Graph* createGraph() {
    int vertices = readIntInRange("Number of vertices (2 to " + to_string(Graph::MAXV) + "): ", 2, Graph::MAXV);
    cout << " 1. Undirected graph\n 2. Directed graph" << endl;
    int type = readIntInRange("Type: ", 1, 2);
    return new Graph(vertices, type == 2);
}

// a small weighted undirected graph that is good for traversals and spanning trees
static Graph* sampleUndirected() {
    Graph* g = new Graph(6, false);
    g->addEdge(0, 1, 4); g->addEdge(0, 2, 3); g->addEdge(1, 2, 1); g->addEdge(1, 3, 2);
    g->addEdge(2, 3, 4); g->addEdge(3, 4, 2); g->addEdge(4, 5, 6);
    return g;
}

// a directed graph without cycles: good for topological sort
static Graph* sampleDirected() {
    Graph* g = new Graph(6, true);
    g->addEdge(5, 2, 1); g->addEdge(5, 0, 1); g->addEdge(4, 0, 1);
    g->addEdge(4, 1, 1); g->addEdge(2, 3, 1); g->addEdge(3, 1, 1);
    return g;
}

void runGraphMenu() {
    Graph* graph = createGraph();
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printTitle("Graph");
        cout << (graph->isDirected() ? "Directed" : "Undirected") << " graph, "
             << graph->vertexCount() << " vertices, " << graph->edgeCount() << " edges" << endl;
        cout << "Adjacency list (neighbour(weight)):" << endl;
        graph->displayList();
        cout << endl;
        cout << " 1. Add an edge" << endl;
        cout << " 2. Remove an edge" << endl;
        cout << " 3. Show the adjacency matrix" << endl;
        cout << " 4. BFS (breadth first, uses a queue)" << endl;
        cout << " 5. DFS (depth first, recursion)" << endl;
        cout << " 6. DFS (depth first, uses a stack)" << endl;
        cout << " 7. Connected components" << endl;
        cout << " 8. Minimum spanning tree: Prim" << endl;
        cout << " 9. Minimum spanning tree: Kruskal" << endl;
        cout << "10. Topological sort" << endl;
        cout << "11. Load the sample undirected graph" << endl;
        cout << "12. Load the sample directed graph (no cycles)" << endl;
        cout << "13. Start a new empty graph" << endl;
        cout << " 0. Back to main menu\n" << endl;

        choice = readInt("Your choice: ");
        int order[Graph::MAXV];
        int eu[Graph::MAXV], ev[Graph::MAXV], ew[Graph::MAXV];

        if (choice == 1) {
            int u = readInt("From vertex: ");
            int v = readInt("To vertex: ");
            int w = readInt("Weight (1 or more): ");
            if (graph->addEdge(u, v, w)) cout << "Edge added." << endl;
            else cout << "The edge was not added (bad vertex, same vertex twice, edge exists, or weight below 1)." << endl;
        }
        else if (choice == 2) {
            int u = readInt("From vertex: ");
            int v = readInt("To vertex: ");
            if (graph->removeEdge(u, v)) cout << "Edge removed." << endl;
            else cout << "There is no such edge." << endl;
        }
        else if (choice == 3) {
            graph->displayMatrix();
            printComplexity("O(V^2) memory for the matrix, a list needs only O(V + E)");
        }
        else if (choice >= 4 && choice <= 6) {
            int start = readInt("Start vertex: ");
            int count;
            if (choice == 4) count = graph->bfs(start, order);
            else if (choice == 5) count = graph->dfs(start, order);
            else count = graph->dfsIterative(start, order);

            if (count == 0) cout << "Invalid start vertex." << endl;
            else {
                printOrder(choice == 4 ? "BFS order" : "DFS order", order, count);
                if (count < graph->vertexCount()) cout << "Only " << count << " of " << graph->vertexCount() << " vertices can be reached from " << start << "." << endl;
                printComplexity("O(V + E)");
            }
        }
        else if (choice == 7) {
            if (graph->isDirected()) cout << "Components are defined for undirected graphs. Start a new undirected graph." << endl;
            else cout << "Connected components: " << graph->connectedComponents() << endl;
            printComplexity("O(V + E)");
        }
        else if (choice == 8 || choice == 9) {
            if (graph->isDirected()) {
                cout << "A minimum spanning tree needs an undirected graph." << endl;
            } else {
                int count;
                bool connected;
                int total;
                if (choice == 8) {
                    int start = readInt("Start vertex: ");
                    total = graph->primMST(start, eu, ev, ew, count, connected);
                } else {
                    total = graph->kruskalMST(eu, ev, ew, count, connected);
                }
                if (total == -1) cout << "Invalid start vertex." << endl;
                else {
                    cout << "Edges of the tree:" << endl;
                    for (int i = 0; i < count; i++) {
                        cout << "  " << eu[i] << " - " << ev[i] << "  (weight " << ew[i] << ")" << endl;
                    }
                    cout << "Total weight: " << total << endl;
                    if (!connected) cout << "The graph is not connected, so this is only a part of a spanning tree." << endl;
                    printComplexity(choice == 8 ? "O(V^2) with this array version of Prim" : "O(E log E), mostly the sorting of the edges");
                }
            }
        }
        else if (choice == 10) {
            int count;
            if (!graph->isDirected()) {
                cout << "Topological sort needs a directed graph." << endl;
            } else if (graph->topologicalSort(order, count)) {
                printOrder("Topological order", order, count);
                printComplexity("O(V + E)");
            } else {
                cout << "The graph has a cycle, so no topological order exists." << endl;
            }
        }
        else if (choice == 11 || choice == 12 || choice == 13) {
            delete graph;
            if (choice == 11) graph = sampleUndirected();
            else if (choice == 12) graph = sampleDirected();
            else graph = createGraph();
            cout << "Done." << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice. Please pick a number from the menu." << endl;
        }

        if (choice != 0) {
            pauseScreen();
        }
    }
    delete graph;
}
