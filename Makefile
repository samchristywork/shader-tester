CC := g++
LIBS := -lglfw -lGLEW -lGL -lm -lassimp
CFLAGS := -I src/ -I imgui -I imgui/backends
OBJECTS := \
	build/main.o \
	build/gui.o \
	build/controls.o \
	build/imgui.o \
	build/imgui_draw.o \
	build/imgui_widgets.o \
	build/imgui_tables.o \
	build/imgui_demo.o \
	build/backends/imgui_impl_glfw.o \
	build/backends/imgui_impl_opengl3.o

.PHONY: all
all: build/main

.PHONY: objects
objects: $(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))

build/%.o: imgui/%.cpp
	mkdir -p build/backends/
	$(CC) -c $(CFLAGS) $< -o $@

build/%.o: src/%.cpp
	mkdir -p build/backends/
	$(CC) -c $(CFLAGS) $< -o $@

build/main: $(OBJECTS)
	${CC} $(CFLAGS) build/*.o build/backends/*.o ${LIBS} -o $@

.PHONY: run
run:
	make && ./build/main


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
