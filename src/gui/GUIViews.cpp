#include "GUIApp.h"
#include "../core/Complexity.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>

using namespace gfx;

static const Color C_TEXT = gfx::TEXT, C_TEXT2 = gfx::TEXT2, C_MUTED = gfx::MUTED, C_PANEL = gfx::PANEL,
                   C_PANEL2 = gfx::PANEL2, C_ACCENT = gfx::ACCENT, C_LINE = gfx::LINE;

// ============================================================================
//  Stage: the card that shows the current frame
// ============================================================================

void GUIApp::drawLegend(float x, float y) {
    struct Item { Role role; const char* text; };
    std::vector<Item> items;
    auto add = [&](Role r, const char* t) { items.push_back({ r, t }); };
    switch (view) {
        case V_SORT:
            add(ROLE_CURRENT, "Comparing"); add(ROLE_NEW, "Condition met"); add(ROLE_KEPT, "Not met");
            add(ROLE_FOUND, "Pivot / key / min"); add(ROLE_DONE, "Sorted");
            break;
        case V_ARRAY:
            add(ROLE_CURRENT, "Looking at"); add(ROLE_NEW, "New / moved"); add(ROLE_KEPT, "Discarded");
            add(ROLE_FOUND, "Found"); add(ROLE_DIM, "Ruled out"); add(ROLE_REMOVED, "Removed");
            break;
        case V_GRAPH:
            add(ROLE_CURRENT, "Current"); add(ROLE_NEW, "Discovered"); add(ROLE_KEPT, "Skipped");
            add(ROLE_DONE, "Finished / in the tree");
            break;
        case V_HASH:
            add(ROLE_CURRENT, "Probing"); add(ROLE_KEPT, "Collision"); add(ROLE_NEW, "Stored");
            add(ROLE_FOUND, "Found"); add(ROLE_REMOVED, "Deleted");
            break;
        case V_COMPLEX:
            break;
        default:
            add(ROLE_CURRENT, "Visiting"); add(ROLE_NEW, "New / moved"); add(ROLE_KEPT, "Not met");
            add(ROLE_FOUND, "Found"); add(ROLE_REMOVED, "Removed"); add(ROLE_DONE, "Done");
            break;
    }
    for (auto& it : items) {
        Tint t = tintFor(it.role);
        float m[3];
        midOf(t, m);
        glowCircle(x + 5, y + 8, 4, m, 0.8f, 6);
        gradCircleH(x + 5, y + 8, 5, rgb(t.top), rgb(t.bot));
        Txt(fontUI, it.text, x + 16, y, 13, C_TEXT2);
        x += 16 + TxtW(fontUI, it.text, 13) + 18;
    }
}

void GUIApp::drawStage() {
    Rectangle r = stageRect;
    DrawRectangleRounded(r, 0.025f, 12, C_PANEL);
    DrawRectangleRoundedLinesEx(r, 0.025f, 12, 1.0f, C_LINE);

    // dotted grid, clipped to the card
    BeginScissorMode((int)r.x + 1, (int)r.y + 1, (int)r.width - 2, (int)r.height - 2);
    for (float gx = r.x + 24; gx < r.x + r.width; gx += 28)
        for (float gy = r.y + 90; gy < r.y + r.height - 70; gy += 28)
            DrawRectangle((int)gx, (int)gy, 2, 2, Color{ 255, 255, 255, 9 });
    EndScissorMode();

    const Frame* f = currentFrame();
    Session& s = S();
    std::string title = s.title.empty() ? std::string(viewName(view)) : s.title;
    Txt(fontBold, title, r.x + 24, r.y + 16, 22, C_TEXT);

    // counters (right side)
    if (f && view != V_COMPLEX) {
        float cx = r.x + r.width - 24;
        auto chip = [&](const std::string& name, long value) {
            std::string v = std::to_string(value);
            float w = TxtW(fontBold, v, 17) + TxtW(fontUI, name, 13) + 34;
            cx -= w;
            Rectangle cr = { cx, r.y + 14, w, 32 };
            DrawRectangleRounded(cr, 0.4f, 10, C_PANEL2);
            DrawRectangleRoundedLinesEx(cr, 0.4f, 10, 1.0f, ColorAlpha(WHITE, 0.06f));
            Txt(fontUI, name, cr.x + 12, cr.y + 8, 13, C_TEXT2);
            Txt(fontBold, v, cr.x + 22 + TxtW(fontUI, name, 13), cr.y + 5, 17, C_ACCENT);
            cx -= 8;
        };
        if (view == V_SORT) {
            chip("Swaps / moves", f->swaps);
            chip("Comparisons", f->comparisons);
        }
        else if (f->kind == KIND_HANOI) {
            chip("Moves", f->swaps);
        }
        else if (f->comparisons > 0) {
            if (view == V_HASH) chip("Probes", f->comparisons);
            else if (view == V_RECURSION) chip("Calls", f->comparisons);
            else if (view == V_ARRAY) chip("Comparisons", f->comparisons);
            else chip("Nodes visited", f->comparisons);
        }
    }
    drawLegend(r.x + 24, r.y + 54);
    DrawRectangle((int)r.x + 1, (int)r.y + 80, (int)r.width - 2, 1, C_LINE);

    Rectangle area = { r.x, r.y + 82, r.width, r.height - 82 - 74 };
    if (view == V_COMPLEX) {
        drawComplexity(area);
    }
    else if (f) {
        switch (f->kind) {
            case KIND_BARS: drawBars(*f, area); break;
            case KIND_ARRAY:
            case KIND_SLOTS: drawBoxes(*f, area); break;
            case KIND_LIST: drawList(*f, area); break;
            case KIND_STACK: drawStack(*f, area); break;
            case KIND_GRAPH: drawGraph(*f, area); break;
            case KIND_HASH: drawHash(*f, area); break;
            case KIND_HANOI: drawHanoi(*f, area); break;
            default: drawTree(*f, area); break;
        }
    }

    // narrator strip
    Rectangle ms = { r.x + 14, r.y + r.height - 66, r.width - 28, 52 };
    DrawRectangleRounded(ms, 0.28f, 10, Color{ 12, 14, 20, 255 });
    DrawRectangleRoundedLinesEx(ms, 0.28f, 10, 1.0f, ColorAlpha(WHITE, 0.06f));
    if (f) {
        float pulse = 0.55f + 0.45f * std::sin((float)now * 4.0f);
        float c[3] = { 112.0f, 170.0f, 255.0f };
        glowCircle(ms.x + 22, ms.y + ms.height / 2, 4, c, 0.9f * pulse, 9);
        DrawCircleV({ ms.x + 22, ms.y + ms.height / 2 }, 4.5f, ColorAlpha(C_ACCENT, 0.6f + 0.4f * pulse));
        auto lines = wrapText([&](const std::string& t) { return TxtW(fontUI, t, 17); }, f->message, ms.width - 64);
        float ly = ms.y + (ms.height - (float)std::min<size_t>(lines.size(), 2) * 22) / 2 + 1;
        for (size_t i = 0; i < lines.size() && i < 2; i++) Txt(fontUI, lines[i], ms.x + 40, ly + i * 22, 17, C_TEXT);
    }
}

// extra text lines of a frame (for example "Output: a b +")
void GUIApp::drawInfoLines(const Frame& f, float x, float y, float maxW) {
    float yy = y;
    for (const std::string& line : f.info) {
        auto parts = wrapText([&](const std::string& t) { return TxtW(fontCode, t, 15); }, line, maxW);
        for (const std::string& p : parts) {
            Txt(fontCode, p, x, yy, 15, C_TEXT2);
            yy += 21;
        }
    }
}

static void drawChip(GUIApp* app, const Font& font, float cx, float yBottom, const std::string& text, const float* c) {
    float size = 12.5f;
    float w = MeasureTextEx(font, text.c_str(), size, 0).x + 14;
    Rectangle r = { cx - w / 2, yBottom - 22, w, 20 };
    DrawRectangleRounded(r, 0.5f, 8, Color{ 12, 14, 20, 235 });
    DrawRectangleRoundedLinesEx(r, 0.5f, 8, 1.2f, rgb(c));
    DrawTextEx(font, text.c_str(), { std::floor(r.x + 7), std::floor(r.y + 2.5f) }, size, 0, lighten(c, 0.25f));
    DrawTriangle({ cx - 4, r.y + r.height + 1 }, { cx, r.y + r.height + 6 }, { cx + 4, r.y + r.height + 1 }, rgb(c));
    (void)app;
}

static std::string markerText(const Frame& f, int index) {
    std::string t;
    for (auto& m : f.markers) {
        if (m.index == index) { if (!t.empty()) t += " / "; t += m.label; }
    }
    return t;
}

void GUIApp::drawBars(const Frame& f, Rectangle area) {
    int n = (int)f.cells.size();
    if (n == 0) {
        TxtC(fontUI, "Nothing to show.", area.x + area.width / 2, area.y + area.height / 2, 18, C_MUTED);
        return;
    }
    if ((int)barAnims.size() != n) {
        barAnims.assign(n, BarAnim());
        for (int i = 0; i < n; i++) barAnims[i].tint = tintFor(f.cells[i].role);
        int mx = 1;
        for (auto& c : f.cells) mx = std::max(mx, c.value);
        for (int i = 0; i < n; i++) barAnims[i].h = (float)f.cells[i].value / mx;
    }
    int maxVal = 1;
    for (auto& c : f.cells) maxVal = std::max(maxVal, c.value);

    float slot = (area.width - 70) / n;
    float barW = std::min(70.0f, slot * 0.74f);
    float startX = area.x + (area.width - slot * n) / 2 + (slot - barW) / 2;
    float baseY = area.y + area.height - 34;
    float maxH = area.height - 110;
    float k = 1.0f - std::exp(-dt * 13.0f);
    float kc = 1.0f - std::exp(-dt * 16.0f);

    // soft floor line
    DrawRectangleGradientEx({ area.x + 30, baseY + 1, area.width - 60, 1 }, Color{ 255,255,255,0 }, Color{ 255,255,255,0 }, Color{ 255,255,255,30 }, Color{ 255,255,255,30 });

    for (int i = 0; i < n; i++) {
        BarAnim& b = barAnims[i];
        float target = std::max(0.07f, (float)f.cells[i].value / maxVal);
        b.h += (target - b.h) * k;
        approach(b.tint, tintFor(f.cells[i].role), kc);

        float h = b.h * maxH;
        Rectangle r = { startX + i * slot, baseY - h, barW, h };
        float gm[3]; midOf(b.tint, gm);
        glowRect(r, std::min(8.0f, barW * 0.25f), gm, b.tint.glow, 4.0f + 16.0f * std::min(1.0f, b.tint.glow));
        gradRounded(r, std::min(8.0f, barW * 0.25f), rgb(b.tint.top), rgb(b.tint.bot), false);
        // glossy highlight at the top and a bright top edge
        {
            int gh = (int)std::min(h * 0.4f, 70.0f);
            for (int row = 0; row < gh; row++) { // one pixel row at a time: no overlap, no banding
                float t = (float)row / std::max(1, gh);
                DrawRectangle((int)(r.x + 1), (int)(r.y + 3) + row, (int)(barW - 2), 1, ColorAlpha(WHITE, 0.24f * (1.0f - t) * (1.0f - t)));
            }
        }
        DrawRectangleRounded({ r.x + 3, r.y + 1.5f, barW - 6, 2.0f }, 1.0f, 4, ColorAlpha(WHITE, 0.45f));

        // value above, index below
        std::string v = std::to_string(f.cells[i].value);
        float vs = barW >= 26 ? 15.0f : 12.0f;
        if (barW >= 18) TxtC(fontBold, v, r.x + barW / 2, r.y - vs - 7, vs, C_TEXT);
        TxtC(fontUI, std::to_string(i), r.x + barW / 2, baseY + 9, 12, C_MUTED);

        std::string mt = markerText(f, i);
        if (!mt.empty()) drawChip(this, fontBold, r.x + barW / 2, r.y - (barW >= 18 ? 28 : 8), mt, b.tint.top);
    }
}


// ============================================================================
//  Linked lists (singly, doubly, circular) and linked stacks/queues drawn as rows
// ============================================================================

void GUIApp::drawList(const Frame& f, Rectangle area) {
    int n = (int)f.cells.size();
    float cy = area.y + area.height * 0.55f;
    if (n == 0) {
        TxtC(fontBold, f.circular ? "empty circle" : (f.doubly ? "NULL  \xE2\x86\x94  NULL" : "head  \xE2\x86\x92  NULL"), area.x + area.width / 2, cy - 14, 24, C_TEXT2);
        TxtC(fontUI, "The list is empty.", area.x + area.width / 2, cy + 20, 16, C_MUTED);
    }
    float extra = (f.doubly ? 120.0f : 0.0f) + 100.0f;
    float nodeW = clampf((area.width - extra - 60) / std::max(1, n) * 0.62f, 38.0f, 68.0f);
    float gap = clampf(nodeW * 0.7f, 22.0f, 50.0f);
    float total = n * nodeW + std::max(0, n - 1) * gap + (f.circular ? 0.0f : gap + 62) + (f.doubly ? gap + 62 : 0.0f);
    float startX = area.x + (area.width - total) / 2 + (f.doubly ? gap + 62 : 0.0f);
    float k = 1.0f - std::exp(-dt * 12.0f);
    float kc = 1.0f - std::exp(-dt * 16.0f);

    if ((int)cellAnims.size() < n) cellAnims.resize(n);
    for (int i = 0; i < n; i++) {
        CellAnim& a = cellAnims[i];
        float tx = startX + i * (nodeW + gap);
        if (!a.init) { a.x = tx; a.tint = tintFor(f.cells[i].role); a.init = true; }
        a.x += (tx - a.x) * k;
        approach(a.tint, tintFor(f.cells[i].role), kc);
    }
    for (int i = n; i < (int)cellAnims.size(); i++) cellAnims[i].init = false;

    Color ac = ColorAlpha(C_TEXT2, 0.75f);
    // arrows first so nodes cover their ends
    for (int i = 0; i < n; i++) {
        float x0 = cellAnims[i].x + nodeW;
        bool hasNext = (i + 1 < n);
        if (!hasNext && f.circular) continue;
        float x1 = hasNext ? cellAnims[i + 1].x : x0 + gap + 4;
        float yy = f.doubly ? cy - 8 : cy;
        DrawLineEx({ x0 + 4, yy }, { x1 - 9, yy }, 2.2f, ac);
        arrowHead({ x1 - 1, yy }, { x1 - 12, yy }, 10, ac);
        if (f.doubly && hasNext) { // the prev link goes backwards
            DrawLineEx({ x1 - 4, cy + 10 }, { x0 + 9, cy + 10 }, 2.2f, ColorAlpha(ACCENT, 0.7f));
            arrowHead({ x0 + 1, cy + 10 }, { x0 + 12, cy + 10 }, 10, ColorAlpha(ACCENT, 0.9f));
        }
    }
    if (n > 0 && !f.circular) {
        float nx = cellAnims[n - 1].x + nodeW + gap + 4;
        Rectangle nr = { nx, cy - 16, 52, 32 };
        DrawRectangleRounded(nr, 0.4f, 8, Color{ 12, 14, 20, 255 });
        DrawRectangleRoundedLinesEx(nr, 0.4f, 8, 1.0f, ColorAlpha(WHITE, 0.14f));
        TxtC(fontCode, "NULL", nr.x + nr.width / 2, nr.y + 7, 14, C_MUTED);
    }
    if (n > 0 && f.doubly) { // NULL in front of the head (the prev of the first node)
        float nx = cellAnims[0].x - gap - 56;
        Rectangle nr = { nx, cy - 16, 52, 32 };
        DrawRectangleRounded(nr, 0.4f, 8, Color{ 12, 14, 20, 255 });
        DrawRectangleRoundedLinesEx(nr, 0.4f, 8, 1.0f, ColorAlpha(WHITE, 0.14f));
        TxtC(fontCode, "NULL", nr.x + nr.width / 2, nr.y + 7, 14, C_MUTED);
        DrawLineEx({ nr.x + nr.width, cy + 10 }, { cellAnims[0].x - 2, cy + 10 }, 2.2f, ColorAlpha(ACCENT, 0.7f));
        arrowHead({ nr.x + nr.width + 2, cy + 10 }, { nr.x + nr.width + 12, cy + 10 }, 10, ColorAlpha(ACCENT, 0.9f));
    }
    if (n > 1 && f.circular) { // the last node points back to the first one
        float xl = cellAnims[n - 1].x + nodeW / 2;
        float xf = cellAnims[0].x + nodeW / 2;
        float yb = cy + nodeW / 2;
        float drop = 58;
        Color cc = ColorAlpha(Color{ 255, 244, 0, 255 }, 0.8f);
        DrawLineEx({ xl, yb + 2 }, { xl, yb + drop }, 2.4f, cc);
        DrawLineEx({ xl, yb + drop }, { xf, yb + drop }, 2.4f, cc);
        DrawLineEx({ xf, yb + drop }, { xf, yb + 12 }, 2.4f, cc);
        arrowHead({ xf, yb + 2 }, { xf, yb + 14 }, 11, cc);
        TxtC(fontUI, "last node points back to the first", (xl + xf) / 2, yb + drop + 6, 13, C_MUTED);
    }
    else if (n == 1 && f.circular) {
        float x = cellAnims[0].x + nodeW / 2;
        float yb = cy + nodeW / 2;
        Color cc = ColorAlpha(Color{ 255, 244, 0, 255 }, 0.8f);
        DrawLineEx({ x + 12, yb + 2 }, { x + 12, yb + 40 }, 2.4f, cc);
        DrawLineEx({ x + 12, yb + 40 }, { x - 12, yb + 40 }, 2.4f, cc);
        DrawLineEx({ x - 12, yb + 40 }, { x - 12, yb + 12 }, 2.4f, cc);
        arrowHead({ x - 12, yb + 2 }, { x - 12, yb + 14 }, 11, cc);
        TxtC(fontUI, "a single node points to itself", x, yb + 46, 13, C_MUTED);
    }

    for (int i = 0; i < n; i++) {
        CellAnim& a = cellAnims[i];
        Rectangle r = { a.x, cy - nodeW / 2, nodeW, nodeW };
        float gm[3]; midOf(a.tint, gm);
        glowRect(r, 12, gm, a.tint.glow, 4.0f + 16.0f * std::min(1.0f, a.tint.glow));
        gradRounded(r, 12, rgb(a.tint.top), rgb(a.tint.bot), true);
        DrawRectangleRoundedLinesEx(r, 0.3f, 10, 1.2f, ColorAlpha(WHITE, 0.22f));
        TxtC(fontBold, std::to_string(f.cells[i].value), r.x + nodeW / 2, cy - 11, 21, textOn(a.tint));
        std::string mt = markerText(f, i);
        if (!mt.empty()) drawChip(this, fontBold, r.x + nodeW / 2, r.y - 8, mt, a.tint.top);
    }

    if (f.hasFloating) {
        float fx = n > 0 ? cellAnims[0].x : startX;
        Rectangle r = { fx, cy - nodeW / 2 - 150, nodeW, nodeW };
        Tint t = tintFor(ROLE_NEW);
        float pulse = 0.8f + 0.2f * std::sin((float)now * 5.0f);
        float gm[3]; midOf(t, gm);
        glowRect(r, 12, gm, t.glow * pulse, 20);
        gradRounded(r, 12, rgb(t.top), rgb(t.bot), true);
        DrawRectangleRoundedLinesEx(r, 0.3f, 10, 1.2f, ColorAlpha(WHITE, 0.3f));
        TxtC(fontBold, std::to_string(f.floating.value), r.x + nodeW / 2, r.y + nodeW / 2 - 11, 21, textOn(t));
        Txt(fontUI, "new node, not linked yet", r.x + nodeW + 14, r.y + nodeW / 2 - 8, 14, C_TEXT2);
    }
}

// ============================================================================
//  Boxes: arrays, queues and deques (fixed slots, empty ones are drawn too)
// ============================================================================

void GUIApp::drawBoxes(const Frame& f, Rectangle area) {
    int n = (int)f.cells.size();
    if (n == 0) return;
    float boxW = clampf((area.width - 90) / n - 8, 34.0f, 84.0f);
    float gap = 8;
    float total = n * boxW + (n - 1) * gap;
    float startX = area.x + (area.width - total) / 2;
    float boxH = 70;
    float cy = area.y + area.height * 0.5f;
    float kc = 1.0f - std::exp(-dt * 16.0f);
    if ((int)cellAnims.size() < n) cellAnims.resize(n);

    for (int i = 0; i < n; i++) {
        CellAnim& a = cellAnims[i];
        const Cell& c = f.cells[i];
        if (!a.init) { a.tint = tintFor(c.role); a.init = true; }
        approach(a.tint, tintFor(c.role), kc);
        Rectangle r = { startX + i * (boxW + gap), cy - boxH / 2, boxW, boxH };
        if (c.empty) {
            Color outline = c.role == ROLE_NORMAL ? ColorAlpha(C_MUTED, 0.9f) : rgb(a.tint.top);
            if (c.role != ROLE_NORMAL) {
                float gm[3]; midOf(a.tint, gm);
                glowRect(r, 10, gm, a.tint.glow * 0.45f, 10);
                DrawRectangleRounded(r, 0.14f, 8, ColorAlpha(rgb(a.tint.bot), 0.22f));
            }
            dashedRounded(r, 0.14f, outline);
        }
        else {
            float gm[3]; midOf(a.tint, gm);
            glowRect(r, 10, gm, a.tint.glow, 4.0f + 14.0f * std::min(1.0f, a.tint.glow));
            gradRounded(r, 10, rgb(a.tint.top), rgb(a.tint.bot), true);
            DrawRectangleRoundedLinesEx(r, 0.14f, 8, 1.2f, ColorAlpha(WHITE, 0.22f));
            TxtC(fontBold, std::to_string(c.value), r.x + boxW / 2, cy - 12, boxW > 44 ? 22.0f : 17.0f, textOn(a.tint));
        }
        TxtC(fontUI, std::to_string(i), r.x + boxW / 2, r.y + boxH + 8, 13, C_MUTED);
        std::string mt = markerText(f, i);
        if (!mt.empty()) drawChip(this, fontBold, r.x + boxW / 2, r.y - 10, mt, a.tint.top);
    }
    for (int i = n; i < (int)cellAnims.size(); i++) cellAnims[i].init = false;

    if (!f.auxLabel.empty()) TxtC(fontUI, f.auxLabel, area.x + area.width / 2, cy + boxH / 2 + 38, 15, C_TEXT2);
    drawInfoLines(f, area.x + 28, area.y + 12, area.width - 60);
}

// ============================================================================
//  Stack: a vertical pile
// ============================================================================

void GUIApp::drawStack(const Frame& f, Rectangle area) {
    int n = (int)f.cells.size();
    bool chars = f.auxLabel == "chars";
    bool calls = f.auxLabel == "calls";
    float cellW = 190;
    float cellH = clampf((area.height - 110) / std::max(1, n), 24.0f, 46.0f);
    float cx = area.x + area.width * (f.info.empty() ? 0.5f : 0.68f);
    float baseY = area.y + area.height - 28;
    float kc = 1.0f - std::exp(-dt * 16.0f);
    if ((int)cellAnims.size() < n) cellAnims.resize(n);

    // base plate
    DrawRectangleGradientEx({ cx - cellW / 2 - 20, baseY + 4, cellW + 40, 4 }, ColorAlpha(WHITE, 0.0f), ColorAlpha(WHITE, 0.0f), ColorAlpha(WHITE, 0.0f), ColorAlpha(WHITE, 0.0f));
    DrawRectangleRounded({ cx - cellW / 2 - 20, baseY + 4, cellW + 40, 4 }, 1.0f, 4, ColorAlpha(WHITE, 0.18f));

    if (n == 0) {
        TxtC(fontBold, "The stack is empty", cx, baseY - 60, 22, C_TEXT2);
    }
    for (int i = 0; i < n; i++) {
        CellAnim& a = cellAnims[i];
        const Cell& c = f.cells[i];
        if (!a.init) { a.tint = tintFor(c.role); a.init = true; }
        approach(a.tint, tintFor(c.role), kc);
        Rectangle r = { cx - cellW / 2, baseY - (i + 1) * (cellH + 4) + 4, cellW, cellH };
        if (c.empty) {
            Color outline = c.role == ROLE_NORMAL ? ColorAlpha(C_MUTED, 0.8f) : rgb(a.tint.top);
            if (c.role != ROLE_NORMAL) {
                float gm[3]; midOf(a.tint, gm);
                glowRect(r, 8, gm, a.tint.glow * 0.4f, 8);
                DrawRectangleRounded(r, 0.25f, 8, ColorAlpha(rgb(a.tint.bot), 0.2f));
            }
            dashedRounded(r, 0.25f, outline);
        }
        else {
            float gm[3]; midOf(a.tint, gm);
            glowRect(r, 9, gm, a.tint.glow, 4.0f + 12.0f * std::min(1.0f, a.tint.glow));
            gradRounded(r, 9, rgb(a.tint.top), rgb(a.tint.bot), true);
            DrawRectangleRoundedLinesEx(r, 0.25f, 8, 1.2f, ColorAlpha(WHITE, 0.22f));
            std::string text = chars ? std::string(1, (char)c.value) : (calls ? "factorial(" + std::to_string(c.value) + ")" : std::to_string(c.value));
            TxtC(fontBold, text, cx, r.y + (cellH - 21) / 2 - 1, 21, textOn(a.tint));
        }
        std::string mt = markerText(f, i);
        if (!mt.empty()) {
            float w = TxtW(fontBold, mt, 13) + 16;
            Rectangle tag = { r.x + r.width + 14, r.y + (cellH - 22) / 2, w, 22 };
            DrawRectangleRounded(tag, 0.5f, 8, Color{ 12, 14, 20, 235 });
            DrawRectangleRoundedLinesEx(tag, 0.5f, 8, 1.2f, rgb(a.tint.top));
            Txt(fontBold, mt, tag.x + 8, tag.y + 3, 13, lighten(a.tint.top, 0.25f));
            DrawTriangle({ tag.x - 1, tag.y + 11 }, { tag.x - 8, tag.y + 6 }, { tag.x - 8, tag.y + 16 }, rgb(a.tint.top));
        }
    }
    for (int i = n; i < (int)cellAnims.size(); i++) cellAnims[i].init = false;
    drawInfoLines(f, area.x + 28, area.y + 14, area.width * 0.52f);
}

// ============================================================================
//  Graph: vertices on a circle
// ============================================================================

void GUIApp::drawGraph(const Frame& f, Rectangle area) {
    int n = (int)f.vertices.size();
    if (n == 0) {
        TxtC(fontBold, "The graph has no vertices", area.x + area.width / 2, area.y + area.height / 2, 22, C_TEXT2);
        return;
    }
    float cx = area.x + area.width / 2;
    float cy = area.y + area.height * 0.47f;
    float R = std::min(area.width * 0.30f, area.height * 0.31f);
    float vr = 23;
    float kc = 1.0f - std::exp(-dt * 16.0f);
    float k = 1.0f - std::exp(-dt * 10.0f);

    std::vector<Vector2> pos(n);
    for (int i = 0; i < n; i++) {
        float ang = -3.14159265f / 2 + 6.2831853f * i / n;
        pos[i] = { cx + std::cos(ang) * R, cy + std::sin(ang) * R };
    }
    for (int i = 0; i < n; i++) {
        auto it = vertexAnims.find(i);
        if (it == vertexAnims.end()) {
            NodeAnim a;
            a.x = pos[i].x; a.y = pos[i].y; a.scale = 0.3f; a.alpha = 1; a.tint = tintFor(f.vertices[i].role);
            it = vertexAnims.emplace(i, a).first;
        }
        NodeAnim& a = it->second;
        a.x += (pos[i].x - a.x) * k;
        a.y += (pos[i].y - a.y) * k;
        a.scale += (1.0f - a.scale) * k;
        approach(a.tint, tintFor(f.vertices[i].role), kc);
    }

    // edges
    for (const GraphEdge& e : f.edges) {
        if (e.u >= n || e.v >= n) continue;
        Vector2 a = { vertexAnims[e.u].x, vertexAnims[e.u].y };
        Vector2 b = { vertexAnims[e.v].x, vertexAnims[e.v].y };
        Tint t = tintFor(e.role);
        float dx = b.x - a.x, dy = b.y - a.y;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len < 1) continue;
        float ux = dx / len, uy = dy / len;
        Vector2 a2 = { a.x + ux * vr, a.y + uy * vr };
        Vector2 b2 = { b.x - ux * (vr + (f.directed ? 2 : 0)), b.y - uy * (vr + (f.directed ? 2 : 0)) };
        bool plain = e.role == ROLE_NORMAL;
        Color lc = plain ? ColorAlpha(Color{ 170, 185, 220, 255 }, 0.30f) : rgb(t.top, 235);
        if (!plain) {
            float gm[3]; midOf(t, gm);
            BeginBlendMode(BLEND_ADDITIVE);
            DrawLineEx(a2, b2, 9.0f, rgb(gm, 30));
            DrawLineEx(a2, b2, 5.0f, rgb(gm, 50));
            EndBlendMode();
        }
        DrawLineEx(a2, b2, plain ? 2.4f : 3.6f, lc);
        if (f.directed) arrowHead(b2, { b2.x - ux * 14, b2.y - uy * 14 }, 12, plain ? ColorAlpha(Color{ 200, 210, 235, 255 }, 0.7f) : lc);
        // weight chip
        Vector2 mid = { (a.x + b.x) / 2 - uy * 11, (a.y + b.y) / 2 + ux * 11 };
        std::string w = std::to_string(e.weight);
        float cw = TxtW(fontBold, w, 12) + 12;
        Rectangle chip = { mid.x - cw / 2, mid.y - 9, cw, 18 };
        DrawRectangleRounded(chip, 0.5f, 8, Color{ 12, 14, 20, 240 });
        DrawRectangleRoundedLinesEx(chip, 0.5f, 8, 1.0f, plain ? ColorAlpha(WHITE, 0.15f) : rgb(t.top, 200));
        TxtC(fontBold, w, mid.x, chip.y + 2, 12, plain ? C_TEXT2 : lighten(t.top, 0.3f));
    }
    // vertices
    for (int i = 0; i < n; i++) {
        NodeAnim& a = vertexAnims[i];
        float r = vr * a.scale;
        float gm[3]; midOf(a.tint, gm);
        glowCircle(a.x, a.y, r, gm, a.tint.glow, 6.0f + 16.0f * std::min(1.0f, a.tint.glow));
        gradCircleH(a.x, a.y, r, rgb(a.tint.top), rgb(a.tint.bot));
        DrawRing({ a.x, a.y }, r - 1.4f, r, 0, 360, 40, ColorAlpha(WHITE, 0.28f));
        TxtC(fontBold, std::to_string(i), a.x, a.y - 10, 20, textOn(a.tint));
    }
    for (const Marker& m : f.markers) {
        if (m.index < 0 || m.index >= n) continue;
        NodeAnim& a = vertexAnims[m.index];
        Vector2 dir = { a.x - cx, a.y - cy };
        float dl = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (dl < 1) dl = 1;
        float tx = a.x + dir.x / dl * (vr + 20);
        float ty = a.y + dir.y / dl * (vr + 20);
        float w = TxtW(fontBold, m.label, 12) + 14;
        Rectangle tag = { tx - w / 2, ty - 10, w, 20 };
        DrawRectangleRounded(tag, 0.5f, 8, Color{ 12, 14, 20, 235 });
        DrawRectangleRoundedLinesEx(tag, 0.5f, 8, 1.0f, ColorAlpha(C_ACCENT, 0.8f));
        TxtC(fontBold, m.label, tx, tag.y + 2, 12, C_ACCENT);
    }

    // queue / stack / order strip
    float sy = area.y + area.height - 52;
    if (!f.auxLabel.empty()) {
        Txt(fontBold, f.auxLabel, area.x + 28, sy + 9, 14, C_TEXT2);
        float bx = area.x + 28 + TxtW(fontBold, f.auxLabel, 14) + 18;
        if (f.cells.empty()) Txt(fontUI, "(empty)", bx, sy + 9, 14, C_MUTED);
        for (size_t i = 0; i < f.cells.size(); i++) {
            Rectangle r = { bx + i * 42.0f, sy, 36, 36 };
            Tint t = tintFor(ROLE_NORMAL);
            gradRounded(r, 8, rgb(t.top), rgb(t.bot), true);
            TxtC(fontBold, std::to_string(f.cells[i].value), r.x + 18, r.y + 8, 17, WHITE);
        }
    }
    drawInfoLines(f, area.x + 28, area.y + 12, area.width * 0.55f);
}

// ============================================================================
//  Hash table: one row per slot
// ============================================================================

void GUIApp::drawHash(const Frame& f, Rectangle area) {
    int n = (int)f.cells.size();
    if (n == 0) return;
    bool chaining = (int)f.chains.size() == n;
    float rowH = clampf((area.height - 24) / n, 22.0f, 36.0f);
    float x0 = area.x + area.width * 0.10f;
    float y0 = area.y + (area.height - n * rowH) / 2;
    float kc = 1.0f - std::exp(-dt * 16.0f);
    if ((int)cellAnims.size() < n) cellAnims.resize(n);

    for (int i = 0; i < n; i++) {
        CellAnim& a = cellAnims[i];
        const Cell& c = f.cells[i];
        if (!a.init) { a.tint = tintFor(c.role); a.init = true; }
        approach(a.tint, tintFor(c.role), kc);
        float y = y0 + i * rowH;
        Rectangle idx = { x0, y + 2, 46, rowH - 4 };
        bool lit = c.role != ROLE_NORMAL;
        if (lit && (chaining || c.empty)) {
            float gm[3]; midOf(a.tint, gm);
            glowRect(idx, 8, gm, a.tint.glow * 0.7f, 8);
            gradRounded(idx, 8, rgb(a.tint.top), rgb(a.tint.bot), true);
            TxtC(fontBold, std::to_string(i), idx.x + 23, idx.y + (idx.height - 16) / 2 - 1, 16, textOn(a.tint));
        }
        else {
            DrawRectangleRounded(idx, 0.3f, 8, C_PANEL2);
            DrawRectangleRoundedLinesEx(idx, 0.3f, 8, 1.0f, ColorAlpha(WHITE, 0.1f));
            TxtC(fontBold, std::to_string(i), idx.x + 23, idx.y + (idx.height - 16) / 2 - 1, 16, C_TEXT2);
        }
        if (chaining) {
            float x = idx.x + idx.width + 18;
            DrawLineEx({ idx.x + idx.width + 2, idx.y + idx.height / 2 }, { x - 4, idx.y + idx.height / 2 }, 2.0f, ColorAlpha(C_TEXT2, 0.6f));
            if (f.chains[i].empty()) {
                Txt(fontCode, "NULL", x + 2, idx.y + (idx.height - 14) / 2, 14, C_MUTED);
            }
            for (size_t k = 0; k < f.chains[i].size(); k++) {
                const Cell& cc = f.chains[i][k];
                Tint t = tintFor(cc.role);
                Rectangle nr = { x, idx.y, 58, idx.height };
                float gm[3]; midOf(t, gm);
                if (cc.role != ROLE_NORMAL) glowRect(nr, 8, gm, t.glow * 0.8f, 9);
                gradRounded(nr, 8, rgb(t.top), rgb(t.bot), true);
                TxtC(fontBold, std::to_string(cc.value), nr.x + 29, nr.y + (nr.height - 16) / 2 - 1, 16, textOn(t));
                x += 58;
                float ax1 = x + 24;
                DrawLineEx({ x + 2, nr.y + nr.height / 2 }, { ax1 - 4, nr.y + nr.height / 2 }, 2.0f, ColorAlpha(C_TEXT2, 0.6f));
                arrowHead({ ax1, nr.y + nr.height / 2 }, { ax1 - 12, nr.y + nr.height / 2 }, 9, ColorAlpha(C_TEXT2, 0.8f));
                x = ax1 + 4;
            }
            if (!f.chains[i].empty()) Txt(fontCode, "NULL", x + 2, idx.y + (idx.height - 14) / 2, 14, C_MUTED);
        }
        else {
            Rectangle sr = { idx.x + idx.width + 10, idx.y, 170, idx.height };
            bool deleted = false;
            for (auto& m : f.markers) if (m.index == i && m.label == "deleted") deleted = true;
            if (c.empty) {
                if (lit) {
                    float gm[3]; midOf(a.tint, gm);
                    glowRect(sr, 8, gm, a.tint.glow * 0.4f, 8);
                    DrawRectangleRounded(sr, 0.3f, 8, ColorAlpha(rgb(a.tint.bot), 0.25f));
                }
                dashedRounded(sr, 0.3f, lit ? rgb(a.tint.top) : ColorAlpha(C_MUTED, 0.9f));
                if (deleted) TxtC(fontCode, "deleted", sr.x + sr.width / 2, sr.y + (sr.height - 14) / 2, 14, ColorAlpha(Color{ 255, 90, 120, 255 }, 0.8f));
            }
            else {
                float gm[3]; midOf(a.tint, gm);
                if (lit) glowRect(sr, 8, gm, a.tint.glow * 0.8f, 10);
                gradRounded(sr, 8, rgb(a.tint.top), rgb(a.tint.bot), true);
                DrawRectangleRoundedLinesEx(sr, 0.3f, 8, 1.0f, ColorAlpha(WHITE, 0.2f));
                TxtC(fontBold, std::to_string(c.value), sr.x + sr.width / 2, sr.y + (sr.height - 17) / 2 - 1, 17, textOn(a.tint));
            }
        }
    }
    for (int i = n; i < (int)cellAnims.size(); i++) cellAnims[i].init = false;

    float rx = area.x + area.width * 0.62f;
    if (!f.auxLabel.empty()) {
        Txt(fontBold, "TABLE", rx, area.y + 20, 12, C_MUTED);
        auto lines = wrapText([&](const std::string& t) { return TxtW(fontUI, t, 15); }, f.auxLabel, area.x + area.width - rx - 24);
        for (size_t i = 0; i < lines.size(); i++) Txt(fontUI, lines[i], rx, area.y + 40 + i * 20, 15, C_TEXT2);
    }
    if (!f.info.empty()) {
        Txt(fontBold, "HASH FUNCTION", rx, area.y + 100, 12, C_MUTED);
        drawInfoLines(f, rx, area.y + 120, area.x + area.width - rx - 24);
    }
}

// ============================================================================
//  Towers of Hanoi
// ============================================================================

void GUIApp::drawHanoi(const Frame& f, Rectangle area) {
    if (f.chains.size() < 3) return;
    float colW = area.width / 3;
    float baseY = area.y + area.height - 34;
    float diskH = 28;
    int maxDisk = 1;
    for (auto& peg : f.chains) for (auto& c : peg) maxDisk = std::max(maxDisk, c.value);
    float rodH = std::min(area.height - 60, (maxDisk + 1) * (diskH + 3) + 20);
    float k = 1.0f - std::exp(-dt * 9.0f);
    float kc = 1.0f - std::exp(-dt * 16.0f);
    const char* names[3] = { "A", "B", "C" };

    for (int p = 0; p < 3; p++) {
        float cx = area.x + colW * (p + 0.5f);
        DrawRectangleRounded({ cx - 5, baseY - rodH, 10, rodH }, 0.5f, 6, ColorAlpha(WHITE, 0.14f));
        DrawRectangleRounded({ cx - colW * 0.42f, baseY, colW * 0.84f, 8 }, 0.5f, 6, ColorAlpha(WHITE, 0.2f));
        TxtC(fontBold, names[p], cx, baseY + 14, 17, C_TEXT2);
    }
    std::unordered_map<int, bool> seen;
    for (int p = 0; p < 3; p++) {
        float cx = area.x + colW * (p + 0.5f);
        for (size_t i = 0; i < f.chains[p].size(); i++) {
            const Cell& c = f.chains[p][i];
            float ty = baseY - (i + 1) * (diskH + 3) + 3;
            auto it = nodeAnims.find(c.value);
            if (it == nodeAnims.end()) {
                NodeAnim a;
                a.x = cx; a.y = ty; a.scale = 1; a.alpha = 1; a.tint = tintFor(c.role);
                it = nodeAnims.emplace(c.value, a).first;
            }
            NodeAnim& a = it->second;
            a.x += (cx - a.x) * k;
            a.y += (ty - a.y) * k;
            // disks that are not highlighted keep their own color by size
            static const Role palette[5] = { ROLE_NORMAL, ROLE_FOUND, ROLE_CURRENT, ROLE_KEPT, ROLE_DONE };
            Role shown = c.role == ROLE_NORMAL ? palette[(c.value - 1) % 5] : c.role;
            approach(a.tint, tintFor(shown), kc);
            seen[c.value] = true;
        }
    }
    // draw big disks first
    std::vector<int> order;
    for (auto& kv : seen) order.push_back(kv.first);
    std::sort(order.begin(), order.end(), std::greater<int>());
    for (int size : order) {
        NodeAnim& a = nodeAnims[size];
        float w = 56 + size * 34;
        Rectangle r = { a.x - w / 2, a.y, w, diskH };
        float gm[3]; midOf(a.tint, gm);
        glowRect(r, 12, gm, a.tint.glow * 0.7f + 0.2f, 10);
        gradRounded(r, 12, rgb(a.tint.top), rgb(a.tint.bot), true);
        DrawRectangleRoundedLinesEx(r, 0.5f, 10, 1.2f, ColorAlpha(WHITE, 0.25f));
        TxtC(fontBold, std::to_string(size), a.x, r.y + 3, 18, textOn(a.tint));
    }
}

// ============================================================================
//  Time complexity: growth curves on a logarithmic scale
// ============================================================================

static std::string bigText(long long v) {
    if (v < 0) return "too big";
    std::string s = std::to_string(v);
    if (v >= 1000000000000LL) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%.1fe%d", (double)v / std::pow(10.0, (double)(s.size() - 1)), (int)s.size() - 1);
        return buf;
    }
    for (int i = (int)s.size() - 3; i > 0; i -= 3) s.insert(i, ",");
    return s;
}

void GUIApp::drawComplexity(Rectangle area) {
    int n = std::max(1, S().index + 1);
    const int NMAX = 64;
    struct Curve { ComplexityKind kind; Role role; };
    const Curve curves[7] = {
        { CONSTANT, ROLE_DONE }, { LOGARITHMIC, ROLE_NEW }, { LINEAR, ROLE_NORMAL }, { LINEARITHMIC, ROLE_CURRENT },
        { QUADRATIC, ROLE_REMOVED }, { CUBIC, ROLE_KEPT }, { EXPONENTIAL, ROLE_FOUND }
    };
    Rectangle plot = { area.x + 80, area.y + 22, area.width - 80 - 290, area.height - 62 };
    const double decades = 12.0;

    // grid and axis labels
    for (int d = 0; d <= 12; d += 2) {
        float y = plot.y + plot.height - (float)(d / decades) * plot.height;
        DrawLineEx({ plot.x, y }, { plot.x + plot.width, y }, 1.0f, ColorAlpha(WHITE, d == 0 ? 0.25f : 0.07f));
        std::string label = d == 0 ? "1" : (d == 2 ? "100" : (d == 4 ? "10,000" : "1e" + std::to_string(d)));
        Txt(fontCode, label, plot.x - 12 - TxtW(fontCode, label, 13), y - 8, 13, C_MUTED);
    }
    for (int x = 0; x <= NMAX; x += 8) {
        float px = plot.x + (float)x / NMAX * plot.width;
        DrawLineEx({ px, plot.y }, { px, plot.y + plot.height }, 1.0f, ColorAlpha(WHITE, 0.05f));
        TxtC(fontCode, std::to_string(x), px, plot.y + plot.height + 8, 13, C_MUTED);
    }
    Txt(fontUI, "input size n", plot.x + plot.width / 2 - 36, plot.y + plot.height + 26, 13, C_TEXT2);
    Txt(fontUI, "steps (log scale)", area.x + 24, plot.y + plot.height + 26, 13, C_TEXT2);

    BeginScissorMode((int)plot.x, (int)plot.y - 4, (int)plot.width + 6, (int)plot.height + 8);
    for (const Curve& c : curves) {
        Tint t = tintFor(c.role);
        float gm[3]; midOf(t, gm);
        Vector2 prev = { 0, 0 };
        bool havePrev = false;
        for (int x = 1; x <= NMAX; x++) {
            long long steps = stepsFor(c.kind, x);
            double dec = steps < 0 ? 14.0 : std::log10((double)steps);
            float px = plot.x + (float)x / NMAX * plot.width;
            float py = plot.y + plot.height - (float)(dec / decades) * plot.height;
            if (havePrev) {
                bool reached = x <= n;
                Color col = ColorLerp(rgb(t.top), rgb(t.bot), (float)x / NMAX);
                if (reached) {
                    BeginBlendMode(BLEND_ADDITIVE);
                    DrawLineEx(prev, { px, py }, 9.0f, rgb(gm, 22));
                    EndBlendMode();
                }
                DrawLineEx(prev, { px, py }, reached ? 3.2f : 1.6f, ColorAlpha(col, reached ? 1.0f : 0.22f));
            }
            prev = { px, py };
            havePrev = true;
        }
        long long steps = stepsFor(c.kind, n);
        double dec = steps < 0 ? 14.0 : std::log10((double)steps);
        float px = plot.x + (float)n / NMAX * plot.width;
        float py = plot.y + plot.height - (float)(dec / decades) * plot.height;
        glowCircle(px, py, 5, gm, 1.0f, 12);
        DrawCircleV({ px, py }, 5.5f, WHITE);
    }
    // marker line for the current n
    float nx = plot.x + (float)n / NMAX * plot.width;
    DrawLineEx({ nx, plot.y }, { nx, plot.y + plot.height }, 1.2f, ColorAlpha(C_ACCENT, 0.5f));
    EndScissorMode();

    // legend table with the real numbers for this n
    float tx = plot.x + plot.width + 26;
    Txt(fontBold, "STEPS FOR n = " + std::to_string(n), tx, area.y + 16, 12, C_MUTED);
    float ty = area.y + 44;
    for (const Curve& c : curves) {
        Tint t = tintFor(c.role);
        float gm[3]; midOf(t, gm);
        Rectangle sw = { tx, ty + 2, 30, 14 };
        glowRect(sw, 7, gm, 0.6f, 6);
        gradRounded(sw, 7, rgb(t.top), rgb(t.bot), true);
        Txt(fontCode, complexityName(c.kind), tx + 42, ty, 15, C_TEXT);
        std::string v = bigText(stepsFor(c.kind, n));
        Txt(fontCode, v, tx + 42, ty + 20, 14, ColorAlpha(rgb(t.top), 1.0f));
        ty += 50;
    }
}

void GUIApp::drawTree(const Frame& f, Rectangle area) {
    bool isHeap = (view == V_HEAP);
    float k = 1.0f - std::exp(-dt * 11.0f);
    float kc = 1.0f - std::exp(-dt * 16.0f);

    for (auto& kv : nodeAnims) kv.second.seen = false;

    if (f.nodes.empty()) {
        for (auto& kv : nodeAnims) kv.second.alpha -= dt * 4;
        TxtC(fontBold, view == V_HEAP ? "The heap is empty" : "The tree is empty", area.x + area.width / 2, area.y + area.height / 2 - 26, 24, C_TEXT2);
        TxtC(fontUI, "Type a number and press Insert (or hit Enter).", area.x + area.width / 2, area.y + area.height / 2 + 12, 16, C_MUTED);
        for (auto it = nodeAnims.begin(); it != nodeAnims.end();) {
            if (it->second.alpha <= 0.02f) it = nodeAnims.erase(it); else ++it;
        }
        return;
    }

    std::unordered_map<int, const TreeNodeInfo*> byId;
    for (auto& nd : f.nodes) byId[nd.id] = &nd;

    // target positions
    std::unordered_map<int, Vector2> target;
    float treeBottom = area.y + area.height;
    float arrayStripH = isHeap ? 74.0f : 0.0f;
    float usableH = area.height - 36 - arrayStripH;
    float radius = 22;
    if (isHeap) {
        int maxDepth = 0;
        for (auto& nd : f.nodes) maxDepth = std::max(maxDepth, (int)std::floor(std::log2((double)nd.id + 1)));
        float dy = std::min(88.0f, usableH / std::max(1, maxDepth + 1));
        for (auto& nd : f.nodes) {
            int d = (int)std::floor(std::log2((double)nd.id + 1));
            int idx = nd.id + 1 - (1 << d);
            float x = area.x + 50 + (idx + 0.5f) / (float)(1 << d) * (area.width - 100);
            target[nd.id] = { x, area.y + 40 + d * dy };
        }
        radius = clampf(std::min(24.0f, (area.width - 100) / (float)(1 << std::min(maxDepth, 4)) * 0.36f), 15.0f, 24.0f);
    }
    else {
        int counter = 0, maxDepth = 0;
        std::unordered_map<int, std::pair<int, int>> pos;
        std::function<void(int, int)> rec = [&](int id, int depth) {
            auto it = byId.find(id);
            if (it == byId.end()) return;
            rec(it->second->left, depth + 1);
            pos[id] = { counter++, depth };
            maxDepth = std::max(maxDepth, depth);
            rec(it->second->right, depth + 1);
        };
        rec(f.rootId, 0);
        float dx = std::min(74.0f, (area.width - 100) / std::max(1, counter - 1));
        float dy = std::min(88.0f, usableH / std::max(1, maxDepth + 1));
        float x0 = area.x + area.width / 2 - (counter - 1) * dx / 2;
        for (auto& p : pos) target[p.first] = { x0 + p.second.first * dx, area.y + 40 + p.second.second * dy };
        radius = clampf(std::min(24.0f, dx * 0.42f), 14.0f, 24.0f);
    }

    // update animations
    for (auto& nd : f.nodes) {
        auto t = target.find(nd.id);
        if (t == target.end()) continue;
        auto it = nodeAnims.find(nd.id);
        if (it == nodeAnims.end()) {
            NodeAnim a;
            a.x = t->second.x;
            a.y = t->second.y;
            a.scale = 0.2f;
            a.alpha = 0.0f;
            a.tint = tintFor(nd.role);
            it = nodeAnims.emplace(nd.id, a).first;
        }
        NodeAnim& a = it->second;
        a.seen = true;
        a.value = nd.value;
        a.x += (t->second.x - a.x) * k;
        a.y += (t->second.y - a.y) * k;
        a.scale += (1.0f - a.scale) * k;
        a.alpha += (1.0f - a.alpha) * k;
        approach(a.tint, tintFor(nd.role), kc);
    }
    for (auto it = nodeAnims.begin(); it != nodeAnims.end();) {
        NodeAnim& a = it->second;
        if (!a.seen) { a.alpha -= dt * 4.5f; a.scale += (0.6f - a.scale) * k; }
        if (a.alpha <= 0.02f && !a.seen) it = nodeAnims.erase(it); else ++it;
    }

    // edges
    for (auto& nd : f.nodes) {
        auto p = nodeAnims.find(nd.id);
        if (p == nodeAnims.end()) continue;
        for (int child : { nd.left, nd.right }) {
            if (child < 0) continue;
            auto c = nodeAnims.find(child);
            if (c == nodeAnims.end()) continue;
            float a = std::min(p->second.alpha, c->second.alpha);
            DrawLineEx({ p->second.x, p->second.y }, { c->second.x, c->second.y }, 2.2f, ColorAlpha(Color{ 170, 185, 220, 255 }, 0.22f * a));
        }
    }
    // nodes (ghosts first so they sit behind)
    auto drawNode = [&](const NodeAnim& a, int value, const TreeNodeInfo* info) {
        float r = radius * a.scale;
        if (r < 1) return;
        float gm[3]; midOf(a.tint, gm);
        glowCircle(a.x, a.y, r, gm, a.tint.glow * a.alpha, 6.0f + 16.0f * std::min(1.0f, a.tint.glow));
        gradCircleH(a.x, a.y, r, ColorAlpha(rgb(a.tint.top), a.alpha), ColorAlpha(rgb(a.tint.bot), a.alpha));
        DrawRing({ a.x, a.y }, r - 1.4f, r, 0, 360, 40, ColorAlpha(WHITE, 0.28f * a.alpha));
        if (a.alpha > 0.3f) {
            std::string v = std::to_string(value);
            float fs = r * (v.size() > 2 ? 0.72f : 0.85f);
            TxtC(fontBold, v, a.x, a.y - fs * 0.62f, fs, textOn(a.tint, 255.0f * a.alpha));
        }
        if (info && info->hasAux && a.alpha > 0.5f) {
            if (f.auxLabel == "fib") {
                std::string t = "= " + std::to_string(info->aux);
                float w = TxtW(fontBold, t, 12) + 12;
                Rectangle br = { a.x - w / 2, a.y + r + 5, w, 18 };
                DrawRectangleRounded(br, 0.5f, 8, Color{ 12, 14, 20, 230 });
                DrawRectangleRoundedLinesEx(br, 0.5f, 8, 1.0f, ColorAlpha(C_ACCENT, 0.6f));
                TxtC(fontBold, t, a.x, br.y + 2, 12, C_ACCENT);
            }
            else if (view == V_AVL) {
                int bf = info->aux;
                std::string t = std::string(bf > 0 ? "+" : "") + std::to_string(bf);
                Color c = (bf > 1 || bf < -1) ? Color{ 255, 116, 144, 255 } : C_TEXT2;
                float w = TxtW(fontBold, t, 12) + 12;
                Rectangle br = { a.x - w / 2, a.y + r + 5, w, 18 };
                DrawRectangleRounded(br, 0.5f, 8, Color{ 12, 14, 20, 230 });
                DrawRectangleRoundedLinesEx(br, 0.5f, 8, 1.0f, ColorAlpha(c, 0.55f));
                TxtC(fontBold, t, a.x, br.y + 2, 12, c);
            }
        }
    };
    for (auto& kv : nodeAnims) if (!kv.second.seen) drawNode(kv.second, kv.second.value, nullptr);
    for (auto& nd : f.nodes) {
        auto it = nodeAnims.find(nd.id);
        if (it != nodeAnims.end()) drawNode(it->second, nd.value, &nd);
    }

    // heap: the same values as an array
    if (isHeap) {
        int n = (int)f.nodes.size();
        std::vector<const TreeNodeInfo*> ordered(n, nullptr);
        for (auto& nd : f.nodes) if (nd.id >= 0 && nd.id < n) ordered[nd.id] = &nd;
        float cw = std::min(40.0f, (area.width - 100) / std::max(1, n));
        float x0 = area.x + area.width / 2 - cw * n / 2;
        float y = treeBottom - 56;
        Txt(fontUI, "as an array", x0 - 84, y + 6, 13, C_MUTED);
        for (int i = 0; i < n; i++) {
            if (!ordered[i]) continue;
            Tint t = tintFor(ordered[i]->role);
            Rectangle r = { x0 + i * cw + 1, y, cw - 2, 30 };
            DrawRectangleRounded(r, 0.25f, 6, ColorAlpha(rgb(t.bot), ordered[i]->role == ROLE_NORMAL ? 0.45f : 0.85f));
            DrawRectangleRoundedLinesEx(r, 0.25f, 6, 1.0f, ColorAlpha(rgb(t.top), 0.6f));
            TxtC(fontBold, std::to_string(ordered[i]->value), r.x + r.width / 2, r.y + 6, cw > 30 ? 15 : 12, textOn(t));
            TxtC(fontUI, std::to_string(i), r.x + r.width / 2, r.y + 33, 11, C_MUTED);
        }
    }
}

