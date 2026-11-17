// Tests for the recordings of the other modules: stacks, queues, doubly and circular lists, dynamic array,
// hash table, graph algorithms, recursion and the stack applications.
// Every recording must end in the right final picture and every frame must be well formed.

#include "TestUtils.h"
#include "../src/core/Recorder.h"
#include "../src/core/ArrayStack.h"
#include "../src/core/LinkedStack.h"
#include "../src/core/ArrayQueue.h"
#include "../src/core/CircularQueue.h"
#include "../src/core/Deque.h"
#include "../src/core/LinkedQueue.h"
#include "../src/core/DoublyLinkedList.h"
#include "../src/core/CircularLinkedList.h"
#include "../src/core/DynamicArray.h"
#include "../src/core/HashTable.h"
#include "../src/core/Graph.h"
#include "../src/core/Recursion.h"
#include "../src/core/StackApps.h"
#include <string>
#include <vector>
using namespace std;

static bool framesWellFormed() {
    for (int i = 0; i < Recorder::frameCount(); i++) {
        const Frame& f = Recorder::frame(i);
        if (f.message.empty()) return false;
        if (f.codeLine < -1 || f.codeLine >= (int)Recorder::code().size()) return false;
    }
    return Recorder::frameCount() > 0 && !Recorder::title().empty();
}

static const Frame& last() {
    return Recorder::frame(Recorder::frameCount() - 1);
}

static bool hasRole(Role role) {
    for (int i = 0; i < Recorder::frameCount(); i++) {
        const Frame& f = Recorder::frame(i);
        for (size_t c = 0; c < f.cells.size(); c++) if (f.cells[c].role == role) return true;
    }
    return false;
}

static int countUsed(const Frame& f) {
    int used = 0;
    for (size_t i = 0; i < f.cells.size(); i++) if (!f.cells[i].empty) used++;
    return used;
}

static bool messageContains(const char* text) {
    for (int i = 0; i < Recorder::frameCount(); i++) {
        if (Recorder::frame(i).message.find(text) != string::npos) return true;
    }
    return false;
}

int main() {
    Recorder::setEnabled(true);

    // ---- array stack ----
    {
        ArrayStack st;
        st.push(5);
        st.push(6);
        check(Recorder::title() == "Push (array stack)" && framesWellFormed(), "array stack push: frames are well formed");
        const Frame& f = last();
        check(f.kind == KIND_STACK && (int)f.cells.size() == ArrayStack::MAX && countUsed(f) == 2 && f.cells[1].value == 6 && f.cells[1].role == ROLE_NEW, "array stack push: the new value is on top, free slots are empty");
        int v = 0;
        st.pop(v);
        check(v == 6 && countUsed(last()) == 1 && framesWellFormed(), "array stack pop: the top slot is free again");
        for (int i = 0; i < 10; i++) st.push(i);
        check(last().cells[ArrayStack::MAX - 1].role == ROLE_KEPT && messageContains("full"), "array stack: overflow is explained");
        ArrayStack empty;
        empty.pop(v);
        check(Recorder::frameCount() == 1 && messageContains("empty"), "array stack: underflow gives one explaining frame");
    }

    // ---- linked stack ----
    {
        LinkedStack st;
        st.push(1);
        st.push(2);
        check(framesWellFormed() && last().kind == KIND_STACK && last().cells.size() == 2 && last().cells[1].value == 2 && last().cells[1].role == ROLE_NEW, "linked stack push: new node on top");
        check(Recorder::frame(0).hasFloating && Recorder::frame(0).floating.value == 2, "linked stack push: first frame shows the unlinked node");
        int v = 0;
        st.pop(v);
        check(v == 2 && last().cells.size() == 1 && framesWellFormed(), "linked stack pop");
    }

    // ---- queues ----
    {
        ArrayQueue q;
        for (int i = 1; i <= 4; i++) q.enqueue(i);
        int v = 0;
        q.dequeue(v);
        q.dequeue(v);
        check(framesWellFormed() && last().kind == KIND_SLOTS && (int)last().cells.size() == ArrayQueue::MAX, "simple queue: slots are drawn");
        check(last().cells[1].empty && last().cells[1].role == ROLE_REMOVED, "simple queue: the dequeued slot is wasted");
        for (int i = 0; i < 8; i++) q.enqueue(i);
        check(messageContains("weakness") || messageContains("full"), "simple queue: the full queue is explained");

        CircularQueue c;
        for (int i = 1; i <= 8; i++) c.enqueue(i);
        c.dequeue(v);
        c.dequeue(v);
        c.dequeue(v);
        c.enqueue(99);
        check(framesWellFormed() && last().cells[0].value == 99 && last().cells[0].role == ROLE_NEW, "circular queue: the rear wraps around to slot 0");
        check(messageContains("wrapped"), "circular queue: the wrap around is explained");

        Deque d;
        d.insertRear(2);
        d.insertFront(1);
        check(framesWellFormed() && countUsed(last()) == 2 && last().cells[Deque::MAX - 1].value == 1, "deque: insert at the front wraps to the last slot");
        d.deleteRear(v);
        check(v == 2 && countUsed(last()) == 1, "deque: delete at the rear");

        LinkedQueue l;
        l.enqueue(7);
        l.enqueue(8);
        check(framesWellFormed() && last().kind == KIND_LIST && last().cells.size() == 2 && last().cells[1].role == ROLE_NEW, "linked queue enqueue");
        l.dequeue(v);
        check(v == 7 && last().cells.size() == 1, "linked queue dequeue");
    }

    // ---- doubly and circular lists ----
    {
        DoublyLinkedList d;
        d.insertAtEnd(1);
        d.insertAtEnd(2);
        d.insertAtBeginning(0);
        check(framesWellFormed() && last().doubly && !last().circular && last().cells.size() == 3 && last().cells[0].value == 0, "doubly list: frames carry the doubly flag");
        d.insertAtPosition(2, 9);
        check(framesWellFormed() && last().cells.size() == 4 && last().cells[1].value == 9 && last().cells[1].role == ROLE_NEW, "doubly list: insert at position");
        d.search(2);
        check(last().cells[3].role == ROLE_FOUND, "doubly list: search marks the found node");
        d.searchFromEnd(0);
        check(last().cells[0].role == ROLE_FOUND && messageContains("from the end"), "doubly list: search from the tail");
        d.reverse();
        check(framesWellFormed() && last().cells[0].value == 2 && last().cells[3].value == 0, "doubly list: reverse");
        d.deleteByValue(9);
        check(framesWellFormed() && last().cells.size() == 3, "doubly list: delete by value");
        d.deleteLast();
        d.deleteFirst();
        check(framesWellFormed() && last().cells.size() == 1, "doubly list: delete at both ends");

        CircularLinkedList c;
        c.insertAtEnd(1);
        c.insertAtEnd(2);
        c.insertAtEnd(3);
        check(framesWellFormed() && last().circular && !last().doubly && last().cells.size() == 3, "circular list: frames carry the circular flag");
        c.insertAtBeginning(0);
        check(last().cells[0].value == 0 && last().cells[0].role == ROLE_NEW, "circular list: insert at the beginning");
        c.deleteLast();
        check(framesWellFormed() && last().cells.size() == 3 && last().cells[2].value == 2, "circular list: delete last walks to the node before the tail");
        c.search(7);
        check(messageContains("not in the list"), "circular list: a missed search stops after count nodes");
        c.deleteByValue(1);
        check(framesWellFormed() && last().cells.size() == 2, "circular list: delete by value");
    }

    // ---- dynamic array ----
    {
        DynamicArray a(2);
        a.append(30);
        a.append(10);
        a.append(20);
        check(framesWellFormed() && last().kind == KIND_SLOTS && (int)last().cells.size() == 4 && countUsed(last()) == 3, "dynamic array: capacity doubles when full");
        check(messageContains("grow"), "dynamic array: the growth is explained");
        a.insertAt(1, 5);
        check(framesWellFormed() && last().cells[1].value == 5 && countUsed(last()) == 4, "dynamic array: insert shifts the elements");
        a.removeAt(0);
        check(framesWellFormed() && countUsed(last()) == 3 && last().cells[0].value == 5, "dynamic array: remove closes the gap");
        DynamicArray unsorted(4);
        unsorted.append(3);
        unsorted.append(1);
        unsorted.binarySearch(1);
        check(messageContains("not sorted"), "binary search refuses unsorted data");
        a.sortAscending();
        check(framesWellFormed() && last().cells[0].role == ROLE_DONE, "array: sort ends with every value done");
        a.binarySearch(20);
        check(framesWellFormed() && hasRole(ROLE_DIM) && hasRole(ROLE_FOUND), "binary search: ruled out halves are dimmed and the value is found");
        a.linearSearch(99);
        check(messageContains("not in the array"), "linear search: a miss is explained");
    }

    // ---- hash table ----
    {
        HashTable chain(7, CHAINING, DIVISION);
        chain.insert(10);
        chain.insert(17);
        check(framesWellFormed() && last().kind == KIND_HASH && last().chains.size() == 7 && last().chains[3].size() == 2, "hash chaining: a collision joins the chain");
        check(!last().info.empty() && last().info[0].find("hash(17)") != string::npos, "hash chaining: the hash computation is shown");
        chain.search(17);
        check(framesWellFormed() && last().message.find("Found") != string::npos, "hash chaining: search");
        chain.remove(10);
        check(framesWellFormed() && last().chains[3].size() == 1, "hash chaining: delete");

        HashTable lin(7, LINEAR_PROBING, DIVISION);
        lin.insert(10);
        lin.insert(17);
        check(framesWellFormed() && last().chains.empty() && !last().cells[4].empty && last().cells[4].value == 17, "linear probing: the colliding key moves to the next slot");
        check(messageContains("Collision"), "linear probing: the collision is explained");
        lin.remove(10);
        bool deletedMarker = false;
        for (size_t i = 0; i < last().markers.size(); i++) if (last().markers[i].label == "deleted") deletedMarker = true;
        check(deletedMarker, "linear probing: a deleted slot is marked");
        lin.insert(10);
        check(framesWellFormed() && !last().cells[3].empty && last().cells[3].value == 10, "linear probing: a deleted slot is reused");
    }

    // ---- graph algorithms ----
    {
        Graph g(5, false);
        g.addEdge(0, 1, 4);
        g.addEdge(0, 2, 1);
        g.addEdge(1, 2, 2);
        g.addEdge(1, 3, 5);
        g.addEdge(2, 3, 8);
        g.addEdge(3, 4, 3);
        int order[Graph::MAXV];
        int n = g.bfs(0, order);
        check(n == 5 && framesWellFormed() && last().kind == KIND_GRAPH && last().vertices.size() == 5, "bfs: graph frames");
        bool allDone = true;
        for (size_t i = 0; i < last().vertices.size(); i++) if (last().vertices[i].role != ROLE_DONE) allDone = false;
        check(allDone, "bfs: every vertex is finished in the last frame");
        check(last().edges.size() == 6, "bfs: undirected edges are listed once");
        n = g.dfs(0, order);
        check(n == 5 && framesWellFormed() && last().auxLabel == "Call stack", "dfs: the call stack is shown");

        int eu[Graph::MAXV * Graph::MAXV], ev[Graph::MAXV * Graph::MAXV], ew[Graph::MAXV * Graph::MAXV], ec = 0;
        bool connected = false;
        int total = g.kruskalMST(eu, ev, ew, ec, connected);
        int treeEdges = 0;
        for (size_t i = 0; i < last().edges.size(); i++) if (last().edges[i].role == ROLE_DONE) treeEdges++;
        check(framesWellFormed() && total == 11 && treeEdges == 4, "kruskal: the accepted edges are marked in the last frame");
        total = g.primMST(0, eu, ev, ew, ec, connected);
        treeEdges = 0;
        for (size_t i = 0; i < last().edges.size(); i++) if (last().edges[i].role == ROLE_DONE) treeEdges++;
        check(framesWellFormed() && total == 11 && treeEdges == 4, "prim: the tree edges are marked in the last frame");

        Graph dag(4, true);
        dag.addEdge(0, 1, 1);
        dag.addEdge(0, 2, 1);
        dag.addEdge(1, 3, 1);
        dag.addEdge(2, 3, 1);
        int count = 0;
        check(dag.topologicalSort(order, count) && framesWellFormed() && last().directed && last().cells.size() == 0, "topological sort: directed frames");
        Graph cyc(3, true);
        cyc.addEdge(0, 1, 1);
        cyc.addEdge(1, 2, 1);
        cyc.addEdge(2, 0, 1);
        check(!cyc.topologicalSort(order, count) && messageContains("cycle"), "topological sort: a cycle is explained");

        g.showGraph("calm picture");
        check(Recorder::frameCount() == 1 && last().vertices.size() == 5, "showGraph records one frame");
    }

    // ---- recursion ----
    {
        check(factorialRecorded(5) == 120 && framesWellFormed(), "factorial: result and frames");
        bool peaked = false;
        for (int i = 0; i < Recorder::frameCount(); i++) if (Recorder::frame(i).cells.size() == 5) peaked = true;
        check(peaked && last().cells.empty(), "factorial: the call stack grows to 5 calls and ends empty");
        check(fibonacciRecorded(4) == 3 && framesWellFormed(), "fibonacci: result");
        check(last().comparisons == 9 && last().nodes.size() == 9, "fibonacci: the call tree has 9 calls");
        check(last().nodes[0].hasAux && last().nodes[0].aux == 3, "fibonacci: the root shows its result");
        check(hanoiRecorded(3) == 7 && framesWellFormed(), "hanoi: 7 moves");
        check(last().kind == KIND_HANOI && last().chains.size() == 3 && last().chains[2].size() == 3 && last().chains[0].empty(), "hanoi: all disks end on peg C");
        bool ordered = true;
        for (size_t i = 1; i < last().chains[2].size(); i++) if (last().chains[2][i].value > last().chains[2][i - 1].value) ordered = false;
        check(ordered, "hanoi: the biggest disk is at the bottom");
    }

    // ---- stack applications ----
    {
        check(checkBracketsRecorded("{[a+b]*(c-d)}") && framesWellFormed(), "brackets: balanced");
        check(!checkBracketsRecorded("(a+b]*c") && messageContains("does not match"), "brackets: a mismatch is explained");
        check(!checkBracketsRecorded("((a+b)") && messageContains("never closed"), "brackets: an unclosed bracket is explained");
        bool ok = false;
        string post = infixToPostfixRecorded("a+b*(c-d)/e", ok);
        bool framesOk = framesWellFormed();
        check(last().info.size() == 3 && last().info[2].find("Output:") != string::npos, "infix to postfix: the output is shown");
        Recorder::setEnabled(false); // the plain function uses a linked stack, keep its frames out of the way
        bool ok2 = false;
        check(ok && framesOk && post == infixToPostfix("a+b*(c-d)/e", ok2), "infix to postfix: same result as the plain function");
        Recorder::setEnabled(true);
        int result = 0;
        check(evaluatePostfixRecorded("5 1 2 + 4 * + 3 -", result) && result == 14 && framesWellFormed(), "postfix evaluation: 5 1 2 + 4 * + 3 - = 14");
        check(last().cells.size() == 1 && last().cells[0].value == 14, "postfix evaluation: the result is the only value on the stack");
        check(!evaluatePostfixRecorded("1 +", result), "postfix evaluation: too few operands");
    }

    Recorder::setEnabled(false);
    return finishTests();
}
