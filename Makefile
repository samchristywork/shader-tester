CC := g++
LIBS := -lglfw -lGLEW -lGL -lm -lassimp
CFLAGS := -I src/ -I imgui -I imgui/backends
IMGUI_SRC = ./imgui/imgui.cpp \
            ./imgui/imgui_draw.cpp \
            ./imgui/imgui_widgets.cpp \
            ./imgui/imgui_tables.cpp \
            ./imgui/imgui_demo.cpp \
            ./imgui/backends/imgui_impl_glfw.cpp \
            ./imgui/backends/imgui_impl_opengl3.cpp

.PHONY: all
all: build/main

.PHONY: objects
objects: $(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))

build/%.o: src/%.cpp
	mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/main: build/main.o build/gui.o build/imgui.o build/imgui_draw.o build/imgui_widgets.o build/imgui_tables.o build/imgui_demo.o build/imgui_impl_glfw.o build/imgui_impl_opengl3.o
	${CC} $(CFLAGS) build/*.o ${LIBS} -o $@


.PHONY: debug
debug: CFLAGS+=-g -Wall -Werror -Wpedantic
debug: LIBS+=
debug: all

.PHONY: release
release: CFLAGS+=-O2
release: LIBS+=
release: all

.PHONY: clean
clean:
	rm -rf build/
