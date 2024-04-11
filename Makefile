# Makefile for Assn7

# Compiler
CXX = g++-4.9

# Compiler flags
CXXFLAGS = -g -I/usr/local/lib/Oracle/instantclient_11_2/sdk/include -I./include

# Linker flags
LFLAGS = -L/usr/local/lib/Oracle/instantclient_11_2 -locci -lociei

# Source files
SRCS = src/ui.cpp src/db.cpp main.cpp src/screenStack.cpp

# Object files
OBJS = $(addprefix build/, $(notdir $(SRCS:.cpp=.o)))

# Target executable
TARGET = Project

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LFLAGS)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $<  -o $@

build/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $<  -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET)