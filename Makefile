# -- Port -- #
PORT = linux

# -- Source files -- #
SRC = $(wildcard src/*.cpp)
SRC += ports/$(PORT)/iotoro_$(PORT).cpp
OBJ = $(patsubst %.cpp,%.o,$(SRC))

# -- Include dirs -- #
INCLUDE = include
PORT_INCLUDE = ports/$(PORT)

DEPS = $(INCLUDE)/*
DEPS += $(PORT_INCLUDE)

# -- Compiler -- #
CC = g++
CFLAGS = -I src -I $(PORT_INCLUDE) -g -Wall

# -- Output -- #
TARGET = iotoro


run: $(TARGET)
	./$(TARGET)

clean: $(TARGET)
	rm $(OBJ)


$(TARGET): $(OBJ) 
	$(CC) $(CFLAGS) $^ -o $@


%.o: %.cpp Makefile $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@ 