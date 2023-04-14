CC := gcc
LIBS := -lglfw -lGLEW -lGL -lm
CFLAGS :=

.PHONY: all
all: build/main

.PHONY: objects
objects: $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))

build/%.o: src/%.c
	mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/main: build/main.o
	${CC} build/*.o ${LIBS} -o $@

.PHONY: clean
clean:
	rm -rf build/
