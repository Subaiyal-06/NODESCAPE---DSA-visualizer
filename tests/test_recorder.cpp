// Tests for the step recorder: linked list, the sorting algorithms, BST, heap and AVL tree.
// A recording must end in the right final picture and every frame must be well formed.

#include "TestUtils.h"
#include "../src/core/Recorder.h"
#include "../src/core/LinkedList.h"
#include "../src/core/Sorting.h"
#include "../src/core/BST.h"
#include "../src/core/BinaryHeap.h"
#include "../src/core/AVLTree.h"
#include <string>
#include <vector>
using namespace std;

// every frame must have a message and a code line that exists
static bool framesWellFormed() {
    for (int i = 0; i < Recorder::frameCount(); i++) {
        const Frame& f = Recorder::frame(i);
        if (f.message.empty()) return false;
        if (f.codeLine < -1 || f.codeLine >= (int)Recorder::code().size()) return false;
    }
    return Recorder::frameCount() > 0 && !Recorder::title().empty();
}

static bool cellsAre(const Frame& f, const vector<int>& values) {
    if (f.cells.size() != values.size()) return false;
    for (size_t i = 0; i < values.size(); i++) {
        if (f.cells[i].value != values[i]) return false;
    }
    return true;
}

static const TreeNodeInfo* nodeWithValue(const Frame& f, int value) {
    for (size_t i = 0; i < f.nodes.size(); i++) {
        if (f.nodes[i].value == value) return &f.nodes[i];
    }
    return nullptr;
}

int main() {
    // recording is off by default and stays silent
    {
        LinkedList list;
        list.insertAtEnd(5);
        check(!Recorder::enabled() && Recorder::frameCount() == 0, "nothing is recorded while recording is off");
    }

    Recorder::setEnabled(true);

    // linked list
    {
        LinkedList list;
        list.insertAtEnd(1); list.insertAtEnd(2); list.insertAtEnd(3);
        list.insertAtEnd(4);
        check(Recorder::title() == "Insert at end" && framesWellFormed(), "insert at end: frames are well formed");
        const Frame& last = Recorder::frame(Recorder::frameCount() - 1);
        check(cellsAre(last, {1, 2, 3, 4}) && last.cells[3].role == ROLE_NEW, "insert at end: last frame shows the new node");
        check(!Recorder::frame(0).cells.empty() && Recorder::frame(0).hasFloating && Recorder::frame(0).floating.value == 4, "insert at end: first frame shows the unlinked node");
        check(Recorder::frameCount() >= 5, "insert at end: one frame per step of the walk");

        list.insertAtBeginning(0);
        check(Recorder::title() == "Insert at beginning" && cellsAre(Recorder::frame(Recorder::frameCount() - 1), {0, 1, 2, 3, 4}), "a new operation starts a new recording");

        list.insertAtPosition(3, 9);
        check(framesWellFormed() && cellsAre(Recorder::frame(Recorder::frameCount() - 1), {0, 1, 9, 2, 3, 4}), "insert at position");

        check(list.search(3) == 5, "search result is unchanged by recording");
        const Frame& found = Recorder::frame(Recorder::frameCount() - 1);
        check(found.cells[4].role == ROLE_FOUND && found.cells[4].value == 3, "search: the found node is marked");
        list.search(77);
        check(Recorder::frame(Recorder::frameCount() - 1).message.find("not in the list") != string::npos, "search: a miss is explained");

        check(list.deleteByValue(9), "delete by value still works");
        check(framesWellFormed() && cellsAre(Recorder::frame(Recorder::frameCount() - 1), {0, 1, 2, 3, 4}), "delete by value: last frame has no deleted node");
        check(list.deleteAtPosition(1) && cellsAre(Recorder::frame(Recorder::frameCount() - 1), {1, 2, 3, 4}), "delete at position");
        check(list.deleteLast() && cellsAre(Recorder::frame(Recorder::frameCount() - 1), {1, 2, 3}), "delete last");
        check(list.deleteFirst() && cellsAre(Recorder::frame(Recorder::frameCount() - 1), {2, 3}), "delete first");
        list.reverse();
        check(framesWellFormed() && cellsAre(Recorder::frame(Recorder::frameCount() - 1), {3, 2}), "reverse");
        list.insertAtEnd(1);
        list.sortAscending();
        check(framesWellFormed() && cellsAre(Recorder::frame(Recorder::frameCount() - 1), {1, 2, 3}), "sort");
        check(Recorder::frame(Recorder::frameCount() - 1).cells[0].role == ROLE_DONE, "sort: the final frame marks the list as done");

        LinkedList empty;
        check(!empty.deleteFirst() && Recorder::frameCount() == 1 && Recorder::frame(0).message.find("empty") != string::npos, "delete on an empty list gives one explaining frame");
        check(!empty.deleteLast() && !empty.deleteByValue(1) && !empty.deleteAtPosition(1), "other deletes on an empty list");
        check(!list.insertAtPosition(9, 1) && Recorder::frameCount() == 1, "bad position gives one explaining frame");
    }

    // sorting
    {
        const char* names[] = {"Bubble sort", "Selection sort", "Insertion sort", "Merge sort", "Quick sort"};
        void (*sorters[])(int[], int, SortStats&) = {bubbleSort, selectionSort, insertionSort, mergeSort, quickSort};
        for (int s = 0; s < 5; s++) {
            int a[] = {5, 2, 9, 1, 7, 3};
            SortStats stats;
            sorters[s](a, 6, stats);
            string name = names[s];
            check(Recorder::title() == name && framesWellFormed(), (name + ": frames are well formed").c_str());

            const Frame& last = Recorder::frame(Recorder::frameCount() - 1);
            bool allDone = cellsAre(last, {1, 2, 3, 5, 7, 9});
            for (size_t i = 0; i < last.cells.size(); i++) {
                if (last.cells[i].role != ROLE_DONE) allDone = false;
            }
            check(allDone, (name + ": last frame is the sorted array, all done").c_str());
            check(last.comparisons == stats.comparisons && last.swaps == stats.swaps, (name + ": counters in the last frame match").c_str());

            bool sameSize = true;
            for (int i = 0; i < Recorder::frameCount(); i++) {
                if (Recorder::frame(i).cells.size() != 6) sameSize = false;
                if (Recorder::frame(i).kind != KIND_BARS) sameSize = false;
            }
            check(sameSize && Recorder::frameCount() > 2, (name + ": every frame shows all six bars").c_str());
        }
        int sorted[] = {1, 2, 3};
        SortStats stats;
        bubbleSort(sorted, 3, stats);
        check(framesWellFormed(), "sorting an already sorted array still records");
        int one[] = {4};
        mergeSort(one, 1, stats);
        check(framesWellFormed() && Recorder::frame(Recorder::frameCount() - 1).cells[0].role == ROLE_DONE, "one element");
    }

    // BST
    {
        BST tree;
        tree.insert(50);
        check(Recorder::title() == "Insert into the BST" && framesWellFormed(), "BST insert: frames are well formed");
        int rootId = Recorder::frame(Recorder::frameCount() - 1).rootId;
        check(Recorder::frame(Recorder::frameCount() - 1).nodes.size() == 1 && Recorder::frame(Recorder::frameCount() - 1).nodes[0].role == ROLE_NEW, "BST insert into an empty tree");

        tree.insert(30); tree.insert(70); tree.insert(20); tree.insert(40);
        const Frame& f = Recorder::frame(Recorder::frameCount() - 1);
        check(f.nodes.size() == 5 && f.rootId == rootId, "node ids stay the same between operations");
        const TreeNodeInfo* added = nodeWithValue(f, 40);
        check(added != nullptr && added->role == ROLE_NEW, "BST insert: the new node is marked");
        check(Recorder::frameCount() >= 3 && Recorder::frame(0).comparisons == 1, "BST insert: one frame per step of the path");

        check(!tree.insert(40) && nodeWithValue(Recorder::frame(Recorder::frameCount() - 1), 40)->role == ROLE_FOUND, "BST insert of a duplicate marks the existing node");

        check(tree.search(40), "search result is unchanged by recording");
        check(nodeWithValue(Recorder::frame(Recorder::frameCount() - 1), 40)->role == ROLE_FOUND, "BST search: found node is marked");
        tree.search(99);
        check(Recorder::frame(Recorder::frameCount() - 1).message.find("not in the tree") != string::npos, "BST search: a miss is explained");

        const TreeNodeInfo* thirty = nodeWithValue(Recorder::frame(0), 30);
        int thirtyId = thirty != nullptr ? thirty->id : -1;
        check(tree.remove(30), "BST remove still works");
        check(framesWellFormed(), "BST remove: frames are well formed");
        bool twoChildrenExplained = false;
        for (int i = 0; i < Recorder::frameCount(); i++) {
            if (Recorder::frame(i).message.find("two children") != string::npos) twoChildrenExplained = true;
        }
        check(twoChildrenExplained, "BST remove: the two children case is explained");
        const Frame& afterRemove = Recorder::frame(Recorder::frameCount() - 1);
        check(afterRemove.nodes.size() == 4 && nodeWithValue(afterRemove, 30) == nullptr && nodeWithValue(afterRemove, 40) != nullptr, "BST remove: the value is gone");
        bool idKept = false;
        for (size_t i = 0; i < afterRemove.nodes.size(); i++) {
            if (afterRemove.nodes[i].id == thirtyId) idKept = true;
        }
        check(idKept, "BST remove: the node that took over keeps its id");

        int out[20];
        int n = tree.inorder(out, 20);
        check(framesWellFormed() && Recorder::title() == "Inorder traversal", "inorder: frames are well formed");
        vector<int> visited;
        for (int i = 0; i < Recorder::frameCount(); i++) {
            const string& m = Recorder::frame(i).message;
            if (m.compare(0, 6, "Visit ") == 0) visited.push_back(stoi(m.substr(6)));
        }
        bool sameOrder = ((int)visited.size() == n);
        for (int i = 0; i < n && sameOrder; i++) {
            if (visited[i] != out[i]) sameOrder = false;
        }
        check(sameOrder, "inorder: the visit frames follow the real traversal order");

        tree.levelOrder(out, 20);
        int visits = 0;
        for (int i = 0; i < Recorder::frameCount(); i++) {
            if (Recorder::frame(i).message.compare(0, 6, "Visit ") == 0) visits++;
        }
        check(visits == 4, "level order: one frame per node");

        BST empty;
        check(!empty.remove(1) && Recorder::frameCount() == 1, "remove from an empty tree");
    }

    // not-met comparisons are marked, heap sort, heap and AVL tree
    {
        int mixed[3] = {3, 1, 2};
        SortStats s0;
        bubbleSort(mixed, 3, s0);
        bool sawKept = false, sawNew = false, sawCurrent = false;
        for (int i = 0; i < Recorder::frameCount(); i++) {
            for (size_t c = 0; c < Recorder::frame(i).cells.size(); c++) {
                Role r = Recorder::frame(i).cells[c].role;
                if (r == ROLE_KEPT) sawKept = true;
                if (r == ROLE_NEW) sawNew = true;
                if (r == ROLE_CURRENT) sawCurrent = true;
            }
        }
        check(sawKept && sawNew && sawCurrent, "bubble sort uses checking, condition met and condition not met colors");

        int a[8] = {5, 2, 9, 1, 7, 3, 8, 4};
        SortStats stats;
        heapSort(a, 8, stats);
        check(Recorder::title() == "Heap sort" && framesWellFormed(), "heap sort: frames are well formed");
        const Frame& lastBars = Recorder::frame(Recorder::frameCount() - 1);
        bool allDone = cellsAre(lastBars, {1, 2, 3, 4, 5, 7, 8, 9});
        for (size_t i = 0; i < lastBars.cells.size(); i++) {
            if (lastBars.cells[i].role != ROLE_DONE) allDone = false;
        }
        check(allDone, "heap sort: last frame is the sorted array, all done");
        check(lastBars.comparisons == stats.comparisons && lastBars.swaps == stats.swaps, "heap sort: counters match");

        BinaryHeap heap(true);
        heap.insert(5);
        heap.insert(3);
        heap.insert(8);
        heap.insert(10);
        check(Recorder::title() == "Insert into the max heap" && framesWellFormed(), "heap insert: frames are well formed");
        const Frame& heapFrame = Recorder::frame(Recorder::frameCount() - 1);
        const TreeNodeInfo* top = nodeWithValue(heapFrame, 10);
        check(heapFrame.nodes.size() == 4 && top != nullptr && top->id == 0 && heapFrame.rootId == 0, "heap insert: the new maximum ends at the root");
        int removed = 0;
        heap.extractTop(removed);
        const Frame& afterExtract = Recorder::frame(Recorder::frameCount() - 1);
        check(removed == 10 && afterExtract.nodes.size() == 3 && nodeWithValue(afterExtract, 8) != nullptr && nodeWithValue(afterExtract, 8)->id == 0, "heap extract: the next largest value is the root");

        AVLTree avl;
        avl.insert(30);
        avl.insert(20);
        avl.insert(10);
        check(Recorder::title() == "Insert into the AVL tree" && framesWellFormed(), "AVL insert: frames are well formed");
        bool rotated = false;
        for (int i = 0; i < Recorder::frameCount(); i++) {
            if (Recorder::frame(i).message.find("Rotate right") != string::npos) rotated = true;
        }
        const Frame& avlLast = Recorder::frame(Recorder::frameCount() - 1);
        bool balanced = avlLast.nodes.size() == 3;
        for (size_t i = 0; i < avlLast.nodes.size(); i++) {
            if (avlLast.nodes[i].aux < -1 || avlLast.nodes[i].aux > 1) balanced = false;
        }
        const Frame* firstRoot = &avlLast;
        check(rotated && balanced && firstRoot->rootId >= 0, "AVL insert: the left-left case is rotated and every balance factor is -1, 0 or 1");
        const TreeNodeInfo* newRoot = nodeWithValue(avlLast, 20);
        check(newRoot != nullptr && newRoot->id == avlLast.rootId, "AVL insert: the middle value becomes the root");

        AVLTree lr;
        lr.insert(30);
        lr.insert(10);
        lr.insert(20);
        bool leftRight = false;
        for (int i = 0; i < Recorder::frameCount(); i++) {
            if (Recorder::frame(i).message.find("Left-Right") != string::npos) leftRight = true;
        }
        check(leftRight && nodeWithValue(Recorder::frame(Recorder::frameCount() - 1), 20)->id == Recorder::frame(Recorder::frameCount() - 1).rootId, "AVL insert: the left-right case is explained and fixed");

        AVLTree del;
        del.insert(20);
        del.insert(10);
        del.insert(30);
        del.insert(40);
        del.remove(10);
        const Frame& afterDelete = Recorder::frame(Recorder::frameCount() - 1);
        check(Recorder::title() == "Delete from the AVL tree" && framesWellFormed() && afterDelete.nodes.size() == 3 && nodeWithValue(afterDelete, 10) == nullptr, "AVL delete: the value is gone and the recording is well formed");
        bool idsUnique = true;
        for (size_t i = 0; i < afterDelete.nodes.size(); i++) {
            for (size_t j = i + 1; j < afterDelete.nodes.size(); j++) {
                if (afterDelete.nodes[i].id == afterDelete.nodes[j].id) idsUnique = false;
            }
        }
        check(idsUnique && del.isValidAVL(), "AVL delete: node ids are unique and the tree is still a valid AVL tree");
    }

    // switching recording off clears everything
    Recorder::setEnabled(false);
    check(Recorder::frameCount() == 0 && !Recorder::enabled(), "switching off clears the frames");
    LinkedList quiet;
    quiet.insertAtEnd(1);
    check(Recorder::frameCount() == 0, "no frames after switching off");

    return finishTests();
}
