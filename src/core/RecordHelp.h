// Small helpers that build frames for the simple containers (stacks, queues, lists, arrays).
// Every function does nothing when the recorder is off.

#ifndef RECORDHELP_H
#define RECORDHELP_H

#include "Recorder.h"
#include <string>
#include <vector>

namespace RecHelp {

    // up to two highlighted positions
    struct Hi {
        int a;
        Role roleA;
        int b;
        Role roleB;
        Hi() : a(-1), roleA(ROLE_CURRENT), b(-1), roleB(ROLE_CURRENT) {}
        Hi(int posA, Role ra) : a(posA), roleA(ra), b(-1), roleB(ROLE_CURRENT) {}
        Hi(int posA, Role ra, int posB, Role rb) : a(posA), roleA(ra), b(posB), roleB(rb) {}
    };

    inline Role roleAt(const Hi& hi, int i) {
        if (i == hi.a) return hi.roleA;
        if (i == hi.b) return hi.roleB;
        return ROLE_NORMAL;
    }

    // a row (or column) of values that exist: linked list, linked stack, linked queue
    inline Frame valuesFrame(FrameKind kind, const std::vector<int>& vals, const std::string& msg, int line,
                             const std::vector<Marker>& markers, const Hi& hi) {
        Frame f;
        f.kind = kind;
        f.message = msg;
        f.codeLine = line;
        for (size_t i = 0; i < vals.size(); i++) f.cells.push_back(Cell(vals[i], roleAt(hi, (int)i)));
        f.markers = markers;
        return f;
    }

    inline void addValues(FrameKind kind, const std::vector<int>& vals, const std::string& msg, int line,
                          const std::vector<Marker>& markers, const Hi& hi, bool floating = false, int floatValue = 0,
                          bool doubly = false, bool circular = false) {
        if (!Recorder::enabled()) return;
        Frame f = valuesFrame(kind, vals, msg, line, markers, hi);
        f.hasFloating = floating;
        f.floating = Cell(floatValue, ROLE_NEW);
        f.doubly = doubly;
        f.circular = circular;
        Recorder::add(f);
    }

    // fixed slots: slot i holds arr[i] when used[i] is true, otherwise it is drawn empty
    inline void addSlots(FrameKind kind, const int arr[], int cap, const std::vector<char>& used, const std::string& msg,
                         int line, const std::vector<Marker>& markers, const Hi& hi) {
        if (!Recorder::enabled()) return;
        Frame f;
        f.kind = kind;
        f.message = msg;
        f.codeLine = line;
        for (int i = 0; i < cap; i++) {
            Role r = roleAt(hi, i);
            if (used[i]) f.cells.push_back(Cell(arr[i], r));
            else f.cells.push_back(Cell::Empty(r));
        }
        f.markers = markers;
        Recorder::add(f);
    }

    // used flags for a slot range [from .. to] that may wrap around (circular queue, deque)
    inline std::vector<char> usedRange(int cap, int from, int count) {
        std::vector<char> used(cap, 0);
        for (int k = 0; k < count; k++) used[(from + k) % cap] = 1;
        return used;
    }

    // one explaining frame without any highlighted cell (errors such as overflow)
    inline void addNote(Frame f, const std::string& msg, int line) {
        if (!Recorder::enabled()) return;
        f.message = msg;
        f.codeLine = line;
        Recorder::add(f);
    }
}

#endif
