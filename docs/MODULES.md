# Modules

Every module has a core class in `src/core` (the data structure itself), a console menu in `src/ui`
and tests in `tests/`. Step mode (main menu, option 13) makes the core explain what it does.

| Menu | Module | Core files | What it covers |
|------|--------|------------|----------------|
| 1 | Singly linked list | LinkedList | insert, delete, search, reverse, sort |
| 2 | Doubly and circular lists | DoublyLinkedList, CircularLinkedList | head and tail pointers, forward and backward walk, circular link |
| 3 | Stack, applications, recursion | ArrayStack, LinkedStack, StackApps, Recursion | array and linked stack, bracket check, infix to postfix, postfix evaluation, factorial, fibonacci, hanoi |
| 4 | Queues | ArrayQueue, CircularQueue, Deque, LinkedQueue | simple queue and its weakness, circular queue, deque, linked queue |
| 5 | Sorting | Sorting | bubble, selection, insertion, merge, quick (and heap sort), step counts, best and worst case |
| 6 | Binary tree and BST | BinaryTree, BST | level order insert, four traversals, BST insert, search, delete (three cases) |
| 7 | AVL tree | AVLTree | balance factors, LL, RR, LR, RL rotations, delete with rebalancing |
| 8 | Heap and priority queue | BinaryHeap, PriorityQueue | max and min heap, heapify, heap sort, priority queue |
| 9 | Graph | Graph | adjacency list and matrix, BFS, DFS, components, Prim, Kruskal, topological sort |
| 10 | Hashing | HashTable | division, folding, mid-square; chaining, linear, quadratic, double hashing |
| 11 | Array and searching | DynamicArray | dynamic array, linear search, binary search |
| 12 | Time complexity | Complexity | growth table, real step counts of searches and loops |

## Tests

`run_tests.bat` (Windows) or `make test` builds and runs every file in `tests/`.
Sorting, trees, AVL and heap are checked against `std::sort` or a plain reference on random data.
The AVL tree is validated after every one of 1500 random inserts and deletes.

## Phase 2 (recorder) is described in RECORDER.md

## Phase 3: the desktop app

`build_app.bat` (or `make build/NodeScapeApp.exe`) builds the raylib desktop app from `src/gui`.
`NodeScape-App.bat` starts it (and builds it first if it is missing); `NodeScape.vbs` does the same without a console window,
and `create_desktop_shortcut.bat` puts a shortcut on the desktop.

The app never touches the data structures itself: it only plays the frames the core classes record.
Every module of the console version has a picture:

| Tab | Views |
|-----|-------|
| Sorting | bubble, selection, insertion, merge, quick, heap sort (bars, size 5 to 40) |
| Arrays | dynamic array (growth, insert, remove), linear search, binary search, insertion sort |
| Lists | singly, doubly (backward links) and circular (wrap-around arrow) linked lists |
| Stacks | array stack, linked stack, expressions (bracket matching, infix to postfix, postfix evaluation), recursion (factorial call stack, fibonacci call tree, towers of hanoi) |
| Queues | simple, circular, deque, linked queue |
| Trees | binary search tree (insert, delete, search, four traversals), AVL tree (balance factors, LL RR LR RL demos) |
| Heap | max and min heap, insert and extract, the same values as an array |
| Graph | editable weighted graph, directed or not: BFS, DFS, Prim, Kruskal, topological sort |
| Hashing | chaining, linear, quadratic and double hashing, division, folding and mid-square functions |
| Complexity | growth curves of O(1) to O(2^n) on a log scale, with the real step counts for n |

Testing without clicking: `NodeScapeApp.exe --shot file.png <view> <sub> "<script>" <step>` draws one frame and saves a picture
(see the comment above `GUIApp::shot` in `src/gui/GUIOps.cpp`). The old browser version in `web/` is kept for reference but is no longer the main app.
