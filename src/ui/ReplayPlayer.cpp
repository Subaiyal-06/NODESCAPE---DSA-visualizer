#include "ReplayPlayer.h"
#include "ConsoleUtils.h"
#include "../core/Recorder.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

static string roleTag(Role role) {
    if (role == ROLE_CURRENT) return "cur";
    if (role == ROLE_FOUND) return "found";
    if (role == ROLE_NEW) return "new";
    if (role == ROLE_REMOVED) return "delete";
    if (role == ROLE_DONE) return "done";
    if (role == ROLE_KEPT) return "keep";
    return "";
}

static string padRight(const string& text, size_t width) {
    if (text.size() >= width) {
        return text;
    }
    return text + string(width - text.size(), ' ');
}

// all marker labels that point at one cell, for example "head/t"
static string markerText(const Frame& frame, int index) {
    string text = "";
    for (size_t i = 0; i < frame.markers.size(); i++) {
        if (frame.markers[i].index == index) {
            if (!text.empty()) text += "/";
            text += frame.markers[i].label;
        }
    }
    return text;
}

static void drawLinear(const Frame& frame) {
    if (frame.hasFloating) {
        cout << "New node (not linked yet): [" << frame.floating.value << "]\n" << endl;
    }
    if (frame.cells.empty()) {
        cout << (frame.kind == KIND_LIST ? "head -> NULL   (the list is empty)" : "(empty)") << endl;
        return;
    }

    vector<size_t> widths;
    vector<string> cellStrings;
    for (size_t i = 0; i < frame.cells.size(); i++) {
        string text = "[" + to_string(frame.cells[i].value) + "]";
        if (frame.kind == KIND_LIST) {
            text += (i + 1 < frame.cells.size()) ? "->" : "->NULL";
        }
        cellStrings.push_back(text);
        size_t w = text.size();
        string marker = markerText(frame, (int)i);
        string tag = roleTag(frame.cells[i].role);
        if (marker.size() > w) w = marker.size();
        if (tag.size() + 1 > w) w = tag.size() + 1;
        widths.push_back(w + 1);
    }

    string markerRow = "";
    string cellRow = "";
    string tagRow = "";
    for (size_t i = 0; i < frame.cells.size(); i++) {
        string marker = markerText(frame, (int)i);
        string tag = roleTag(frame.cells[i].role);
        markerRow += padRight(marker.empty() ? "" : marker, widths[i]);
        cellRow += padRight(cellStrings[i], widths[i]);
        tagRow += padRight(tag.empty() ? "" : "^" + tag, widths[i]);
    }
    cout << markerRow << endl;
    cout << cellRow << endl;
    cout << tagRow << endl;
}

static void drawBars(const Frame& frame) {
    int n = (int)frame.cells.size();
    if (n == 0) {
        cout << "(empty)" << endl;
        return;
    }
    int minValue = frame.cells[0].value;
    int maxValue = frame.cells[0].value;
    for (int i = 1; i < n; i++) {
        if (frame.cells[i].value < minValue) minValue = frame.cells[i].value;
        if (frame.cells[i].value > maxValue) maxValue = frame.cells[i].value;
    }
    const int LEVELS = 8;
    vector<int> heights;
    vector<size_t> widths;
    for (int i = 0; i < n; i++) {
        int h = (maxValue == minValue) ? LEVELS / 2 : 1 + (int)(((long long)(frame.cells[i].value - minValue) * (LEVELS - 1)) / (maxValue - minValue));
        heights.push_back(h);
        size_t w = to_string(frame.cells[i].value).size() + 2;
        string tag = roleTag(frame.cells[i].role);
        if (tag.size() + 1 > w) w = tag.size() + 1;
        widths.push_back(w + 1);
    }
    for (int level = LEVELS; level >= 1; level--) {
        string row = "";
        for (int i = 0; i < n; i++) {
            row += padRight(heights[i] >= level ? "###" : "", widths[i]);
        }
        cout << row << endl;
    }
    string valueRow = "";
    string tagRow = "";
    for (int i = 0; i < n; i++) {
        string tag = roleTag(frame.cells[i].role);
        valueRow += padRight("[" + to_string(frame.cells[i].value) + "]", widths[i]);
        tagRow += padRight(tag.empty() ? "" : "^" + tag, widths[i]);
    }
    cout << valueRow << endl;
    cout << tagRow << endl;
}

static const TreeNodeInfo* findNode(const Frame& frame, int id) {
    for (size_t i = 0; i < frame.nodes.size(); i++) {
        if (frame.nodes[i].id == id) {
            return &frame.nodes[i];
        }
    }
    return nullptr;
}

static void printTree(const Frame& frame, int id, int indent) {
    const TreeNodeInfo* node = findNode(frame, id);
    if (node == nullptr) {
        return;
    }
    printTree(frame, node->right, indent + 6);
    string tag = roleTag(node->role);
    cout << string(indent, ' ') << node->value;
    if (!tag.empty()) cout << "   <- " << tag;
    cout << endl;
    printTree(frame, node->left, indent + 6);
}

static void drawFrame(const Frame& frame) {
    if (frame.kind == KIND_TREE) {
        if (frame.nodes.empty()) cout << "(the tree is empty)" << endl;
        else printTree(frame, frame.rootId, 0);
    }
    else if (frame.kind == KIND_BARS) {
        drawBars(frame);
    }
    else {
        drawLinear(frame);
    }
}

void runReplay() {
    int total = Recorder::frameCount();
    if (total == 0) {
        return;
    }
    int index = 0;
    bool leave = false;

    while (!leave) {
        const Frame& frame = Recorder::frame(index);
        clearScreen();
        printTitle("Replay: " + Recorder::title());
        cout << "Step " << (index + 1) << " of " << total << (Recorder::truncated() ? "   (very long operation, the rest was not recorded)" : "") << "\n" << endl;

        drawFrame(frame);

        cout << "\n" << frame.message << endl;
        if (frame.comparisons > 0 || frame.swaps > 0) {
            cout << "Comparisons: " << frame.comparisons;
            if (frame.kind == KIND_BARS) cout << "   Swaps or moves: " << frame.swaps;
            cout << endl;
        }

        const vector<string>& code = Recorder::code();
        if (!code.empty()) {
            cout << "\nCode:" << endl;
            for (size_t i = 0; i < code.size(); i++) {
                cout << (frame.codeLine == (int)i ? " => " : "    ") << code[i] << endl;
            }
        }

        cout << "\n[Enter] next   [p] previous   [f] first   [l] last   [e] export JSON   [number] jump   [q] leave" << endl;
        string command = readLine("> ");

        if (command == "q" || command == "Q") {
            leave = true;
        }
        else if (command == "e" || command == "E") {
            if (Recorder::exportJSON("recording.json")) {
                cout << "\nSaved trace to recording.json! It holds every step of this operation." << endl;
            } else {
                cout << "\nFailed to write recording.json." << endl;
            }
            pauseScreen();
        }
        else if (command == "p" || command == "P") {
            if (index > 0) index--;
        }
        else if (command == "f" || command == "F") {
            index = 0;
        }
        else if (command == "l" || command == "L") {
            index = total - 1;
        }
        else if (command.empty() || command == "n" || command == "N") {
            if (index < total - 1) index++;
            else leave = true; // Enter on the last step leaves the replay
        }
        else {
            bool digitsOnly = true;
            for (size_t i = 0; i < command.size(); i++) {
                if (command[i] < '0' || command[i] > '9') digitsOnly = false;
            }
            if (digitsOnly && command.size() < 7) {
                int step = stoi(command);
                if (step >= 1 && step <= total) index = step - 1;
            }
        }
    }
}

void finishOperation() {
    if (Recorder::enabled() && Recorder::frameCount() > 0) {
        cout << "\nReplay mode is on. Press Enter to watch this operation step by step...";
        cin.get();
        runReplay();
        Recorder::clear();
    }
    else {
        pauseScreen();
    }
}
