#include "Recorder.h"
#include <fstream>
#include <sstream>

namespace {
    const int MAX_FRAMES = 4000; // keeps memory small even for long operations

    bool recordingOn = false;
    int pauseDepth = 0;
    bool tooMany = false;
    std::string recordingTitle;
    std::vector<std::string> recordingCode;
    std::vector<Frame> frames;

    std::string escapeJSON(const std::string& input) {
        std::ostringstream ss;
        for (char c : input) {
            if (c == '"') ss << "\\\"";
            else if (c == '\\') ss << "\\\\";
            else if (c == '\b') ss << "\\b";
            else if (c == '\f') ss << "\\f";
            else if (c == '\n') ss << "\\n";
            else if (c == '\r') ss << "\\r";
            else if (c == '\t') ss << "\\t";
            else if ((unsigned char)c < 0x20) {
                // control character
            } else {
                ss << c;
            }
        }
        return ss.str();
    }
}

void Recorder::setEnabled(bool on) {
    recordingOn = on;
    if (!on) {
        clear();
    }
}

bool Recorder::enabled() {
    return recordingOn && pauseDepth == 0;
}

void Recorder::pause() {
    pauseDepth++;
}

void Recorder::resume() {
    if (pauseDepth > 0) pauseDepth--;
}

void Recorder::begin(const std::string& title, const std::vector<std::string>& code) {
    if (!enabled()) {
        return;
    }
    frames.clear();
    tooMany = false;
    recordingTitle = title;
    recordingCode = code;
}

void Recorder::add(const Frame& frame) {
    if (!enabled()) {
        return;
    }
    if ((int)frames.size() >= MAX_FRAMES) {
        tooMany = true;
        return;
    }
    frames.push_back(frame);
}

void Recorder::clear() {
    frames.clear();
    recordingCode.clear();
    recordingTitle.clear();
    tooMany = false;
}

int Recorder::frameCount() {
    return (int)frames.size();
}

const Frame& Recorder::frame(int index) {
    return frames[index];
}

const std::vector<std::string>& Recorder::code() {
    return recordingCode;
}

const std::string& Recorder::title() {
    return recordingTitle;
}

bool Recorder::truncated() {
    return tooMany;
}

bool Recorder::exportJSON(const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        return false;
    }

    out << "{\n";
    out << "  \"title\": \"" << escapeJSON(recordingTitle) << "\",\n";
    out << "  \"truncated\": " << (tooMany ? "true" : "false") << ",\n";
    out << "  \"code\": [\n";
    for (size_t i = 0; i < recordingCode.size(); i++) {
        out << "    \"" << escapeJSON(recordingCode[i]) << "\"" << (i + 1 < recordingCode.size() ? "," : "") << "\n";
    }
    out << "  ],\n";
    out << "  \"frames\": [\n";
    for (size_t i = 0; i < frames.size(); i++) {
        const Frame& f = frames[i];
        out << "    {\n";
        out << "      \"kind\": " << (int)f.kind << ",\n";
        out << "      \"message\": \"" << escapeJSON(f.message) << "\",\n";
        out << "      \"codeLine\": " << f.codeLine << ",\n";
        out << "      \"comparisons\": " << f.comparisons << ",\n";
        out << "      \"swaps\": " << f.swaps << ",\n";
        out << "      \"cells\": [";
        for (size_t c = 0; c < f.cells.size(); c++) {
            out << "{\"value\":" << f.cells[c].value << ",\"role\":" << (int)f.cells[c].role << "}" << (c + 1 < f.cells.size() ? "," : "");
        }
        out << "],\n";
        out << "      \"markers\": [";
        for (size_t m = 0; m < f.markers.size(); m++) {
            out << "{\"index\":" << f.markers[m].index << ",\"label\":\"" << escapeJSON(f.markers[m].label) << "\"}" << (m + 1 < f.markers.size() ? "," : "");
        }
        out << "],\n";
        out << "      \"nodes\": [";
        for (size_t n = 0; n < f.nodes.size(); n++) {
            const TreeNodeInfo& tn = f.nodes[n];
            out << "{\"id\":" << tn.id << ",\"value\":" << tn.value << ",\"left\":" << tn.left << ",\"right\":" << tn.right << ",\"role\":" << (int)tn.role << "}" << (n + 1 < f.nodes.size() ? "," : "");
        }
        out << "],\n";
        out << "      \"rootId\": " << f.rootId << "\n";
        out << "    }" << (i + 1 < frames.size() ? "," : "") << "\n";
    }
    out << "  ]\n";
    out << "}\n";
    return true;
}
