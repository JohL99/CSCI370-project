# Makefile for Assn7

# Compiler
CXX = g++-4.9

# Compiler flags
CXXFLAGS = -Wall -Wextra -g -I/usr/local/lib/Oracle/instantclient_11_2/sdk/include

# Linker flags
LFLAGS = -L/usr/local/lib/Oracle/instantclient_11_2 -locci -lociei

# Source files
SRCS = ui.cpp main.cpp db.cpp

# Object files
OBJS = $(addprefix build/, $(SRCS:.cpp=.o))

# Target executable
TARGET = Project

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LFLAGS)

build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<  -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET)