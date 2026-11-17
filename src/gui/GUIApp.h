// NodeScape native desktop app (raylib). It only reads the frames that the core classes record,
// so what is animated is exactly what the C++ code did.

#ifndef GUIAPP_H
#define GUIAPP_H

#include "GUIDraw.h"
#include "../core/Recorder.h"
#include "../core/LinkedList.h"
#include "../core/DoublyLinkedList.h"
#include "../core/CircularLinkedList.h"
#include "../core/ArrayStack.h"
#include "../core/LinkedStack.h"
#include "../core/ArrayQueue.h"
#include "../core/CircularQueue.h"
#include "../core/Deque.h"
#include "../core/LinkedQueue.h"
#include "../core/DynamicArray.h"
#include "../core/BST.h"
#include "../core/AVLTree.h"
#include "../core/BinaryHeap.h"
#include "../core/Graph.h"
#include "../core/HashTable.h"
#include "raylib.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// every picture the app can show
enum View {
    V_SORT = 0, V_ARRAY,
    V_LIST_S, V_LIST_D, V_LIST_C,
    V_STACK_ARR, V_STACK_LINK, V_STACK_APPS, V_RECURSION,
    V_Q_SIMPLE, V_Q_CIRC, V_DEQUE, V_Q_LINK,
    V_BST, V_AVL,
    V_HEAP, V_GRAPH, V_HASH, V_COMPLEX,
    V_COUNT
};

// the tabs at the top. A group can hold several views that are chosen in the second row.
enum Group { G_SORT = 0, G_ARRAY, G_LIST, G_STACK, G_QUEUE, G_TREE, G_HEAP, G_GRAPH, G_HASH, G_COMPLEX, G_COUNT };

enum SortAlgo { ALGO_BUBBLE = 0, ALGO_SELECTION, ALGO_INSERTION, ALGO_MERGE, ALGO_QUICK, ALGO_HEAP, ALGO_COUNT };

struct InputBox {
    std::string text;
    bool focused = false;
    int maxLen = 3;
    bool anyText = false;   // false: digits only
};

// everything the player needs for the operation that is on screen
struct Session {
    std::vector<Frame> frames;
    std::vector<std::string> code;
    std::string title;
    int index = 0;
    bool built = false;
};

struct GEdge { int u, v, w; };

struct BarAnim { float h = 0; Tint tint{}; };
struct CellAnim { float x = 0; Tint tint{}; bool init = false; };
struct NodeAnim { float x = 0, y = 0, scale = 0, alpha = 0; Tint tint{}; bool seen = false; int value = 0; };

class GUIApp {
public:
    GUIApp();
    ~GUIApp();
    void run();
    // renders a few frames of a scripted situation, saves a screenshot and returns (used to check the look)
    void shot(const std::string& file, int viewId, int sub, const std::string& act, int step);

private:
    // window and fonts
    int W = 1440, H = 900;
    Font fontUI{}, fontBold{}, fontCode{};
    bool ownFonts = false;

    // what is selected
    View view = V_SORT;
    View lastView[G_COUNT];
    SortAlgo sortAlgo = ALGO_BUBBLE;
    Session sessions[V_COUNT];

    // data that survives between operations
    std::vector<int> array;
    int arraySize = 12;
    std::unique_ptr<DynamicArray> dyn;
    LinkedList list;
    DoublyLinkedList dlist;
    CircularLinkedList clist;
    ArrayStack astack;
    LinkedStack lstack;
    ArrayQueue aqueue;
    CircularQueue cqueue;
    Deque deque;
    LinkedQueue lqueue;
    BST bst;
    AVLTree avl;
    BinaryHeap heap{ true };
    bool heapIsMax = true;
    int graphN = 7;
    bool graphDirected = false;
    std::vector<GEdge> gedges;
    std::unique_ptr<HashTable> ht;
    int hashMethod = 0, hashFunc = 0, hashSize = 11;
    int appsMode = 0, recMode = 0;

    // inputs
    InputBox inA, inB, inC, inText;
    InputBox* focusReq = nullptr;

    // playback
    bool playing = false;
    float speed = 1.0f;
    float stepTimer = 0.0f;
    bool draggingScrub = false;

    // animation state
    std::vector<BarAnim> barAnims;
    std::vector<CellAnim> cellAnims;
    std::unordered_map<int, NodeAnim> nodeAnims;
    std::unordered_map<int, NodeAnim> vertexAnims;
    float codeHighlightY = 0.0f;
    float dt = 0.016f;
    float fixedDt = 0.0f;
    double now = 0.0;
    std::unordered_map<std::string, float> hover;

    // toast message
    std::string toastText;
    float toastTimer = 0.0f;

    // layout
    float headerH = 118;
    Rectangle stageRect{}, panelRect{}, codeRect{}, infoRect{};
    float tbX = 24, tbY = 64, tbH = 40;   // cursor of the toolbar row that is being drawn

    Session& S() { return sessions[view]; }
    const Frame* currentFrame();
    static Group groupOf(View v);
    static const char* groupName(Group g);
    static const char* viewName(View v);
    int rowsFor(View v) const;

    // building sessions
    void buildView(View v);
    void captureSession(bool autoplay);
    void setRest(const std::string& message);
    void emptyPicture(const std::string& message, FrameKind kind);
    void runSort();
    void newArray(int mode);
    void toast(const std::string& text);
    bool readA(int& value, const char* what = "value");
    bool readB(int& value, const char* what);
    void selectView(View v);
    void buildComplexity();
    void rebuildGraph(Graph& g) const;
    void sampleGraph(bool directed);
    void newHash();

    // operations (the toolbar buttons call these)
    void act(int op);
    void actSort(int op);
    void actArray(int op);
    void actList(int op);
    void actStack(int op);
    void actQueue(int op);
    void actBst(int op);
    void actAvl(int op);
    void actHeap(int op);
    void actGraph(int op);
    void actHash(int op);
    void actApps(int op);
    void actRecursion(int op);
    void primaryAction();

    // drawing
    void layout();
    void drawBackground();
    void drawTopBar();
    void drawToolbar();
    void drawStage();
    void drawBars(const Frame& f, Rectangle area);
    void drawBoxes(const Frame& f, Rectangle area);
    void drawList(const Frame& f, Rectangle area);
    void drawTree(const Frame& f, Rectangle area);
    void drawStack(const Frame& f, Rectangle area);
    void drawGraph(const Frame& f, Rectangle area);
    void drawHash(const Frame& f, Rectangle area);
    void drawHanoi(const Frame& f, Rectangle area);
    void drawComplexity(Rectangle area);
    void drawInfoLines(const Frame& f, float x, float y, float maxW);
    void drawLegend(float x, float y);
    void drawCodePanel();
    void drawInfoPanel();
    void drawPlayback();
    void drawToast();

    // toolbar widgets
    bool Button(Rectangle r, const std::string& label, int style = 0);
    bool IconButton(Rectangle r, int icon, bool primary);
    void Input(Rectangle r, InputBox& box, const char* hint);
    bool tbBtn(const std::string& label, int style = 0);
    void tbLabel(const std::string& text);
    void tbSep();
    void tbInput(InputBox& box, const char* hint, float w);
    void toolbarFor(View v);
    void handleTyping();
    void handleKeys();
    void updatePlayback();

    // text helpers
    void Txt(const Font& f, const std::string& s, float x, float y, float size, Color c);
    float TxtW(const Font& f, const std::string& s, float size);
    void TxtC(const Font& f, const std::string& s, float cx, float y, float size, Color c);
};

#endif
