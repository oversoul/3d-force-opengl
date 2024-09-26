BUILD_DIR = build
INCLUDE_DIR = include
CFLAGS = -std=c++17 -Wall -Iinclude -I. -Wextra -Wall -Wno-pragmas
LDFLAGS = `pkg-config --cflags --libs jsoncpp` -lGL -lGLEW -lGLU -lglfw

CC = clang++-14

SRC_DIR = src

# Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

TARGET = app

all: $(TARGET)

run: all
	@./$(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

clean:
	@rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean run
