#include "GUIDraw.h"
#include <algorithm>
#include <cmath>

namespace gfx {

const Color BG      = { 11, 13, 18, 255 };
const Color HEADER  = { 15, 17, 24, 255 };
const Color PANEL   = { 19, 22, 31, 255 };
const Color PANEL2  = { 26, 30, 41, 255 };
const Color LINE    = { 255, 255, 255, 20 };
const Color TEXT    = { 234, 237, 245, 255 };
const Color TEXT2   = { 154, 163, 183, 255 };
const Color MUTED   = { 98, 107, 129, 255 };
const Color ACCENT  = { 112, 170, 255, 255 };

// Every state is a left-to-right gradient (top = left end, bot = right end), like glossy pills.
Tint tintFor(Role r) {
    switch (r) {
        case ROLE_CURRENT: return { { 255, 244, 0 },   { 255, 150, 0 },  1.00f };  // checking: yellow to orange
        case ROLE_NEW:     return { { 0, 255, 72 },    { 214, 255, 0 },  1.10f };  // condition met / swapped: green to lime
        case ROLE_KEPT:    return { { 255, 0, 170 },   { 255, 40, 70 },  0.90f };  // condition not met: pink to red
        case ROLE_FOUND:   return { { 255, 0, 255 },   { 110, 0, 255 },  1.00f };  // pivot / key / found: magenta to violet
        case ROLE_DONE:    return { { 30, 255, 214 },  { 0, 190, 150 },  0.60f };  // finished: aqua to teal
        case ROLE_REMOVED: return { { 255, 130, 0 },   { 255, 0, 50 },   1.00f };  // about to leave: orange to red
        case ROLE_DIM:     return { { 76, 86, 112 },   { 48, 56, 80 },   0.00f };  // ruled out: dim slate
        default:           return { { 0, 236, 255 },   { 22, 66, 255 },  0.30f };  // idle: cyan to blue
    }
}

void approach(Tint& a, const Tint& b, float k) {
    for (int i = 0; i < 3; i++) {
        a.top[i] += (b.top[i] - a.top[i]) * k;
        a.bot[i] += (b.bot[i] - a.bot[i]) * k;
    }
    a.glow += (b.glow - a.glow) * k;
}

// middle color of a tint, used for glows and to decide between dark and white text
void midOf(const Tint& t, float* out) {
    for (int i = 0; i < 3; i++) out[i] = (t.top[i] + t.bot[i]) * 0.5f;
}

Color textOn(const Tint& t, float alpha) {
    float m[3];
    midOf(t, m);
    float lum = 0.299f * m[0] + 0.587f * m[1] + 0.114f * m[2];
    return lum > 150 ? Color{ 16, 18, 28, (unsigned char)alpha } : Color{ 255, 255, 255, (unsigned char)alpha };
}

Color rgb(const float* c, float a) {
    auto cl = [](float v) { return (unsigned char)std::max(0.0f, std::min(255.0f, v)); };
    return Color{ cl(c[0]), cl(c[1]), cl(c[2]), cl(a) };
}

Color lighten(const float* c, float amount, float a) {
    float t[3] = { c[0] + (255 - c[0]) * amount, c[1] + (255 - c[1]) * amount, c[2] + (255 - c[2]) * amount };
    return rgb(t, a);
}

// soft glow made of many additive layers (looks like a blurred halo on the dark background)
void glowRect(Rectangle r, float radius, const float* c, float intensity, float spread) {
    if (intensity < 0.02f) return;
    BeginBlendMode(BLEND_ADDITIVE);
    const int N = 10;
    for (int k = N; k >= 1; k--) {
        float s = spread * (float)k / N;
        float a = intensity * 0.075f * (1.0f - (float)k / (N + 1));
        Rectangle g = { r.x - s, r.y - s, r.width + 2 * s, r.height + 2 * s };
        float rad = (radius + s) * 2.0f / std::min(g.width, g.height);
        DrawRectangleRounded(g, std::min(1.0f, rad), 10, rgb(c, a * 255.0f));
    }
    EndBlendMode();
}

void glowCircle(float cx, float cy, float radius, const float* c, float intensity, float spread) {
    if (intensity < 0.02f) return;
    BeginBlendMode(BLEND_ADDITIVE);
    const int N = 10;
    for (int k = N; k >= 1; k--) {
        float s = spread * (float)k / N;
        float a = intensity * 0.075f * (1.0f - (float)k / (N + 1));
        DrawCircleV({ cx, cy }, radius + s, rgb(c, a * 255.0f));
    }
    EndBlendMode();
}

// left-to-right gradient with rounded corners (like the pills in the design picture)
void gradRounded(Rectangle r, float rad, Color left, Color right, bool roundBottom) {
    rad = std::min(rad, std::min(r.width, r.height) / 2.0f);
    float t = rad / r.width;
    Color l2 = ColorLerp(left, right, t);
    Color r2 = ColorLerp(left, right, 1.0f - t);
    DrawRectangleGradientEx({ r.x + rad, r.y, r.width - 2 * rad, r.height }, l2, l2, r2, r2);
    float lowRad = roundBottom ? rad : 0.0f;
    DrawRectangleGradientEx({ r.x, r.y + rad, rad, r.height - rad - lowRad }, left, left, l2, l2);
    DrawRectangleGradientEx({ r.x + r.width - rad, r.y + rad, rad, r.height - rad - lowRad }, r2, r2, right, right);
    DrawCircleSector({ r.x + rad, r.y + rad }, rad, 180, 270, 14, ColorLerp(left, l2, 0.5f));
    DrawCircleSector({ r.x + r.width - rad, r.y + rad }, rad, 270, 360, 14, ColorLerp(r2, right, 0.5f));
    if (roundBottom) {
        DrawCircleSector({ r.x + rad, r.y + r.height - rad }, rad, 90, 180, 14, ColorLerp(left, l2, 0.5f));
        DrawCircleSector({ r.x + r.width - rad, r.y + r.height - rad }, rad, 0, 90, 14, ColorLerp(r2, right, 0.5f));
    }
}

// circle filled with a left-to-right gradient (thin vertical strips)
void gradCircleH(float cx, float cy, float rad, Color left, Color right) {
    if (rad < 1) return;
    const float step = 1.0f;
    for (float dx = -rad; dx < rad; dx += step) {
        float mx = dx + step / 2;
        float h = std::sqrt(std::max(0.0f, rad * rad - mx * mx));
        Color c = ColorLerp(left, right, (mx + rad) / (2 * rad));
        DrawRectangleRec({ cx + dx, cy - h, step + 0.35f, h * 2 }, c);
    }
}

// dashed outline for empty slots
void dashedRounded(Rectangle r, float roundness, Color c) {
    float rad = std::min(r.width, r.height) * roundness / 2.0f;
    Color col = ColorAlpha(c, 0.75f);
    const float dash = 7, gap = 5;
    auto dashed = [&](Vector2 p, Vector2 q) {
        float dx = q.x - p.x, dy = q.y - p.y;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len < 1) return;
        dx /= len;
        dy /= len;
        for (float t = 0; t < len; t += dash + gap) {
            float e = std::min(len, t + dash);
            DrawLineEx({ p.x + dx * t, p.y + dy * t }, { p.x + dx * e, p.y + dy * e }, 1.6f, col);
        }
    };
    dashed({ r.x + rad, r.y }, { r.x + r.width - rad, r.y });
    dashed({ r.x + rad, r.y + r.height }, { r.x + r.width - rad, r.y + r.height });
    dashed({ r.x, r.y + rad }, { r.x, r.y + r.height - rad });
    dashed({ r.x + r.width, r.y + rad }, { r.x + r.width, r.y + r.height - rad });
    DrawRing({ r.x + rad, r.y + rad }, rad - 0.8f, rad + 0.8f, 180, 270, 8, col);
    DrawRing({ r.x + r.width - rad, r.y + rad }, rad - 0.8f, rad + 0.8f, 270, 360, 8, col);
    DrawRing({ r.x + rad, r.y + r.height - rad }, rad - 0.8f, rad + 0.8f, 90, 180, 8, col);
    DrawRing({ r.x + r.width - rad, r.y + r.height - rad }, rad - 0.8f, rad + 0.8f, 0, 90, 8, col);
}

void arrowHead(Vector2 tip, Vector2 from, float size, Color c) {
    float dx = tip.x - from.x, dy = tip.y - from.y;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f) return;
    dx /= len;
    dy /= len;
    Vector2 base = { tip.x - dx * size, tip.y - dy * size };
    Vector2 a = { base.x - dy * size * 0.55f, base.y + dx * size * 0.55f };
    Vector2 b = { base.x + dy * size * 0.55f, base.y - dx * size * 0.55f };
    DrawTriangle(tip, a, b, c);
    DrawTriangle(tip, b, a, c);
}

float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

std::vector<std::string> wrapText(const std::function<float(const std::string&)>& width, const std::string& text, float maxW) {
    std::vector<std::string> lines;
    std::string line, word;
    auto flushWord = [&]() {
        if (word.empty()) return;
        std::string candidate = line.empty() ? word : line + " " + word;
        if (width(candidate) > maxW && !line.empty()) {
            lines.push_back(line);
            line = word;
        }
        else {
            line = candidate;
        }
        word.clear();
    };
    for (char ch : text) {
        if (ch == ' ') flushWord();
        else word += ch;
    }
    flushWord();
    if (!line.empty()) lines.push_back(line);
    return lines;
}

}
