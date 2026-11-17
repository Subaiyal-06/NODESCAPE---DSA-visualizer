// Step recorder (phase 2).
// When recording is switched on, an operation stores a list of "frames". A frame is one picture of
// the data structure plus a message and the line of code that is running. The console replay
// (and later the graphical version) shows the frames one by one, forward and backward.

#ifndef RECORDER_H
#define RECORDER_H

#include <string>
#include <vector>

// How a cell or a tree node should be shown
enum Role {
    ROLE_NORMAL,
    ROLE_CURRENT,  // being looked at right now
    ROLE_FOUND,    // the value we were looking for
    ROLE_NEW,      // just inserted or just moved
    ROLE_REMOVED,  // about to be deleted
    ROLE_DONE,     // finished (for example sorted)
    ROLE_KEPT,     // checked and the condition was NOT met (no swap, kept in place)
    ROLE_DIM       // ruled out (for example the half that binary search throws away)
};

enum FrameKind {
    KIND_LIST,     // linked list: boxes with arrows
    KIND_ARRAY,    // boxes side by side
    KIND_BARS,     // bars for sorting
    KIND_TREE,     // tree
    KIND_STACK,    // vertical stack, cells go from the bottom to the top
    KIND_SLOTS,    // fixed slots side by side (array, queue, deque), empty slots are drawn too
    KIND_GRAPH,    // vertices and edges
    KIND_HASH,     // hash table: slots, plus chains when chaining is used
    KIND_HANOI     // three pegs, chains[0..2] hold the disks (their sizes) from bottom to top
};

struct Cell {
    int value;
    Role role;
    bool empty;     // an unused slot: drawn as an empty box (value is ignored)

    Cell() : value(0), role(ROLE_NORMAL), empty(false) {}
    Cell(int v, Role r) : value(v), role(r), empty(false) {}
    static Cell Empty(Role r = ROLE_NORMAL) { Cell c; c.role = r; c.empty = true; return c; }
};

// A pointer or label drawn at a cell, for example "head" or "t"
struct Marker {
    int index;
    std::string label;

    Marker() : index(0) {}
    Marker(int i, const std::string& text) : index(i), label(text) {}
};

// One node of a tree. id stays the same for the whole life of the node, so a viewer can follow it.
struct TreeNodeInfo {
    int id;
    int value;
    int left;   // id of the left child, -1 if none
    int right;  // id of the right child, -1 if none
    Role role;
    int aux;    // extra number shown beside the node (for example the AVL balance factor), 0 for none
    bool hasAux;

    TreeNodeInfo() : id(-1), value(0), left(-1), right(-1), role(ROLE_NORMAL), aux(0), hasAux(false) {}
};

struct GraphVertex {
    int id;
    Role role;
    GraphVertex() : id(0), role(ROLE_NORMAL) {}
    GraphVertex(int i, Role r) : id(i), role(r) {}
};

struct GraphEdge {
    int u;
    int v;
    int weight;
    Role role;
    GraphEdge() : u(0), v(0), weight(1), role(ROLE_NORMAL) {}
    GraphEdge(int a, int b, int w, Role r) : u(a), v(b), weight(w), role(r) {}
};

struct Frame {
    FrameKind kind;
    std::string message;            // what is happening in this step
    int codeLine;                   // line of the pseudocode that is running, -1 for none

    std::vector<Cell> cells;        // for lists, arrays and bars
    std::vector<Marker> markers;
    bool hasFloating;               // a new node that exists but is not linked into the list yet
    Cell floating;

    std::vector<TreeNodeInfo> nodes; // for trees
    int rootId;

    bool doubly;                    // lists: draw the backward links too
    bool circular;                  // lists: the last node points back to the first

    std::vector<std::string> info;  // extra text lines (for example "Output: 3 4 +")
    std::vector<std::vector<Cell>> chains; // hash chains, or the three pegs of the towers of hanoi
    std::string auxLabel;           // title of the strip of cells under a graph ("Queue", "Order", ...)
    bool directed;                  // graph
    std::vector<GraphVertex> vertices;
    std::vector<GraphEdge> edges;

    long comparisons;
    long swaps;

    Frame() : kind(KIND_LIST), codeLine(-1), hasFloating(false), rootId(-1), doubly(false), circular(false),
              directed(false), comparisons(0), swaps(0) {}
};

class Recorder {
public:
    static void setEnabled(bool on);
    static bool enabled();      // false while recording is switched off or paused
    static void pause();        // nested pauses are counted; used while a helper structure runs inside an algorithm
    static void resume();

    // starts a new recording (the old frames are dropped). Does nothing when recording is off.
    static void begin(const std::string& title, const std::vector<std::string>& code);
    static void add(const Frame& frame);
    static void clear();

    static int frameCount();
    static const Frame& frame(int index);
    static const std::vector<std::string>& code();
    static const std::string& title();
    static bool truncated();   // true when too many frames were produced and the rest was dropped
    static bool exportJSON(const std::string& filename);
};

// stops the recorder for the lifetime of the object (for example around a queue used inside BFS)
class RecordPause {
public:
    RecordPause() { Recorder::pause(); }
    ~RecordPause() { Recorder::resume(); }
};

#endif
