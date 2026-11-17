# Build helper for Linux / macOS / MSYS2 (mingw32-make also works on Windows)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

SRC = $(wildcard src/*.cpp src/core/*.cpp src/ui/*.cpp)
CORE = $(wildcard src/core/*.cpp)
GUI_SRC = $(wildcard src/core/*.cpp src/gui/*.cpp)
TESTS = $(wildcard tests/test_*.cpp)

all: build/NodeScape build/NodeScapeApp.exe

build/NodeScape: $(SRC)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o build/NodeScape

build/NodeScapeApp.exe: $(GUI_SRC)
	mkdir -p build
	$(CXX) $(CXXFLAGS) -O2 -mwindows $(GUI_SRC) -lraylib -lopengl32 -lgdi32 -lwinmm -o build/NodeScapeApp.exe

# every tests/test_*.cpp is built with the core and run
test: $(CORE) $(TESTS)
	mkdir -p build
	@for t in $(TESTS); do \
		name=$$(basename $$t .cpp); \
		$(CXX) $(CXXFLAGS) $(CORE) $$t -o build/$$name || exit 1; \
		echo "== $$name"; \
		./build/$$name || exit 1; \
	done

clean:
	rm -rf build
