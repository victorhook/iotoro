INCLUDES = include

SRC_DIR = src
OBJ_DIR = objects
SRC = $(wildcard $(SRC_DIR)/*.cpp)
# OBJ = $(SRC:.cpp=.o)
OBJ = $(patsubst $(OBJ_DIR)/%.o,%$(SRC_DIR)%.cpp,$(SRC))
DEPS = $(wildcard $(INCLUDES)/*.h)

CC = g++
CFLAGS = -I $(INCLUDES) -g -Wall

TARGET = iotoro

$(TARGET): $(OBJ) $(DEPS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp Makefile  
	$(CC) $(CFLAGS) $^ -o $@

run: $(TARGET)
	@ chmod +x $(TARGET) 
	./$(TARGET)