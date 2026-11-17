// Drawing helpers of the desktop app: palette, gradient tints, glow, rounded gradient shapes.
// Every state is a vivid left-to-right gradient with a soft glow around it.

#ifndef GUIDRAW_H
#define GUIDRAW_H

#include "raylib.h"
#include "../core/Recorder.h"
#include <functional>
#include <string>
#include <vector>

// colors that glide from one state to the next. top = left end of the gradient, bot = right end.
struct Tint {
    float top[3];
    float bot[3];
    float glow;
};

namespace gfx {
    extern const Color BG, HEADER, PANEL, PANEL2, LINE, TEXT, TEXT2, MUTED, ACCENT;

    Tint tintFor(Role r);
    void approach(Tint& a, const Tint& b, float k);
    void midOf(const Tint& t, float* out);
    Color textOn(const Tint& t, float alpha = 255.0f);
    Color rgb(const float* c, float a = 255.0f);
    Color lighten(const float* c, float amount, float a = 255.0f);

    void glowRect(Rectangle r, float radius, const float* c, float intensity, float spread);
    void glowCircle(float cx, float cy, float radius, const float* c, float intensity, float spread);
    void gradRounded(Rectangle r, float rad, Color left, Color right, bool roundBottom);
    void gradCircleH(float cx, float cy, float rad, Color left, Color right);
    void dashedRounded(Rectangle r, float roundness, Color c);
    void arrowHead(Vector2 tip, Vector2 from, float size, Color c);

    float clampf(float v, float lo, float hi);
    std::vector<std::string> wrapText(const std::function<float(const std::string&)>& width, const std::string& text, float maxW);
}

#endif
