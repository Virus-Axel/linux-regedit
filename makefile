CXX = clang++
LD = clang++

INCLUDE_DIR = include
SOURCE_DIR = source
OBJECTS_DIR = object

CXXFLAGS = -I $(INCLUDE_DIR)

SOURCES = $(wildcard $(SOURCE_DIR)/*.cpp)

OBJECTS = $(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECTS_DIR)/%.o,$(SOURCES))

TARGET = linux-regedit
LDFLAGS_COMMON = -std=c++2a
RM = /bin/rm

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(LD) $^ $(LDFLAGS_COMMON) -o $(TARGET)

$(OBJECTS_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(OBJECTS)
