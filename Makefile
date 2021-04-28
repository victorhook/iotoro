# -- Port -- #
PORT = linux

# -- Source files -- #
#SRC = $(wildcard src/*.cpp)
#SRC += ports/$(PORT)/iotoro_$(PORT).cpp
#OBJ = $(patsubst %.cpp,%.o,$(SRC))

SOURCE_DIR = src
BUILD_DIR = build

SOURCES = aes.cpp iotoro.cpp md5.cpp main.cpp
SOURCES += iotoro_$(PORT).cpp

SRC = $(patsubst %.cpp,$(SOURCE_DIR)/%.cpp,$(SOURCES))
OBJ = $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

# -- Compiler -- #
CC = g++
CFLAGS = -I src -I $(PORT_INCLUDE) -g -Wall

# -- Output -- #
TARGET = iotoro_TESTI

build: $(OBJ)
	g++ $(OBJ) -o $(TARGET)

run: $(build)
	echo $^


clean:
	rm $(OBJ)
		

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	g++ -c $^ -o $@
