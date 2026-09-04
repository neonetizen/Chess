EXE = build/chess

SRC   = src
INC   = include
IMGUI = gui/imgui

SRCS =  $(SRC)/main.cpp
SRCS += $(IMGUI)/imgui.cpp $(IMGUI)/imgui_draw.cpp $(IMGUI)/imgui_tables.cpp $(IMGUI)/imgui_widgets.cpp
SRCS += $(IMGUI)/backends/imgui_impl_sdl3.cpp $(IMGUI)/backends/imgui_impl_sdlrenderer3.cpp

OBJS = $(addsuffix .o, $(addprefix build/, $(basename $(notdir $(SRCS)))))

CXX = g++
CXXFLAGS =  -std=c++20 -I$(IMGUI) -I$(IMGUI)/backends
CXXFLAGS += -g -Wall -Wextra -Wpedantic  -Wformat `pkg-config sdl3 --cflags`
LIBS = -ldl `pkg-config sdl3 --libs`

all: $(EXE)
	@echo Build complete

build/%.o: %.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: $(SRC)/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: $(IMGUI)/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: $(IMGUI)/backends/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build:
	mkdir -p build

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -rf build/