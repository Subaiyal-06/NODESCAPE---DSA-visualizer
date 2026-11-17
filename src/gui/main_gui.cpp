#include "GUIApp.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

// Usage: NodeScapeApp.exe                      open the app
//        NodeScapeApp.exe --shot file.png view sub "act" step     save a picture and quit (testing)
int main(int argc, char** argv) {
    std::srand(argc > 1 && std::strcmp(argv[1], "--shot") == 0 ? 7u : (unsigned int)std::time(nullptr));
    GUIApp app;
    if (argc >= 7 && std::strcmp(argv[1], "--shot") == 0) {
        app.shot(argv[2], std::atoi(argv[3]), std::atoi(argv[4]), argv[5], std::atoi(argv[6]));
        return 0;
    }
    app.run();
    return 0;
}
