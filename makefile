CXX = g++
LD = g++

INCLUDE_DIR = include
SOURCE_DIR = source
OBJECTS_DIR = object

CXXFLAGS = -I $(INCLUDE_DIR) `pkg-config --cflags gtk+-3.0`

SOURCES = $(wildcard $(SOURCE_DIR)/*.cpp)

OBJECTS = $(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECTS_DIR)/%.o,$(SOURCES))

TARGET = linux-regedit
LDFLAGS_COMMON = -std=c++2a `pkg-config --libs gtk+-3.0`
RM = /bin/rm

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(LD) $^ $(LDFLAGS_COMMON) -o $(TARGET)

$(OBJECTS_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS_COMMON)

clean:
	$(RM) $(TARGET) $(OBJECTS)

