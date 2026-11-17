// Engines, operations, toolbars and the building of sessions for every view.

#include "GUIApp.h"
#include "../core/Sorting.h"
#include "../core/StackApps.h"
#include "../core/Recursion.h"
#include "../core/Complexity.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

using namespace gfx;

// ============================================================================
//  Recording helpers
// ============================================================================

void GUIApp::captureSession(bool autoplay) {
    Session& s = S();
    int n = Recorder::frameCount();
    if (n == 0) {
        toast("Nothing to animate for that action.");
        return;
    }
    s.frames.clear();
    for (int i = 0; i < n; i++) s.frames.push_back(Recorder::frame(i));
    s.code = Recorder::code();
    s.title = Recorder::title();
    s.index = 0;
    s.built = true;
    playing = autoplay && n > 1;
    stepTimer = 0.0f;
    if (Recorder::truncated()) toast("Very long operation: only the first steps are shown.");
}

static bool keepMarker(const std::string& label) {
    static const char* keep[] = { "head", "tail", "front", "rear", "top", "front / rear", "head / tail", "deleted" };
    for (const char* k : keep) if (label == k) return true;
    return label.compare(0, 3, "in ") == 0 || label.compare(0, 4, "key ") == 0;
}

// turns the last frame of the last recording into a calm "resting" picture
void GUIApp::setRest(const std::string& message) {
    Session& s = S();
    Frame rest;
    int n = Recorder::frameCount();
    if (n > 0) rest = Recorder::frame(n - 1);
    for (auto& c : rest.cells) c.role = ROLE_NORMAL;
    for (auto& t : rest.nodes) t.role = ROLE_NORMAL;
    for (auto& chain : rest.chains) for (auto& c : chain) c.role = ROLE_NORMAL;
    for (auto& v : rest.vertices) v.role = ROLE_NORMAL;
    for (auto& e : rest.edges) e.role = ROLE_NORMAL;
    rest.hasFloating = false;
    rest.info.clear();
    std::vector<Marker> keep;
    for (auto& m : rest.markers) if (keepMarker(m.label)) keep.push_back(m);
    rest.markers = keep;
    rest.message = message;
    rest.codeLine = -1;
    rest.comparisons = 0;
    rest.swaps = 0;
    s.frames.assign(1, rest);
    s.code = Recorder::code();
    s.title = (n > 0) ? Recorder::title() : "";
    s.index = 0;
    s.built = true;
    playing = false;
}

void GUIApp::emptyPicture(const std::string& message, FrameKind kind) {
    Session& s = S();
    Frame f;
    f.kind = kind;
    f.message = message;
    s.frames.assign(1, f);
    s.index = 0;
    s.code.clear();
    s.title = "";
    s.built = true;
    playing = false;
}

// ============================================================================
//  Sorting
// ============================================================================

void GUIApp::runSort() {
    Recorder::setEnabled(true);
    Recorder::clear();
    std::vector<int> a = array;
    SortStats stats;
    int n = (int)a.size();
    switch (sortAlgo) {
        case ALGO_BUBBLE:    bubbleSort(a.data(), n, stats); break;
        case ALGO_SELECTION: selectionSort(a.data(), n, stats); break;
        case ALGO_INSERTION: insertionSort(a.data(), n, stats); break;
        case ALGO_MERGE:     mergeSort(a.data(), n, stats); break;
        case ALGO_QUICK:     quickSort(a.data(), n, stats); break;
        default:             heapSort(a.data(), n, stats); break;
    }
    View saved = view;
    view = V_SORT;
    captureSession(false);
    view = saved;
}

void GUIApp::newArray(int mode) {
    if (mode == 0 || array.empty()) {
        array.clear();
        for (int i = 0; i < arraySize; i++) array.push_back(10 + std::rand() % 90);
    }
    else {
        while ((int)array.size() < arraySize) array.push_back(10 + std::rand() % 90);
        array.resize(arraySize);
        if (mode == 1) {
            std::sort(array.begin(), array.end(), std::greater<int>());
        }
        else if (mode == 2) {
            std::sort(array.begin(), array.end());
            if (array.size() >= 4) {
                int idx = std::rand() % ((int)array.size() - 2);
                std::swap(array[idx], array[idx + 1]);
            }
        }
    }
}

// ============================================================================
//  Graph and hash helpers
// ============================================================================

void GUIApp::rebuildGraph(Graph& g) const {
    for (const GEdge& e : gedges) g.addEdge(e.u, e.v, e.w);
}

void GUIApp::sampleGraph(bool directed) {
    graphDirected = directed;
    graphN = 7;
    gedges.clear();
    if (!directed) {
        const int e[][3] = { { 0, 1, 7 }, { 0, 3, 5 }, { 1, 2, 8 }, { 1, 3, 9 }, { 1, 4, 7 }, { 2, 4, 5 }, { 3, 4, 15 }, { 3, 5, 6 }, { 4, 5, 8 }, { 4, 6, 9 }, { 5, 6, 11 } };
        for (auto& x : e) gedges.push_back({ x[0], x[1], x[2] });
    }
    else {
        const int e[][3] = { { 0, 1, 1 }, { 0, 2, 1 }, { 1, 3, 1 }, { 2, 3, 1 }, { 2, 5, 1 }, { 3, 4, 1 }, { 5, 4, 1 }, { 4, 6, 1 } };
        for (auto& x : e) gedges.push_back({ x[0], x[1], x[2] });
    }
}

void GUIApp::newHash() {
    CollisionMethod methods[4] = { CHAINING, LINEAR_PROBING, QUADRATIC_PROBING, DOUBLE_HASHING };
    HashFunction funcs[3] = { DIVISION, FOLDING, MID_SQUARE };
    ht.reset(new HashTable(hashSize, methods[hashMethod], funcs[hashFunc]));
}

void GUIApp::buildComplexity() {
    Session& s = sessions[V_COMPLEX];
    s.frames.clear();
    for (int n = 1; n <= 64; n++) {
        Frame f;
        f.kind = KIND_LIST;
        auto v = [&](ComplexityKind k) { long long x = stepsFor(k, n); return x < 0 ? std::string("too many") : std::to_string(x); };
        f.message = "With n = " + std::to_string(n) + ": O(log n) needs about " + v(LOGARITHMIC) + " steps, O(n) needs " + v(LINEAR) + ", O(n^2) needs " + v(QUADRATIC) + " and O(2^n) needs " + v(EXPONENTIAL) + ".";
        s.frames.push_back(f);
    }
    s.title = "Growth of common running times";
    s.code.clear();
    s.index = 11;
    s.built = true;
}

// ============================================================================
//  Building a view for the first time
// ============================================================================

void GUIApp::buildView(View v) {
    Session& s = sessions[v];
    if (s.built) return;
    Recorder::setEnabled(true);
    Recorder::clear();
    View saved = view;
    view = v;
    switch (v) {
        case V_SORT:
            runSort();
            sessions[V_SORT].index = 0;
            break;
        case V_ARRAY:
            if (!dyn) dyn.reset(new DynamicArray(4));
            for (int x : { 12, 45, 23, 67, 34 }) dyn->append(x);
            setRest("A dynamic array doubles its capacity when it is full. Empty slots are dashed.");
            break;
        case V_LIST_S:
            for (int x : { 12, 27, 41, 63 }) list.insertAtEnd(x);
            setRest("A singly linked list. Pick an operation above and watch the pointers move.");
            break;
        case V_LIST_D:
            for (int x : { 12, 27, 41, 63 }) dlist.insertAtEnd(x);
            setRest("A doubly linked list: every node knows its next and its previous node.");
            break;
        case V_LIST_C:
            for (int x : { 12, 27, 41, 63 }) clist.insertAtEnd(x);
            setRest("A circular linked list: the last node points back to the first one.");
            break;
        case V_STACK_ARR:
            for (int x : { 15, 28, 42 }) astack.push(x);
            setRest("A stack in an array (8 slots). Only the top can be pushed or popped.");
            break;
        case V_STACK_LINK:
            for (int x : { 15, 28, 42 }) lstack.push(x);
            setRest("A stack made of linked nodes. It can grow as long as memory lasts.");
            break;
        case V_STACK_APPS:
            appsMode = 0;
            inText.text = "{[a+b]*(c-d)}";
            checkBracketsRecorded(inText.text);
            captureSession(false);
            sessions[V_STACK_APPS].index = 0;
            break;
        case V_RECURSION:
            recMode = 0;
            inA.text = "4";
            factorialRecorded(4);
            captureSession(false);
            sessions[V_RECURSION].index = 0;
            break;
        case V_Q_SIMPLE:
            for (int x : { 15, 28, 42 }) aqueue.enqueue(x);
            setRest("A queue in an array. Dequeue until the rear reaches the end to see its weakness.");
            break;
        case V_Q_CIRC:
            for (int x : { 15, 28, 42 }) cqueue.enqueue(x);
            setRest("A circular queue: the indexes wrap around, so no slot is wasted.");
            break;
        case V_DEQUE:
            for (int x : { 15, 28, 42 }) deque.insertRear(x);
            setRest("A deque: values can be inserted and deleted at both ends.");
            break;
        case V_Q_LINK:
            for (int x : { 15, 28, 42 }) lqueue.enqueue(x);
            setRest("A queue made of linked nodes: front is the head, rear is the tail.");
            break;
        case V_BST:
            for (int x : { 50, 30, 70, 20, 40, 60, 80 }) bst.insert(x);
            setRest("A binary search tree: smaller values go left, larger values go right.");
            break;
        case V_AVL:
            for (int x : { 40, 20, 60, 10, 30, 50, 70 }) avl.insert(x);
            setRest("An AVL tree. The small numbers under the nodes are balance factors.");
            break;
        case V_HEAP:
            for (int x : { 90, 70, 80, 40, 50, 60, 30 }) heap.insert(x);
            setRest("A max heap: every parent is larger than its children.");
            break;
        case V_GRAPH: {
            Graph g(graphN, graphDirected);
            rebuildGraph(g);
            g.showGraph(graphDirected ? "A directed graph (a DAG). Try a topological sort." : "A weighted undirected graph. Try BFS, DFS, Prim or Kruskal.");
            setRest(graphDirected ? "A directed graph (a DAG). Try a topological sort." : "A weighted undirected graph. Try BFS, DFS, Prim or Kruskal.");
            break;
        }
        case V_HASH:
            if (!ht) newHash();
            for (int x : { 15, 26, 7, 37 }) ht->insert(x);
            setRest("Keys that hash to the same slot collide. Watch how the method resolves it.");
            break;
        case V_COMPLEX:
            buildComplexity();
            break;
        default:
            break;
    }
    view = saved;
}

// ============================================================================
//  Operations
// ============================================================================

void GUIApp::act(int op) {
    switch (groupOf(view)) {
        case G_ARRAY: actArray(op); break;
        case G_LIST: actList(op); break;
        case G_STACK:
            if (view == V_STACK_APPS) actApps(op);
            else if (view == V_RECURSION) actRecursion(op);
            else actStack(op);
            break;
        case G_QUEUE: actQueue(op); break;
        case G_TREE: if (view == V_BST) actBst(op); else actAvl(op); break;
        case G_HEAP: actHeap(op); break;
        case G_GRAPH: actGraph(op); break;
        case G_HASH: actHash(op); break;
        default: break;
    }
}

void GUIApp::primaryAction() {
    switch (view) {
        case V_ARRAY: act(0); break;
        case V_LIST_S: case V_LIST_D: case V_LIST_C: act(1); break;
        case V_STACK_ARR: case V_STACK_LINK: act(0); break;
        case V_STACK_APPS: case V_RECURSION: act(0); break;
        case V_Q_SIMPLE: case V_Q_CIRC: case V_Q_LINK: act(0); break;
        case V_DEQUE: act(1); break;
        case V_BST: case V_AVL: case V_HEAP: act(0); break;
        case V_GRAPH: act(0); break;
        case V_HASH: act(0); break;
        default: break;
    }
}

void GUIApp::actArray(int op) {
    if (!dyn) dyn.reset(new DynamicArray(4));
    Recorder::setEnabled(true);
    Recorder::clear();
    int v = 0, idx = 0;
    bool needsValue = (op == 0 || op == 1 || op == 3 || op == 4);
    if (needsValue && !readA(v)) return;
    if ((op == 1 || op == 2) && !readB(idx, "index")) return;
    if ((op == 0 || op == 1) && dyn->size() >= 16) { toast("The picture holds up to 16 values."); return; }
    switch (op) {
        case 0: dyn->append(v); break;
        case 1: dyn->insertAt(idx, v); break;
        case 2: dyn->removeAt(idx); break;
        case 3: dyn->linearSearch(v); break;
        case 4: dyn->binarySearch(v); break;
        case 5: dyn->sortAscending(); break;
        case 6: {
            dyn.reset(new DynamicArray(4));
            for (int i = 0; i < 6; i++) dyn->append(10 + std::rand() % 90);
            dyn->sortAscending();
            break;
        }
        case 7:
            dyn.reset(new DynamicArray(4));
            dyn->showArray("The array is empty again (capacity 4).");
            setRest("The array is empty again (capacity 4).");
            return;
    }
    captureSession(true);
}

void GUIApp::actList(int op) {
    Recorder::setEnabled(true);
    Recorder::clear();
    int v = 0, pos = 0;
    bool needsValue = (op == 0 || op == 1 || op == 2 || op == 5 || op == 6 || op == 9);
    if (needsValue && !readA(v)) return;
    if (op == 2 && !readB(pos, "pos")) return;
    int size = view == V_LIST_S ? list.size() : (view == V_LIST_D ? dlist.size() : clist.size());
    if ((op == 0 || op == 1 || op == 2) && size >= 14) {
        toast("The picture holds up to 14 nodes. Delete something first.");
        return;
    }
    if (view == V_LIST_S) {
        switch (op) {
            case 0: list.insertAtBeginning(v); break;
            case 1: list.insertAtEnd(v); break;
            case 2: list.insertAtPosition(pos, v); break;
            case 3: list.deleteFirst(); break;
            case 4: list.deleteLast(); break;
            case 5: list.deleteByValue(v); break;
            case 6: list.search(v); break;
            case 7: list.reverse(); break;
            case 8: list.sortAscending(); break;
            case 10: list.clear(); Recorder::clear(); list.deleteFirst(); setRest("The list is empty."); return;
        }
    }
    else if (view == V_LIST_D) {
        switch (op) {
            case 0: dlist.insertAtBeginning(v); break;
            case 1: dlist.insertAtEnd(v); break;
            case 2: dlist.insertAtPosition(pos, v); break;
            case 3: dlist.deleteFirst(); break;
            case 4: dlist.deleteLast(); break;
            case 5: dlist.deleteByValue(v); break;
            case 6: dlist.search(v); break;
            case 7: dlist.reverse(); break;
            case 9: dlist.searchFromEnd(v); break;
            case 10: dlist.clear(); Recorder::clear(); dlist.deleteFirst(); setRest("The list is empty."); return;
        }
    }
    else {
        switch (op) {
            case 0: clist.insertAtBeginning(v); break;
            case 1: clist.insertAtEnd(v); break;
            case 2: clist.insertAtPosition(pos, v); break;
            case 3: clist.deleteFirst(); break;
            case 4: clist.deleteLast(); break;
            case 5: clist.deleteByValue(v); break;
            case 6: clist.search(v); break;
            case 10: clist.clear(); Recorder::clear(); clist.deleteFirst(); setRest("The list is empty."); return;
        }
    }
    captureSession(true);
}

void GUIApp::actStack(int op) {
    Recorder::setEnabled(true);
    Recorder::clear();
    int v = 0;
    if (op == 0 && !readA(v)) return;
    if (view == V_STACK_ARR) {
        int out = 0;
        if (op == 0) astack.push(v);
        else if (op == 1) astack.pop(out);
        else { astack.clear(); Recorder::clear(); astack.pop(out); setRest("The stack is empty."); return; }
    }
    else {
        int out = 0;
        if (op == 0) {
            if (lstack.size() >= 12) { toast("The picture holds up to 12 nodes."); return; }
            lstack.push(v);
        }
        else if (op == 1) lstack.pop(out);
        else { lstack.clear(); Recorder::clear(); lstack.pop(out); setRest("The stack is empty."); return; }
    }
    captureSession(true);
}

void GUIApp::actQueue(int op) {
    Recorder::setEnabled(true);
    Recorder::clear();
    int v = 0, out = 0;
    bool insert = (view == V_DEQUE) ? (op == 0 || op == 1) : (op == 0);
    if (insert && !readA(v)) return;
    switch (view) {
        case V_Q_SIMPLE:
            if (op == 0) aqueue.enqueue(v);
            else if (op == 1) aqueue.dequeue(out);
            else { aqueue.clear(); Recorder::clear(); aqueue.dequeue(out); setRest("The queue is empty."); return; }
            break;
        case V_Q_CIRC:
            if (op == 0) cqueue.enqueue(v);
            else if (op == 1) cqueue.dequeue(out);
            else { cqueue.clear(); Recorder::clear(); cqueue.dequeue(out); setRest("The queue is empty."); return; }
            break;
        case V_DEQUE:
            if (op == 0) deque.insertFront(v);
            else if (op == 1) deque.insertRear(v);
            else if (op == 2) deque.deleteFront(out);
            else if (op == 3) deque.deleteRear(out);
            else { deque.clear(); Recorder::clear(); deque.deleteFront(out); setRest("The deque is empty."); return; }
            break;
        default:
            if (op == 0) {
                if (lqueue.size() >= 12) { toast("The picture holds up to 12 nodes."); return; }
                lqueue.enqueue(v);
            }
            else if (op == 1) lqueue.dequeue(out);
            else { lqueue.clear(); Recorder::clear(); lqueue.dequeue(out); setRest("The queue is empty."); return; }
            break;
    }
    captureSession(true);
}

void GUIApp::actBst(int op) {
    Recorder::setEnabled(true);
    Recorder::clear();
    int v = 0;
    if (op <= 2 && !readA(v)) return;
    if (op == 0 && bst.size() >= 40) { toast("The picture holds up to 40 nodes."); return; }
    int out[64];
    switch (op) {
        case 0: bst.insert(v); break;
        case 1: bst.remove(v); break;
        case 2: bst.search(v); break;
        case 3: bst.preorder(out, 64); break;
        case 4: bst.inorder(out, 64); break;
        case 5: bst.postorder(out, 64); break;
        case 6: bst.levelOrder(out, 64); break;
        case 7:
            if (bst.size() >= 40) { toast("The picture holds up to 40 nodes."); return; }
            for (int tries = 0; tries < 20; tries++) if (bst.insert(10 + std::rand() % 90)) break;
            break;
        case 8:
            bst.clear();
            emptyPicture("The tree is empty.", KIND_TREE);
            return;
    }
    captureSession(true);
}

void GUIApp::actAvl(int op) {
    Recorder::setEnabled(true);
    Recorder::clear();
    int v = 0;
    if (op <= 1 && !readA(v)) return;
    if ((op == 0 || op == 2) && avl.size() >= 40) { toast("The picture holds up to 40 nodes."); return; }
    switch (op) {
        case 0: avl.insert(v); break;
        case 1: avl.remove(v); break;
        case 2:
            for (int tries = 0; tries < 20; tries++) if (avl.insert(10 + std::rand() % 90)) break;
            break;
        case 3:
            avl.clear();
            emptyPicture("The tree is empty.", KIND_TREE);
            return;
        case 100: case 101: case 102: case 103: { // the four rotation cases
            static const int seq[4][3] = { { 30, 20, 10 }, { 10, 20, 30 }, { 30, 10, 20 }, { 10, 30, 20 } };
            int kind = op - 100;
            avl.clear();
            avl.insert(seq[kind][0]);
            avl.insert(seq[kind][1]);
            Recorder::clear();
            avl.insert(seq[kind][2]);
            break;
        }
    }
    captureSession(true);
}

void GUIApp::actHeap(int op) {
    Recorder::setEnabled(true);
    Recorder::clear();
    int v = 0;
    if (op == 0 && !readA(v)) return;
    switch (op) {
        case 0:
            if (heap.isFull()) { toast("The heap holds up to 31 values."); return; }
            heap.insert(v);
            break;
        case 1: {
            int top = 0;
            if (!heap.extractTop(top)) { toast("The heap is empty."); return; }
            break;
        }
        case 2:
            if (heap.isFull()) { toast("The heap holds up to 31 values."); return; }
            heap.insert(10 + std::rand() % 90);
            break;
        case 3:
            heap.clear();
            emptyPicture("The heap is empty.", KIND_TREE);
            return;
        case 4: { // switch between max heap and min heap, keeping the values
            std::vector<int> values;
            for (int i = 0; i < heap.size(); i++) { int x = 0; heap.getAt(i, x); values.push_back(x); }
            heapIsMax = !heapIsMax;
            heap = BinaryHeap(heapIsMax);
            Recorder::clear();
            for (int x : values) heap.insert(x);
            if (values.empty()) { emptyPicture("The heap is empty.", KIND_TREE); return; }
            setRest(heapIsMax ? "Max heap: every parent is larger than its children." : "Min heap: every parent is smaller than its children.");
            return;
        }
    }
    captureSession(true);
}

void GUIApp::actGraph(int op) {
    Recorder::setEnabled(true);
    Recorder::clear();
    auto show = [&](const std::string& msg) {
        Graph g(graphN, graphDirected);
        rebuildGraph(g);
        Recorder::clear();
        g.showGraph(msg);
        setRest(msg);
    };
    int a = 0, b = 0, w = 1;
    switch (op) {
        case 0: case 1: {
            if (!readA(a, "From") || !readB(b, "To")) return;
            if (!inC.text.empty()) w = std::max(1, std::atoi(inC.text.c_str()));
            if (a >= graphN || b >= graphN) { toast("Vertices go from 0 to " + std::to_string(graphN - 1) + "."); return; }
            if (op == 0) {
                Graph g(graphN, graphDirected);
                rebuildGraph(g);
                if (!g.addEdge(a, b, w)) { toast("That edge already exists, or it is not valid."); return; }
                gedges.push_back({ a, b, w });
                show("Edge " + std::to_string(a) + (graphDirected ? " -> " : " - ") + std::to_string(b) + " (weight " + std::to_string(w) + ") added.");
            }
            else {
                bool found = false;
                for (size_t i = 0; i < gedges.size() && !found; i++) {
                    bool same = gedges[i].u == a && gedges[i].v == b;
                    bool rev = !graphDirected && gedges[i].u == b && gedges[i].v == a;
                    if (same || rev) { gedges.erase(gedges.begin() + i); found = true; }
                }
                if (!found) { toast("There is no such edge."); return; }
                show("Edge removed.");
            }
            return;
        }
        case 2: case 3: case 4: case 5: case 6: {
            int order[Graph::MAXV];
            int start = 0;
            if (op <= 4) {
                if (!inA.text.empty()) start = std::atoi(inA.text.c_str());
                if (start >= graphN) { toast("The start vertex must be 0 to " + std::to_string(graphN - 1) + "."); return; }
            }
            Graph g(graphN, graphDirected);
            rebuildGraph(g);
            Recorder::clear();
            int eu[Graph::MAXV * Graph::MAXV], ev[Graph::MAXV * Graph::MAXV], ew[Graph::MAXV * Graph::MAXV], ec = 0;
            bool connected = false;
            int count = 0;
            if (op == 2) g.bfs(start, order);
            else if (op == 3) g.dfs(start, order);
            else if (op == 4) {
                if (graphDirected) { toast("Prim needs an undirected graph. Switch with the Directed button."); return; }
                g.primMST(start, eu, ev, ew, ec, connected);
            }
            else if (op == 5) {
                if (graphDirected) { toast("Kruskal needs an undirected graph. Switch with the Directed button."); return; }
                g.kruskalMST(eu, ev, ew, ec, connected);
            }
            else {
                if (!graphDirected) { toast("Topological sort needs a directed graph. Switch with the Directed button."); return; }
                g.topologicalSort(order, count);
            }
            captureSession(true);
            return;
        }
        case 7: // directed or undirected
            graphDirected = !graphDirected;
            sampleGraph(graphDirected);
            show(graphDirected ? "A directed graph (a DAG). Try a topological sort." : "A weighted undirected graph. Try BFS, DFS, Prim or Kruskal.");
            return;
        case 8:
            sampleGraph(graphDirected);
            show(graphDirected ? "A directed graph (a DAG). Try a topological sort." : "A weighted undirected graph. Try BFS, DFS, Prim or Kruskal.");
            return;
        case 9: { // random connected graph
            gedges.clear();
            for (int v = 1; v < graphN; v++) {
                int u = std::rand() % v;
                gedges.push_back({ u, v, 1 + std::rand() % 15 });
            }
            int extra = graphN / 2 + 1;
            for (int i = 0; i < extra; i++) {
                int u = std::rand() % graphN, v = std::rand() % graphN;
                if (u == v) continue;
                Graph g(graphN, graphDirected);
                rebuildGraph(g);
                if (g.addEdge(u, v, 1 + std::rand() % 15)) gedges.push_back({ u, v, g.hasEdge(u, v) ? 1 + std::rand() % 15 : 1 });
            }
            if (graphDirected) { // keep it acyclic: only edges from a smaller to a larger vertex
                for (auto& e : gedges) if (e.u > e.v) std::swap(e.u, e.v);
                Graph g(graphN, true);
                std::vector<GEdge> keep;
                for (auto& e : gedges) if (g.addEdge(e.u, e.v, e.w)) keep.push_back(e);
                gedges = keep;
            }
            show("A new random graph.");
            return;
        }
        case 10:
            gedges.clear();
            show("The graph has no edges. Add some with From, To and Weight.");
            return;
        case 11: case 12: {
            int n = graphN + (op == 11 ? 1 : -1);
            if (n < 3 || n > Graph::MAXV) { toast("A graph here has 3 to " + std::to_string(Graph::MAXV) + " vertices."); return; }
            graphN = n;
            std::vector<GEdge> keep;
            for (auto& e : gedges) if (e.u < n && e.v < n) keep.push_back(e);
            gedges = keep;
            vertexAnims.clear();
            show(std::to_string(n) + " vertices.");
            return;
        }
    }
}

void GUIApp::actHash(int op) {
    if (!ht) newHash();
    Recorder::setEnabled(true);
    Recorder::clear();
    int v = 0;
    if (op <= 2 && !readA(v, "key")) return;
    switch (op) {
        case 0: ht->insert(v); break;
        case 1: ht->search(v); break;
        case 2: ht->remove(v); break;
        case 3:
            for (int tries = 0; tries < 20; tries++) {
                int key = 10 + std::rand() % 90;
                if (ht->search(key) == -1) { Recorder::clear(); ht->insert(key); break; }
            }
            break;
        case 4:
            ht->clear();
            ht->showTable("The table is empty.");
            setRest("The table is empty.");
            return;
    }
    captureSession(true);
}

void GUIApp::actApps(int op) {
    Recorder::setEnabled(true);
    Recorder::clear();
    static const char* examples[3][3] = {
        { "{[a+b]*(c-d)}", "(a+b]*c", "((a+b)" },
        { "a+b*(c-d)/e", "(a+b)*c-d/e", "a*b+c*d" },
        { "3 4 + 2 *", "5 1 2 + 4 * + 3 -", "6 2 /" }
    };
    static int exampleIndex = 0;
    if (op == 1) {
        exampleIndex = (exampleIndex + 1) % 3;
        inText.text = examples[appsMode][exampleIndex];
    }
    if (inText.text.empty()) {
        toast("Type an expression first, or press Example.");
        inText.focused = true;
        return;
    }
    if (appsMode == 0) {
        checkBracketsRecorded(inText.text);
    }
    else if (appsMode == 1) {
        bool ok = false;
        infixToPostfixRecorded(inText.text, ok);
    }
    else {
        int result = 0;
        evaluatePostfixRecorded(inText.text, result);
    }
    captureSession(true);
}

void GUIApp::actRecursion(int op) {
    (void)op;
    Recorder::setEnabled(true);
    Recorder::clear();
    int n = 0;
    if (!readA(n, "n")) return;
    if (recMode == 0) {
        if (n > 12) { toast("Choose n from 0 to 12 for this picture."); return; }
        factorialRecorded(n);
    }
    else if (recMode == 1) {
        if (n > 6) { toast("Choose n from 0 to 6: the call tree doubles with every step."); return; }
        fibonacciRecorded(n);
    }
    else {
        if (n < 1 || n > 5) { toast("Choose 1 to 5 disks."); return; }
        hanoiRecorded(n);
    }
    nodeAnims.clear();
    captureSession(true);
}

// ============================================================================
//  Toolbar of every view
// ============================================================================

void GUIApp::toolbarFor(View v) {
    auto valueBox = [&]() { tbLabel("Value"); tbInput(inA, "number", 84); };
    switch (v) {
        case V_ARRAY:
            valueBox();
            tbLabel("Index"); tbInput(inB, "0", 66);
            tbSep();
            if (tbBtn("Append", 2)) act(0);
            if (tbBtn("Insert at index")) act(1);
            if (tbBtn("Remove at index")) act(2);
            tbSep();
            if (tbBtn("Linear search")) act(3);
            if (tbBtn("Binary search")) act(4);
            if (tbBtn("Sort")) act(5);
            tbSep();
            if (tbBtn("Random")) act(6);
            if (tbBtn("Clear")) act(7);
            break;
        case V_LIST_S: case V_LIST_D: case V_LIST_C:
            valueBox();
            tbLabel("Pos"); tbInput(inB, "1", 62);
            tbSep();
            if (tbBtn("Add head", 2)) act(0);
            if (tbBtn("Add tail", 2)) act(1);
            if (tbBtn("Add at pos")) act(2);
            tbSep();
            if (tbBtn("Del head")) act(3);
            if (tbBtn("Del tail")) act(4);
            if (tbBtn("Del value")) act(5);
            tbSep();
            if (tbBtn("Search")) act(6);
            if (v == V_LIST_D && tbBtn("Search from tail")) act(9);
            if (v != V_LIST_C && tbBtn("Reverse")) act(7);
            if (v == V_LIST_S && tbBtn("Sort")) act(8);
            if (tbBtn("Clear")) act(10);
            break;
        case V_STACK_ARR: case V_STACK_LINK:
            valueBox();
            tbSep();
            if (tbBtn("Push", 2)) act(0);
            if (tbBtn("Pop")) act(1);
            if (tbBtn("Clear")) act(2);
            tbSep();
            tbLabel(v == V_STACK_ARR ? "Last in, first out. Push onto a full stack to see an overflow." : "Last in, first out. Nodes are linked, so there is no size limit.");
            break;
        case V_STACK_APPS: {
            const char* names[3] = { "Brackets", "Infix to postfix", "Postfix evaluation" };
            const char* defaults[3] = { "{[a+b]*(c-d)}", "a+b*(c-d)/e", "3 4 + 2 *" };
            for (int i = 0; i < 3; i++) {
                if (tbBtn(names[i], appsMode == i ? 1 : 0) && appsMode != i) { appsMode = i; inText.text = defaults[i]; act(0); }
            }
            tbSep();
            tbInput(inText, "type an expression", 320);
            if (tbBtn("Run", 2)) act(0);
            if (tbBtn("Example")) act(1);
            break;
        }
        case V_RECURSION: {
            const char* names[3] = { "Factorial", "Fibonacci", "Towers of Hanoi" };
            const char* defaults[3] = { "4", "4", "3" };
            for (int i = 0; i < 3; i++) {
                if (tbBtn(names[i], recMode == i ? 1 : 0) && recMode != i) { recMode = i; inA.text = defaults[i]; act(0); }
            }
            tbSep();
            tbLabel(recMode == 2 ? "Disks" : "n");
            tbInput(inA, "n", 66);
            if (tbBtn("Run", 2)) act(0);
            tbLabel(recMode == 0 ? "n from 0 to 12" : (recMode == 1 ? "n from 0 to 6" : "1 to 5 disks"));
            break;
        }
        case V_Q_SIMPLE: case V_Q_CIRC: case V_Q_LINK:
            valueBox();
            tbSep();
            if (tbBtn("Enqueue", 2)) act(0);
            if (tbBtn("Dequeue")) act(1);
            if (tbBtn("Clear")) act(2);
            tbSep();
            tbLabel("First in, first out.");
            break;
        case V_DEQUE:
            valueBox();
            tbSep();
            if (tbBtn("Insert front", 2)) act(0);
            if (tbBtn("Insert rear", 2)) act(1);
            if (tbBtn("Delete front")) act(2);
            if (tbBtn("Delete rear")) act(3);
            if (tbBtn("Clear")) act(4);
            break;
        case V_BST:
            valueBox();
            tbSep();
            if (tbBtn("Insert", 2)) act(0);
            if (tbBtn("Delete")) act(1);
            if (tbBtn("Search")) act(2);
            tbSep();
            if (tbBtn("Preorder")) act(3);
            if (tbBtn("Inorder")) act(4);
            if (tbBtn("Postorder")) act(5);
            if (tbBtn("Level order")) act(6);
            tbSep();
            if (tbBtn("Random")) act(7);
            if (tbBtn("Clear")) act(8);
            break;
        case V_AVL:
            valueBox();
            tbSep();
            if (tbBtn("Insert", 2)) act(0);
            if (tbBtn("Delete")) act(1);
            tbSep();
            tbLabel("Rotation demo");
            if (tbBtn("LL")) act(100);
            if (tbBtn("RR")) act(101);
            if (tbBtn("LR")) act(102);
            if (tbBtn("RL")) act(103);
            tbSep();
            if (tbBtn("Random")) act(2);
            if (tbBtn("Clear")) act(3);
            break;
        case V_HEAP:
            valueBox();
            tbSep();
            if (tbBtn("Insert", 2)) act(0);
            if (tbBtn("Extract")) act(1);
            tbSep();
            if (tbBtn(heapIsMax ? "Max heap" : "Min heap", 1)) act(4);
            tbSep();
            if (tbBtn("Random")) act(2);
            if (tbBtn("Clear")) act(3);
            break;
        case V_GRAPH:
            tbLabel("From"); tbInput(inA, "u", 56);
            tbLabel("To"); tbInput(inB, "v", 56);
            tbLabel("Weight"); tbInput(inC, "1", 56);
            if (tbBtn("Add edge", 2)) act(0);
            if (tbBtn("Remove edge")) act(1);
            tbSep();
            if (tbBtn("BFS")) act(2);
            if (tbBtn("DFS")) act(3);
            if (tbBtn("Prim")) act(4);
            if (tbBtn("Kruskal")) act(5);
            if (tbBtn("Topo sort")) act(6);
            tbSep();
            if (tbBtn(graphDirected ? "Directed" : "Undirected", 1)) act(7);
            if (tbBtn("Sample")) act(8);
            if (tbBtn("Random")) act(9);
            if (tbBtn("Clear")) act(10);
            tbSep();
            tbLabel("Vertices");
            if (tbBtn("-")) act(12);
            Txt(fontBold, std::to_string(graphN), tbX - 2, tbY + 10, 17, TEXT);
            tbX += 24;
            if (tbBtn("+")) act(11);
            break;
        case V_HASH:
            tbLabel("Key"); tbInput(inA, "number", 84);
            tbSep();
            if (tbBtn("Insert", 2)) act(0);
            if (tbBtn("Search")) act(1);
            if (tbBtn("Delete")) act(2);
            tbSep();
            if (tbBtn("Random")) act(3);
            if (tbBtn("Clear")) act(4);
            break;
        case V_COMPLEX:
            tbLabel("Press play to grow n, or drag the slider at the bottom. Each curve is one growth rate (log scale).");
            break;
        default:
            break;
    }
}

// ============================================================================
//  Scripted screenshot (for checking the look without clicking)
// ============================================================================

// act tokens separated by ';':  "op:a:b:c"  runs act(op) with the three number boxes filled,
// "m<n>" sets the mode of the expressions / recursion view, "hm<n>" and "hf<n>" the hash options,
// "t=text" fills the text box.
void GUIApp::shot(const std::string& file, int viewId, int sub, const std::string& actScript, int step) {
    fixedDt = 0.04f;
    sortAlgo = (SortAlgo)std::max(0, std::min(sub, (int)ALGO_COUNT - 1));
    selectView((View)viewId);
    if (view == V_SORT) runSort();
    size_t pos = 0;
    while (pos < actScript.size()) {
        size_t end = actScript.find(';', pos);
        if (end == std::string::npos) end = actScript.size();
        std::string tok = actScript.substr(pos, end - pos);
        pos = end + 1;
        if (tok.empty()) continue;
        if (tok.compare(0, 2, "hm") == 0) { hashMethod = std::atoi(tok.c_str() + 2); newHash(); sessions[V_HASH].built = false; selectView(V_HASH); continue; }
        if (tok.compare(0, 2, "hf") == 0) { hashFunc = std::atoi(tok.c_str() + 2); newHash(); sessions[V_HASH].built = false; selectView(V_HASH); continue; }
        if (tok[0] == 'm') { appsMode = recMode = std::atoi(tok.c_str() + 1); continue; }
        if (tok[0] == 'D') { graphDirected = std::atoi(tok.c_str() + 1) != 0; sampleGraph(graphDirected); sessions[V_GRAPH].built = false; selectView(V_GRAPH); continue; }
        if (tok.compare(0, 2, "t=") == 0) { inText.text = tok.substr(2); continue; }
        // op:a:b:c
        std::vector<std::string> parts;
        size_t p = 0;
        while (true) {
            size_t q = tok.find(':', p);
            parts.push_back(tok.substr(p, q == std::string::npos ? std::string::npos : q - p));
            if (q == std::string::npos) break;
            p = q + 1;
        }
        inA.text = parts.size() > 1 ? parts[1] : "";
        inB.text = parts.size() > 2 ? parts[2] : "";
        inC.text = parts.size() > 3 ? parts[3] : "";
        act(std::atoi(parts[0].c_str()));
    }
    playing = false;
    Session& s = S();
    s.index = std::max(0, std::min(step < 0 ? (int)s.frames.size() + step : step, (int)s.frames.size() - 1));
    for (int i = 0; i < 60; i++) {
        W = GetScreenWidth();
        H = GetScreenHeight();
        dt = fixedDt;
        now = i * 0.04;
        layout();
        BeginDrawing();
        drawBackground();
        drawStage();
        drawCodePanel();
        drawInfoPanel();
        drawPlayback();
        drawTopBar();
        drawToolbar();
        drawToast();
        EndDrawing();
    }
    TakeScreenshot(file.c_str());
}
