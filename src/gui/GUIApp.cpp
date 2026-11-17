#include "GUIApp.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>

using namespace gfx;

// ============================================================================
//  Groups and views
// ============================================================================

static const View GROUP_VIEWS[G_COUNT][4] = {
    { V_SORT }, { V_ARRAY }, { V_LIST_S, V_LIST_D, V_LIST_C }, { V_STACK_ARR, V_STACK_LINK, V_STACK_APPS, V_RECURSION },
    { V_Q_SIMPLE, V_Q_CIRC, V_DEQUE, V_Q_LINK }, { V_BST, V_AVL }, { V_HEAP }, { V_GRAPH }, { V_HASH }, { V_COMPLEX }
};
static const int GROUP_COUNT[G_COUNT] = { 1, 1, 3, 4, 4, 2, 1, 1, 1, 1 };

Group GUIApp::groupOf(View v) {
    for (int g = 0; g < G_COUNT; g++)
        for (int i = 0; i < GROUP_COUNT[g]; i++)
            if (GROUP_VIEWS[g][i] == v) return (Group)g;
    return G_SORT;
}

const char* GUIApp::groupName(Group g) {
    static const char* names[G_COUNT] = { "Sorting", "Arrays", "Lists", "Stacks", "Queues", "Trees", "Heap", "Graph", "Hashing", "Complexity" };
    return names[g];
}

const char* GUIApp::viewName(View v) {
    static const char* names[V_COUNT] = {
        "Sorting", "Array and searching", "Singly linked list", "Doubly linked list", "Circular linked list",
        "Array stack", "Linked stack", "Stack at work", "Recursion", "Simple queue", "Circular queue", "Deque", "Linked queue",
        "Binary search tree", "AVL tree", "Heap", "Graph", "Hash table", "Time complexity"
    };
    return names[v];
}

int GUIApp::rowsFor(View v) const {
    Group g = groupOf(v);
    if (g == G_LIST || g == G_STACK || g == G_QUEUE || g == G_TREE || g == G_HASH) return 2;
    return 1;
}

// ============================================================================
//  Complexity facts shown in the side panel
// ============================================================================

struct Info { const char* avg; const char* best; const char* worst; const char* space; const char* note; };

static bool has(const std::string& s, const char* part) { return s.find(part) != std::string::npos; }

static Info infoFor(View v, const std::string& title) {
    switch (v) {
        case V_SORT:
            if (has(title, "Bubble"))    return { "O(n^2)", "O(n)", "O(n^2)", "O(1)", "Stable. Stops early when a pass makes no swap." };
            if (has(title, "Selection")) return { "O(n^2)", "O(n^2)", "O(n^2)", "O(1)", "Not stable. At most n - 1 swaps." };
            if (has(title, "Insertion")) return { "O(n^2)", "O(n)", "O(n^2)", "O(1)", "Stable. Fast on nearly sorted data." };
            if (has(title, "Merge"))     return { "O(n log n)", "O(n log n)", "O(n log n)", "O(n)", "Stable. Needs a temporary array." };
            if (has(title, "Quick"))     return { "O(n log n)", "O(n log n)", "O(n^2)", "O(log n)", "Not stable. Worst case on sorted input." };
            return { "O(n log n)", "O(n log n)", "O(n log n)", "O(1)", "Not stable. Sorts inside the array." };
        case V_ARRAY:
            if (has(title, "Binary"))  return { "O(log n)", "O(1)", "O(log n)", "O(1)", "Needs sorted data. Every step throws half away." };
            if (has(title, "Linear"))  return { "O(n)", "O(1)", "O(n)", "O(1)", "Works on any data, looks at one slot at a time." };
            if (has(title, "Append"))  return { "O(1)", "O(1)", "O(n)", "O(n)", "Amortized O(1): growing (copying) is rare." };
            if (has(title, "Insertion")) return { "O(n^2)", "O(n)", "O(n^2)", "O(1)", "Sorting makes binary search possible." };
            return { "O(n)", "O(1)", "O(n)", "O(1)", "Elements after the index must be shifted." };
        case V_LIST_S:
            if (has(title, "beginning") || has(title, "first")) return { "O(1)", "O(1)", "O(1)", "O(1)", "Only the head pointer changes." };
            return { "O(n)", "O(1)", "O(n)", "O(1)", "Singly linked: walk from the head." };
        case V_LIST_D:
            if (has(title, "beginning") || has(title, "first") || has(title, "end") || has(title, "last") || has(title, "tail"))
                return { "O(1)", "O(1)", "O(1)", "O(1)", "Head and tail pointers reach both ends at once." };
            return { "O(n)", "O(1)", "O(n)", "O(1)", "prev links allow walking backwards." };
        case V_LIST_C:
            if (has(title, "beginning") || has(title, "first") || has(title, "end")) return { "O(1)", "O(1)", "O(1)", "O(1)", "The tail pointer reaches both ends." };
            return { "O(n)", "O(1)", "O(n)", "O(1)", "No NULL at the end: stop after count nodes." };
        case V_STACK_ARR: return { "O(1)", "O(1)", "O(1)", "O(n)", "Fixed size: pushing onto a full stack overflows." };
        case V_STACK_LINK: return { "O(1)", "O(1)", "O(1)", "O(n)", "No size limit, one extra pointer per node." };
        case V_STACK_APPS: return { "O(n)", "O(n)", "O(n)", "O(n)", "One pass over the expression, the stack holds the rest." };
        case V_RECURSION:
            if (has(title, "Factorial")) return { "O(n)", "O(n)", "O(n)", "O(n)", "n nested calls wait on the call stack." };
            if (has(title, "Fibonacci")) return { "O(2^n)", "O(1)", "O(2^n)", "O(n)", "The same values are computed again and again." };
            return { "O(2^n)", "O(2^n)", "O(2^n)", "O(n)", "n disks need 2^n - 1 moves." };
        case V_Q_SIMPLE: return { "O(1)", "O(1)", "O(1)", "O(n)", "Freed slots in front are never reused." };
        case V_Q_CIRC: return { "O(1)", "O(1)", "O(1)", "O(n)", "Indexes wrap around with % MAX, no waste." };
        case V_DEQUE: return { "O(1)", "O(1)", "O(1)", "O(n)", "Insert and delete at both ends." };
        case V_Q_LINK: return { "O(1)", "O(1)", "O(1)", "O(n)", "Head is the front, tail is the rear." };
        case V_BST:
            if (has(title, "traversal")) return { "O(n)", "O(n)", "O(n)", "O(h)", "Every node is visited once." };
            return { "O(log n)", "O(1)", "O(n)", "O(h)", "Degenerates to a list on sorted input." };
        case V_AVL: return { "O(log n)", "O(log n)", "O(log n)", "O(log n)", "Rotations keep the height logarithmic." };
        case V_HEAP: return { "O(log n)", "O(1)", "O(log n)", "O(1)", "Stored in an array: children of i are 2i+1, 2i+2." };
        case V_GRAPH:
            if (has(title, "Prim"))    return { "O(V^2)", "O(V^2)", "O(V^2)", "O(V)", "Greedy: always the cheapest edge leaving the tree." };
            if (has(title, "Kruskal")) return { "O(E log E)", "O(E log E)", "O(E log E)", "O(V)", "Greedy: cheapest edge that makes no cycle." };
            return { "O(V + E)", "O(V + E)", "O(V + E)", "O(V)", "Every vertex and edge is looked at once." };
        case V_HASH: return { "O(1)", "O(1)", "O(n)", "O(n)", "A good hash spreads keys; many collisions slow it down." };
        default: return { "-", "-", "-", "-", "" };
    }
}

// ============================================================================
//  Setup
// ============================================================================

static Font loadFont(const char* path, int size, bool& ok) {
    if (!FileExists(path)) { ok = false; return GetFontDefault(); }
    std::vector<int> cps;
    for (int c = 32; c < 127; c++) cps.push_back(c);
    int extra[] = { 0xB7, 0x2022, 0x2013, 0x2014, 0x2026, 0x2190, 0x2192, 0x2264, 0x2265, 0xB2 };
    for (int c : extra) cps.push_back(c);
    Font f = LoadFontEx(path, size, cps.data(), (int)cps.size());
    if (f.texture.id == 0) { ok = false; return GetFontDefault(); }
    SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
    return f;
}

GUIApp::GUIApp() {
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(W, H, "NodeScape");
    SetWindowMinSize(1240, 740);
    SetTargetFPS(60);
    SetExitKey(0);

    // window icon: three glowing bars
    Image icon = GenImageColor(64, 64, Color{ 11, 13, 18, 255 });
    ImageDrawRectangle(&icon, 10, 30, 12, 26, Color{ 96, 150, 255, 255 });
    ImageDrawRectangle(&icon, 26, 10, 12, 46, Color{ 74, 226, 164, 255 });
    ImageDrawRectangle(&icon, 42, 22, 12, 34, Color{ 255, 205, 86, 255 });
    SetWindowIcon(icon);
    UnloadImage(icon);

    bool ok = true;
    fontUI = loadFont("C:/Windows/Fonts/segoeui.ttf", 32, ok);
    fontBold = loadFont("C:/Windows/Fonts/seguisb.ttf", 32, ok);
    fontCode = loadFont("C:/Windows/Fonts/consola.ttf", 28, ok);
    ownFonts = ok;

    inA.maxLen = 3;
    inB.maxLen = 3;
    inC.maxLen = 3;
    inText.maxLen = 34;
    inText.anyText = true;
    for (int g = 0; g < G_COUNT; g++) lastView[g] = GROUP_VIEWS[g][0];
    newArray(0);
    newHash();
    sampleGraph(false);
    buildView(V_SORT);
}

GUIApp::~GUIApp() {
    if (ownFonts) {
        UnloadFont(fontUI);
        UnloadFont(fontBold);
        UnloadFont(fontCode);
    }
    CloseWindow();
}

const Frame* GUIApp::currentFrame() {
    Session& s = S();
    if (s.frames.empty()) return nullptr;
    int i = std::max(0, std::min(s.index, (int)s.frames.size() - 1));
    return &s.frames[i];
}

void GUIApp::toast(const std::string& text) {
    toastText = text;
    toastTimer = 2.6f;
}

bool GUIApp::readA(int& value, const char* what) {
    if (inA.text.empty()) {
        toast(std::string("Type a number in the ") + what + " box first.");
        inA.focused = true;
        return false;
    }
    value = std::atoi(inA.text.c_str());
    return true;
}

bool GUIApp::readB(int& value, const char* what) {
    if (inB.text.empty()) {
        toast(std::string("Type a number in the ") + what + " box first.");
        inB.focused = true;
        return false;
    }
    value = std::atoi(inB.text.c_str());
    return true;
}

void GUIApp::selectView(View v) {
    view = v;
    lastView[groupOf(v)] = v;
    buildView(v);
    playing = false;
    inA.focused = inB.focused = inC.focused = inText.focused = false;
    barAnims.clear();
    cellAnims.clear();
    nodeAnims.clear();
    vertexAnims.clear();
    codeHighlightY = 0;
}

// ============================================================================
//  Text
// ============================================================================

void GUIApp::Txt(const Font& f, const std::string& s, float x, float y, float size, Color c) {
    DrawTextEx(f, s.c_str(), { std::floor(x), std::floor(y) }, size, 0.0f, c);
}

float GUIApp::TxtW(const Font& f, const std::string& s, float size) {
    return MeasureTextEx(f, s.c_str(), size, 0.0f).x;
}

void GUIApp::TxtC(const Font& f, const std::string& s, float cx, float y, float size, Color c) {
    Txt(f, s, cx - TxtW(f, s, size) / 2.0f, y, size, c);
}

// ============================================================================
//  Widgets
// ============================================================================

bool GUIApp::Button(Rectangle r, const std::string& label, int style) {
    Vector2 m = GetMousePosition();
    bool hot = CheckCollisionPointRec(m, r);
    std::string key = label + std::to_string((int)r.x) + "_" + std::to_string((int)r.y);
    float& h = hover[key];
    h += ((hot ? 1.0f : 0.0f) - h) * (1.0f - std::exp(-dt * 16.0f));

    float rad = 0.32f;
    if (style == 2) {
        float c[3] = { 40.0f, 190.0f, 255.0f };
        glowRect(r, r.height * 0.32f, c, 0.45f + 0.35f * h, 9.0f);
        gradRounded(r, r.height * 0.32f, lighten(c, 0.2f * h), Color{ 40, 96, 255, 255 }, true);
        TxtC(fontBold, label, r.x + r.width / 2, r.y + (r.height - 16) / 2 - 1, 16, Color{ 255, 255, 255, 255 });
    }
    else if (style == 1) {
        DrawRectangleRounded(r, rad, 10, ColorAlpha(ACCENT, 0.14f + 0.06f * h));
        DrawRectangleRoundedLinesEx(r, rad, 10, 1.3f, ColorAlpha(ACCENT, 0.85f));
        TxtC(fontBold, label, r.x + r.width / 2, r.y + (r.height - 16) / 2 - 1, 16, ACCENT);
    }
    else {
        DrawRectangleRounded(r, rad, 10, ColorLerp(PANEL2, Color{ 40, 46, 62, 255 }, h));
        DrawRectangleRoundedLinesEx(r, rad, 10, 1.0f, ColorAlpha(WHITE, 0.07f + 0.10f * h));
        TxtC(fontUI, label, r.x + r.width / 2, r.y + (r.height - 16) / 2 - 1, 16, ColorLerp(TEXT2, TEXT, h));
    }
    return hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// icon ids: 0 first, 1 previous, 2 play, 3 pause, 4 next, 5 last
bool GUIApp::IconButton(Rectangle r, int icon, bool primary) {
    Vector2 m = GetMousePosition();
    bool hot = CheckCollisionPointRec(m, r);
    std::string key = "icon" + std::to_string(icon) + "_" + std::to_string((int)r.x);
    float& h = hover[key];
    h += ((hot ? 1.0f : 0.0f) - h) * (1.0f - std::exp(-dt * 16.0f));

    Color fg = TEXT;
    if (primary) {
        float c[3] = { 40.0f, 190.0f, 255.0f };
        glowRect(r, r.height / 2, c, 0.55f + 0.35f * h, 12.0f);
        gradRounded(r, r.height / 2, Color{ 0, 226, 255, 255 }, Color{ 40, 90, 255, 255 }, true);
        fg = WHITE;
    }
    else {
        DrawRectangleRounded(r, 0.5f, 12, ColorLerp(PANEL2, Color{ 42, 48, 66, 255 }, h));
        DrawRectangleRoundedLinesEx(r, 0.5f, 12, 1.0f, ColorAlpha(WHITE, 0.07f + 0.1f * h));
        fg = ColorLerp(TEXT2, TEXT, h);
    }
    float cx = r.x + r.width / 2, cy = r.y + r.height / 2;
    float s = primary ? 8.5f : 6.5f;
    auto tri = [&](float x, bool right) {
        if (right) DrawTriangle({ x - s * 0.7f, cy - s }, { x - s * 0.7f, cy + s }, { x + s * 0.9f, cy }, fg);
        else DrawTriangle({ x + s * 0.7f, cy - s }, { x - s * 0.9f, cy }, { x + s * 0.7f, cy + s }, fg);
    };
    switch (icon) {
        case 0: DrawRectangleRounded({ cx - s - 1, cy - s, 2.6f, s * 2 }, 0.5f, 4, fg); tri(cx + 2, false); break;
        case 1: tri(cx, false); break;
        case 2: tri(cx + 1, true); break;
        case 3:
            DrawRectangleRounded({ cx - s * 0.75f, cy - s, s * 0.55f, s * 2 }, 0.3f, 4, fg);
            DrawRectangleRounded({ cx + s * 0.2f, cy - s, s * 0.55f, s * 2 }, 0.3f, 4, fg);
            break;
        case 4: tri(cx, true); break;
        case 5: tri(cx - 2, true); DrawRectangleRounded({ cx + s - 1.5f, cy - s, 2.6f, s * 2 }, 0.5f, 4, fg); break;
    }
    return hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void GUIApp::Input(Rectangle r, InputBox& box, const char* hint) {
    Vector2 m = GetMousePosition();
    bool hot = CheckCollisionPointRec(m, r);
    if (hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) focusReq = &box;

    DrawRectangleRounded(r, 0.32f, 10, Color{ 12, 14, 20, 255 });
    if (box.focused) {
        float c[3] = { 112.0f, 170.0f, 255.0f };
        glowRect(r, r.height * 0.32f, c, 0.35f, 7.0f);
        DrawRectangleRoundedLinesEx(r, 0.32f, 10, 1.4f, ACCENT);
    }
    else {
        DrawRectangleRoundedLinesEx(r, 0.32f, 10, 1.0f, ColorAlpha(WHITE, hot ? 0.16f : 0.09f));
    }
    float ty = r.y + (r.height - 17) / 2 - 1;
    const Font& f = box.anyText ? fontCode : fontBold;
    float fs = box.anyText ? 16.0f : 17.0f;
    if (box.text.empty()) {
        Txt(fontUI, hint, r.x + 12, ty, 16, MUTED);
        if (box.focused && std::fmod(now, 1.0) < 0.55) {
            DrawRectangle((int)(r.x + 12), (int)(r.y + 9), 2, (int)(r.height - 18), ACCENT);
        }
        return;
    }
    // keep the end of long text visible
    std::string shown = box.text;
    while (shown.size() > 1 && TxtW(f, shown, fs) > r.width - 26) shown.erase(shown.begin());
    Txt(f, shown, r.x + 12, ty, fs, TEXT);
    if (box.focused && std::fmod(now, 1.0) < 0.55) {
        float cx = r.x + 12 + TxtW(f, shown, fs) + 1;
        DrawRectangle((int)cx, (int)(r.y + 9), 2, (int)(r.height - 18), ACCENT);
    }
}

// toolbar helpers: a cursor (tbX, tbY) moves to the right while widgets are placed
bool GUIApp::tbBtn(const std::string& label, int style) {
    float w = TxtW(fontUI, label, 16) + 30;
    bool c = Button({ tbX, tbY, w, tbH }, label, style);
    tbX += w + 8;
    return c;
}

void GUIApp::tbLabel(const std::string& text) {
    Txt(fontUI, text, tbX, tbY + 11, 14, MUTED);
    tbX += TxtW(fontUI, text, 14) + 10;
}

void GUIApp::tbSep() {
    DrawRectangle((int)tbX + 6, (int)tbY + 6, 1, (int)tbH - 12, LINE);
    tbX += 20;
}

void GUIApp::tbInput(InputBox& box, const char* hint, float w) {
    Input({ tbX, tbY, w, tbH }, box, hint);
    tbX += w + 12;
}

void GUIApp::handleTyping() {
    InputBox* boxes[4] = { &inA, &inB, &inC, &inText };
    InputBox* target = nullptr;
    for (InputBox* b : boxes) if (b->focused) target = b;
    int ch;
    while ((ch = GetCharPressed()) != 0) {
        bool digit = ch >= '0' && ch <= '9';
        bool printable = ch >= 32 && ch < 127;
        if (!target && view == V_STACK_APPS && printable) {
            inText.focused = true;
            target = &inText;
        }
        else if (!target && digit && view != V_SORT && view != V_COMPLEX && view != V_STACK_APPS) {
            inA.focused = true;
            target = &inA;
        }
        if (target && ((target->anyText && printable) || (!target->anyText && digit)) && (int)target->text.size() < target->maxLen) {
            target->text += (char)ch;
        }
    }
    if (target) {
        if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && !target->text.empty()) target->text.pop_back();
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) primaryAction();
        if (IsKeyPressed(KEY_ESCAPE)) { for (InputBox* b : boxes) b->focused = false; }
        if (IsKeyPressed(KEY_TAB) && !target->anyText) {
            InputBox* order[3] = { &inA, &inB, &inC };
            int at = 0;
            for (int i = 0; i < 3; i++) if (order[i] == target) at = i;
            target->focused = false;
            order[(at + 1) % 3]->focused = true;
        }
    }
}

void GUIApp::handleKeys() {
    if (inA.focused || inB.focused || inC.focused || inText.focused) return;
    Session& s = S();
    int last = (int)s.frames.size() - 1;
    if (IsKeyPressed(KEY_SPACE)) {
        if (!playing && s.index >= last) s.index = 0;
        playing = !playing;
        stepTimer = 0;
    }
    if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) && s.index < last) { playing = false; s.index++; }
    if ((IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) && s.index > 0) { playing = false; s.index--; }
    if (IsKeyPressed(KEY_HOME)) { playing = false; s.index = 0; }
    if (IsKeyPressed(KEY_END)) { playing = false; s.index = std::max(0, last); }
}

void GUIApp::updatePlayback() {
    if (!playing) return;
    Session& s = S();
    stepTimer += dt;
    float delay = (view == V_COMPLEX ? 0.12f : 0.62f) / speed;
    if (stepTimer >= delay) {
        stepTimer = 0;
        if (s.index < (int)s.frames.size() - 1) s.index++;
        else playing = false;
    }
}

// ============================================================================
//  Layout and chrome
// ============================================================================

void GUIApp::layout() {
    float pad = 20, playH = 84, panelW = 420;
    headerH = 64 + rowsFor(view) * 48 + 6;
    float top = headerH + 16;
    float playY = H - playH - pad + 4;
    stageRect = { pad, top, W - pad * 2 - panelW - 16, playY - 14 - top };
    panelRect = { stageRect.x + stageRect.width + 16, top, panelW, stageRect.height };
    float infoH = 168;
    codeRect = { panelRect.x, panelRect.y, panelRect.width, panelRect.height - infoH - 14 };
    infoRect = { panelRect.x, codeRect.y + codeRect.height + 14, panelRect.width, infoH };
}

void GUIApp::drawBackground() {
    ClearBackground(BG);
    BeginBlendMode(BLEND_ADDITIVE);
    DrawCircleGradient(160, 40, 520, Color{ 40, 60, 150, 26 }, Color{ 0, 0, 0, 0 });
    DrawCircleGradient(W - 120, H - 60, 560, Color{ 20, 120, 110, 18 }, Color{ 0, 0, 0, 0 });
    EndBlendMode();
}

void GUIApp::drawTopBar() {
    DrawRectangle(0, 0, W, (int)headerH, HEADER);
    DrawRectangle(0, (int)headerH - 1, W, 1, LINE);

    // brand mark: three glowing bars
    float bx = 24, by = 14;
    float c1[3] = { 96, 150, 255 }, c2[3] = { 74, 226, 164 }, c3[3] = { 255, 205, 86 };
    float hs[3] = { 12, 24, 17 };
    const float* cs[3] = { c1, c2, c3 };
    for (int i = 0; i < 3; i++) {
        Rectangle r = { bx + i * 11.0f, by + 28 - hs[i], 7, hs[i] };
        glowRect(r, 3, cs[i], 0.8f, 7);
        gradRounded(r, 3, lighten(cs[i], 0.25f), rgb(cs[i]), false);
    }
    Txt(fontBold, "NodeScape", 70, 12, 22, TEXT);
    Txt(fontUI, "Data structures, animated", 70, 36, 13, MUTED);

    // group tabs
    float x = 300;
    Group current = groupOf(view);
    for (int g = 0; g < G_COUNT; g++) {
        const char* name = groupName((Group)g);
        float w = TxtW(fontBold, name, 16) + 26;
        Rectangle r = { x, 10, w, 38 };
        bool active = (current == (Group)g);
        bool hot = CheckCollisionPointRec(GetMousePosition(), r);
        float& h = hover[std::string("tab") + name];
        h += ((hot ? 1.0f : 0.0f) - h) * (1.0f - std::exp(-dt * 14.0f));
        Color col = active ? TEXT : ColorLerp(TEXT2, TEXT, h);
        TxtC(active ? fontBold : fontUI, name, r.x + w / 2, r.y + 9, 16, col);
        if (active) {
            float c[3] = { 112, 170, 255 };
            Rectangle u = { r.x + 10, r.y + 34, w - 20, 3 };
            glowRect(u, 1.5f, c, 0.9f, 8);
            gradRounded(u, 1.5f, Color{ 0, 226, 255, 255 }, Color{ 40, 90, 255, 255 }, true);
        }
        if (hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !active) selectView(lastView[g]);
        x += w + 2;
    }
    std::string hint = "Space  play      \xE2\x86\x90 \xE2\x86\x92  step";
    Txt(fontUI, hint, W - 20 - TxtW(fontUI, hint, 13), 20, 13, MUTED);
}

void GUIApp::drawToolbar() {
    int rows = rowsFor(view);
    Group g = groupOf(view);
    tbH = 40;

    if (view == V_SORT) {
        tbY = 64;
        tbX = 24;
        tbLabel("Algorithm");
        const char* algos[ALGO_COUNT] = { "Bubble", "Selection", "Insertion", "Merge", "Quick", "Heap" };
        for (int i = 0; i < ALGO_COUNT; i++) {
            if (tbBtn(algos[i], sortAlgo == (SortAlgo)i ? 1 : 0)) {
                sortAlgo = (SortAlgo)i;
                runSort();
                playing = true;
            }
        }
        float rx = W - 24;
        auto rbtn = [&](const char* t) -> bool {
            float w = TxtW(fontUI, t, 16) + 30;
            rx -= w;
            bool c = Button({ rx, tbY, w, tbH }, t, 0);
            rx -= 8;
            return c;
        };
        if (rbtn("Nearly sorted")) { newArray(2); runSort(); playing = true; }
        if (rbtn("Reversed")) { newArray(1); runSort(); playing = true; }
        if (rbtn("Shuffle")) { newArray(0); runSort(); playing = true; }
        rx -= 8;
        rx -= 118;
        Rectangle box = { rx, tbY, 118, tbH };
        DrawRectangleRounded(box, 0.32f, 10, PANEL2);
        DrawRectangleRoundedLinesEx(box, 0.32f, 10, 1.0f, ColorAlpha(WHITE, 0.08f));
        if (Button({ rx + 4, tbY + 4, 30, tbH - 8 }, "-", 0) && arraySize > 5) { arraySize--; newArray(3); runSort(); }
        if (Button({ rx + 84, tbY + 4, 30, tbH - 8 }, "+", 0) && arraySize < 40) { arraySize++; newArray(3); runSort(); }
        TxtC(fontBold, std::to_string(arraySize), rx + 59, tbY + 11, 17, TEXT);
        Txt(fontUI, "Size", rx - 40, tbY + 11, 14, MUTED);
        return;
    }

    // second row: the views of this group (or hashing options)
    if (rows == 2) {
        tbY = 64;
        tbX = 24;
        if (view == V_HASH) {
            auto rebuild = [&]() { newHash(); sessions[V_HASH].built = false; selectView(V_HASH); };
            tbLabel("Collisions");
            const char* methods[4] = { "Chaining", "Linear probing", "Quadratic probing", "Double hashing" };
            for (int i = 0; i < 4; i++) if (tbBtn(methods[i], hashMethod == i ? 1 : 0)) { hashMethod = i; rebuild(); }
            tbSep();
            tbLabel("Hash function");
            const char* funcs[3] = { "Division", "Folding", "Mid-square" };
            for (int i = 0; i < 3; i++) if (tbBtn(funcs[i], hashFunc == i ? 1 : 0)) { hashFunc = i; rebuild(); }
            tbSep();
            tbLabel("Size");
            const int sizes[3] = { 7, 11, 13 };
            for (int i = 0; i < 3; i++) if (tbBtn(std::to_string(sizes[i]), hashSize == sizes[i] ? 1 : 0)) { hashSize = sizes[i]; rebuild(); }
        }
        else {
            tbLabel(g == G_LIST ? "List type" : g == G_STACK ? "Topic" : g == G_QUEUE ? "Queue type" : "Tree");
            static const char* listNames[3] = { "Singly linked", "Doubly linked", "Circular" };
            static const char* stackNames[4] = { "Array stack", "Linked stack", "Expressions", "Recursion" };
            static const char* queueNames[4] = { "Simple queue", "Circular queue", "Deque", "Linked queue" };
            static const char* treeNames[2] = { "Binary search tree", "AVL tree" };
            const char** names = g == G_LIST ? listNames : g == G_STACK ? stackNames : g == G_QUEUE ? queueNames : treeNames;
            for (int i = 0; i < GROUP_COUNT[g]; i++) {
                View v = GROUP_VIEWS[g][i];
                if (tbBtn(names[i], view == v ? 1 : 0) && view != v) selectView(v);
            }
        }
        tbY = 112;
    }
    else {
        tbY = 64;
    }
    tbX = 24;
    toolbarFor(view);
}

// ============================================================================
//  Side panels and playback bar
// ============================================================================

void GUIApp::drawCodePanel() {
    Rectangle r = codeRect;
    DrawRectangleRounded(r, 0.03f, 12, PANEL);
    DrawRectangleRoundedLinesEx(r, 0.03f, 12, 1.0f, LINE);
    Txt(fontBold, "PSEUDOCODE", r.x + 22, r.y + 18, 12, MUTED);
    DrawRectangle((int)r.x + 1, (int)r.y + 46, (int)r.width - 2, 1, LINE);

    Session& s = S();
    const Frame* f = currentFrame();
    int active = f ? f->codeLine : -1;
    if (s.code.empty()) {
        bool growth = (view == V_COMPLEX);
        TxtC(fontUI, growth ? "Each curve shows how the number" : "Run an operation to see", r.x + r.width / 2, r.y + r.height / 2 - 20, 15, MUTED);
        TxtC(fontUI, growth ? "of steps grows when n grows." : "the code that is executing.", r.x + r.width / 2, r.y + r.height / 2 + 2, 15, MUTED);
        return;
    }
    float lineH = std::min(32.0f, (r.height - 70) / (float)s.code.size());
    float y0 = r.y + 58;
    BeginScissorMode((int)r.x + 1, (int)r.y + 47, (int)r.width - 2, (int)r.height - 50);

    if (active >= 0 && active < (int)s.code.size()) {
        float ty = y0 + active * lineH;
        if (std::fabs(codeHighlightY - ty) > 200 || codeHighlightY == 0) codeHighlightY = ty;
        codeHighlightY += (ty - codeHighlightY) * (1.0f - std::exp(-dt * 16.0f));
        Rectangle hb = { r.x + 8, codeHighlightY - 2, r.width - 16, lineH };
        float c[3] = { 112.0f, 170.0f, 255.0f };
        glowRect(hb, 8, c, 0.22f, 8);
        DrawRectangleRounded(hb, 0.3f, 8, ColorAlpha(ACCENT, 0.13f));
        DrawRectangleRounded({ hb.x, hb.y + 4, 3, hb.height - 8 }, 1.0f, 4, ACCENT);
    }
    for (size_t i = 0; i < s.code.size(); i++) {
        bool on = ((int)i == active);
        float y = y0 + i * lineH;
        Txt(fontCode, std::to_string(i + 1), r.x + 20, y + (lineH - 14) / 2 - 2, 13, on ? ACCENT : MUTED);
        float avail = r.width - 64 - 12;
        float size = 14.0f;
        float w = TxtW(fontCode, s.code[i], size);
        if (w > avail) size = std::max(10.5f, size * avail / w);
        std::string line = s.code[i];
        while (TxtW(fontCode, line, size) > avail && line.size() > 4) line.pop_back();
        if (line.size() != s.code[i].size()) line += "\xE2\x80\xA6";
        Txt(fontCode, line, r.x + 52, y + (lineH - size) / 2 - 2, size, on ? TEXT : TEXT2);
    }
    EndScissorMode();
}

void GUIApp::drawInfoPanel() {
    Rectangle r = infoRect;
    DrawRectangleRounded(r, 0.06f, 12, PANEL);
    DrawRectangleRoundedLinesEx(r, 0.06f, 12, 1.0f, LINE);
    Txt(fontBold, view == V_COMPLEX ? "EXAMPLES" : "COMPLEXITY", r.x + 22, r.y + 16, 12, MUTED);
    Info in = infoFor(view, S().title);
    struct Cellx { const char* name; const char* val; };
    Cellx cells[4] = { { "Average", in.avg }, { "Best", in.best }, { "Worst", in.worst }, { "Space", in.space } };
    if (view == V_COMPLEX) {
        cells[0] = { "Binary", "O(log n)" };
        cells[1] = { "Linear", "O(n)" };
        cells[2] = { "Bubble", "O(n^2)" };
        cells[3] = { "Subsets", "O(2^n)" };
        in.note = "Binary search, one loop, two nested loops, trying every subset.";
    }
    float cw = (r.width - 44 - 18) / 4;
    for (int i = 0; i < 4; i++) {
        Rectangle c = { r.x + 22 + i * (cw + 6), r.y + 42, cw, 56 };
        DrawRectangleRounded(c, 0.2f, 8, PANEL2);
        TxtC(fontUI, cells[i].name, c.x + cw / 2, c.y + 8, 12, MUTED);
        std::string v = cells[i].val;
        float fs = TxtW(fontCode, v, 15) > cw - 8 ? 12.0f : 15.0f;
        TxtC(fontCode, v, c.x + cw / 2, c.y + 28, fs, ACCENT);
    }
    auto lines = wrapText([&](const std::string& t) { return TxtW(fontUI, t, 14); }, in.note, r.width - 44);
    for (size_t i = 0; i < lines.size() && i < 3; i++) Txt(fontUI, lines[i], r.x + 22, r.y + 112 + i * 20, 14, TEXT2);
}

void GUIApp::drawPlayback() {
    float pad = 20, h = 76;
    Rectangle r = { pad, H - h - pad + 4, W - pad * 2.0f, h };
    DrawRectangleRounded(r, 0.2f, 12, PANEL);
    DrawRectangleRoundedLinesEx(r, 0.2f, 12, 1.0f, LINE);

    Session& s = S();
    int total = (int)s.frames.size();
    int last = std::max(0, total - 1);
    float cy = r.y + r.height / 2;
    float x = r.x + 24;

    if (IconButton({ x, cy - 18, 40, 36 }, 0, false)) { playing = false; s.index = 0; }
    x += 48;
    if (IconButton({ x, cy - 18, 40, 36 }, 1, false)) { playing = false; if (s.index > 0) s.index--; }
    x += 48;
    if (IconButton({ x, cy - 24, 56, 48 }, playing ? 3 : 2, true)) {
        if (!playing && s.index >= last) s.index = 0;
        playing = !playing;
        stepTimer = 0;
    }
    x += 64;
    if (IconButton({ x, cy - 18, 40, 36 }, 4, false)) { playing = false; if (s.index < last) s.index++; }
    x += 48;
    if (IconButton({ x, cy - 18, 40, 36 }, 5, false)) { playing = false; s.index = last; }
    x += 64;

    const char* names[4] = { "0.5x", "1x", "2x", "4x" };
    float speeds[4] = { 0.5f, 1.0f, 2.0f, 4.0f };
    float sx = r.x + r.width - 24 - (4 * 52 + 3 * 6);
    Txt(fontUI, "Speed", sx - 52, cy - 9, 14, MUTED);
    for (int i = 0; i < 4; i++) {
        if (Button({ sx + i * 58, cy - 17, 52, 34 }, names[i], speed == speeds[i] ? 1 : 0)) speed = speeds[i];
    }

    float tx = x + 10;
    float tw = sx - 70 - tx;
    float ty = cy + 8;
    std::string step = view == V_COMPLEX ? "n = " + std::to_string(s.index + 1) : "Step " + std::to_string(s.index + 1) + " of " + std::to_string(std::max(1, total));
    Txt(fontUI, step, tx, cy - 24, 14, TEXT2);
    Rectangle hit = { tx - 8, ty - 14, tw + 16, 28 };
    Vector2 m = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(m, hit)) draggingScrub = true;
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) draggingScrub = false;
    if (draggingScrub && total > 1) {
        playing = false;
        float t = clampf((m.x - tx) / tw, 0.0f, 1.0f);
        s.index = (int)std::lround(t * last);
    }
    float prog = total > 1 ? (float)s.index / last : 0.0f;
    Rectangle track = { tx, ty - 3, tw, 6 };
    DrawRectangleRounded(track, 1.0f, 6, Color{ 12, 14, 20, 255 });
    Rectangle fill = { tx, ty - 3, std::max(6.0f, tw * prog), 6 };
    float c1[3] = { 96, 150, 255 };
    glowRect(fill, 3, c1, 0.5f, 8);
    DrawRectangleGradientEx(fill, Color{ 0, 226, 255, 255 }, Color{ 0, 226, 255, 255 }, Color{ 74, 226, 164, 255 }, Color{ 74, 226, 164, 255 });
    float thx = tx + tw * prog;
    glowCircle(thx, ty, 7, c1, 1.0f, 12);
    DrawCircleV({ thx, ty }, 8, Color{ 245, 248, 255, 255 });
    DrawCircleV({ thx, ty }, 4, ACCENT);
}

void GUIApp::drawToast() {
    if (toastTimer <= 0) return;
    toastTimer -= dt;
    float a = std::min(1.0f, toastTimer * 3.0f);
    float w = TxtW(fontUI, toastText, 16) + 44;
    Rectangle r = { W / 2.0f - w / 2, stageRect.y + 94, w, 40 };
    float c[3] = { 255, 205, 86 };
    glowRect(r, 12, c, 0.25f * a, 12);
    DrawRectangleRounded(r, 0.4f, 10, ColorAlpha(Color{ 30, 26, 14, 255 }, a));
    DrawRectangleRoundedLinesEx(r, 0.4f, 10, 1.2f, ColorAlpha(Color{ 255, 205, 86, 255 }, a * 0.8f));
    TxtC(fontUI, toastText, r.x + w / 2, r.y + 10, 16, ColorAlpha(Color{ 255, 226, 150, 255 }, a));
}

// ============================================================================
//  Main loop
// ============================================================================

void GUIApp::run() {
    while (!WindowShouldClose()) {
        W = GetScreenWidth();
        H = GetScreenHeight();
        dt = fixedDt > 0 ? fixedDt : std::min(GetFrameTime(), 0.05f);
        now = GetTime();

        focusReq = nullptr;
        layout();
        handleKeys();
        updatePlayback();

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

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            inA.focused = (focusReq == &inA);
            inB.focused = (focusReq == &inB);
            inC.focused = (focusReq == &inC);
            inText.focused = (focusReq == &inText);
        }
        handleTyping();
    }
}
