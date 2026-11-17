# Step recorder (phase 2)

Turn it on in the main menu (option 14, replay mode). After an operation in the linked list,
sorting or BST module you can watch it step by step and go backward.

## How it works

An operation calls `Recorder::begin(title, codeLines)` and then adds one `Frame` for every step.
A frame is one picture of the data structure:

| Field | Meaning |
|-------|---------|
| `kind` | list, array, bars or tree |
| `cells` | the values with a role (normal, current, found, new, removed, done, kept) |
| `markers` | pointers like head or t drawn at a cell |
| `floating` | a new node that exists but is not linked yet |
| `nodes`, `rootId` | the tree, every node has an id that never changes (`aux` = balance factor for AVL, array index for a heap) |
| `message` | what happens in this step, in plain English |
| `codeLine` | the line of pseudocode that is running |
| `comparisons`, `swaps` | counters up to this step |

The console replay (`src/ui/ReplayPlayer.cpp`) and the later graphical version only read frames,
they never touch the data structure. That is why the animation is always what really happened.

## Recorded so far

Everything in the console menus has frames now:

- Sorting: bubble, selection, insertion, merge, quick, heap (a compare frame, then a green "condition met" or rose "not met" frame)
- Singly, doubly and circular linked lists: every operation (`doubly` and `circular` flags tell the viewer how to draw the links)
- Array and stack and queue types: array stack, linked stack, simple, circular and linked queue, deque (`KIND_STACK`, `KIND_SLOTS`, empty slots are drawn dashed)
- Dynamic array: append, insert, remove, growth, linear and binary search (the half that is thrown away is `ROLE_DIM`), insertion sort
- BST: insert, search, delete, the four traversals; AVL tree: insert and delete with every rotation (`aux` is the balance factor); heap: insert and extract
- Hash table: all four collision methods and three hash functions (`KIND_HASH`, `info` holds the hash computation, `chains` the chains)
- Graph: BFS, DFS, topological sort, Prim, Kruskal (`KIND_GRAPH`: vertices, edges with roles, a strip of numbers for the queue or call stack)
- Recursion: factorial (call stack), fibonacci (call tree) and towers of hanoi (`KIND_HANOI`, `chains` hold the three pegs)
- Stack applications: bracket matching, infix to postfix, postfix evaluation (`recorded` versions that keep their own stack)

Colors the desktop app uses for a role: current = yellow to orange (being checked), new = green to lime (condition met, swapped, placed),
kept = pink to red (condition not met, nothing changes), found = magenta to violet (pivot, key, minimum), done = aqua to teal,
removed = orange to red, dim = slate (ruled out), normal = cyan to blue.

A helper structure that is used inside an algorithm (the queue inside BFS) would overwrite the recording, so wrap its calls in
`RecordPause pause;` (see `Graph.cpp`).

## Adding frames to a module (for the team)

1. Write the pseudocode lines of the operation as a `vector<string>`.
2. Call `Recorder::begin("Operation name", CODE_LINES)` at the start of the operation.
3. At every interesting step build a `Frame` and call `Recorder::add(frame)`. Check `Recorder::enabled()` first
   when building the frame costs time.
4. Add a test in `tests/test_recorder.cpp`: the last frame must show the final result.
