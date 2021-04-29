# -- Port -- #
PORT = linux

# -- Source files -- #
#SRC = $(wildcard src/*.cpp)
#SRC += ports/$(PORT)/iotoro_$(PORT).cpp
#OBJ = $(patsubst %.cpp,%.o,$(SRC))

SOURCE_DIR = src
BUILD_DIR = build

SOURCES = aes.cpp md5.cpp iotoro_client.cpp iotoro_connection.cpp iotoro_utils.cpp  main.cpp
SOURCES += iotoro_$(PORT).cpp

SRC = $(patsubst %.cpp,$(SOURCE_DIR)/%.cpp,$(SOURCES))
OBJ = $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))
DEPS = $(patsubst %.cpp,%.h,$(SRC))

# -- Compiler -- #
CC = g++
CFLAGS = -I $(SOURCE_DIR) -Wall -g

# -- Output -- #
TARGET = $(BUILD_DIR)/iotoro


$(TARGET): $(OBJ) Makefile 
	$(CC) $(CFLAGS) $(OBJ) -o $@

run: $(TARGET) 
	./$(TARGET)

clean:
	rm $(OBJ)
		

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@
 