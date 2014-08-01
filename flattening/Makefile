SOURCES=redraw.cpp
OBJECTS=redraw.o
CC=clang++
CFLAGS=-g -Wall -Werror -Wextra -Wshadow
LIBS=-framework OpenGL -lGLEW -lglfw3 `pkg-config --libs opencv`
INCLUDE=-L/opt/local/lib/
BINARY=redraw

all: $(BINARY)

$(BINARY): $(SOURCES)
	$(CC) $(INCLUDE) $(SOURCES) $(LIBS) -o $(BINARY)

clean:
	rm -f $(OBJECTS) $(BINARY)

.PHONY: clean
