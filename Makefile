EXE = build/chess

SRC_DIR =   src
INC_DIR =   src/includes
IMGUI_DIR = src/includes/imgui

SRCS =  main.cpp
SRCS += $(SRC_DIR)/piece.cpp 
SRCS += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SRCS += $(IMGUI_DIR)/backends/imgui_impl_sdl3.cpp $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer3.cpp

OBJS = $(addsuffix .o, $(addprefix build/, $(basename $(notdir $(SRCS)))))

CXX = g++
CXXFLAGS =  -std=c++20 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -Wall -Wformat `pkg-config sdl3 --cflags`
LIBS = -ldl `pkg-config sdl3 --libs`

all: $(EXE)
	@echo Build complete

build/%.o: %.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: $(SRC_DIR)/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: $(IMGUI_DIR)/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: $(IMGUI_DIR)/backends/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build:
	mkdir -p build



$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -rf build/