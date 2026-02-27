.PHONY: all clean

CXX = g++
CXX_FLAG = -Wall -Wextra -std=c++14 -g
INCLUDE_PATH = -Iheader

OBJECT_DIRECTORY = obj
SOURCE_DIRECTORY = src

SOURCE = $(wildcard $(SOURCE_DIRECTORY)/*.cpp)
OBJECT = $(patsubst $(SOURCE_DIRECTORY)/%.cpp, $(OBJECT_DIRECTORY)/%.o, $(SOURCE))
DEPENDENCY = $(patsubst $(SOURCE_DIRECTORY)/%.cpp, $(OBJECT_DIRECTORY)/%.d, $(SOURCE))

TARGET = sop

all: $(TARGET)

$(OBJECT_DIRECTORY):
	mkdir -p $(OBJECT_DIRECTORY)

$(TARGET): $(OBJECT)
	$(CXX) $(CXX_FLAG) $(INCLUDE_PATH) $^ -o $@

-include $(DEPENDENCY)

$(OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp | $(OBJECT_DIRECTORY)
	$(CXX) $(CXX_FLAG) $(INCLUDE_PATH) -MMD -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJECT_DIRECTORY)