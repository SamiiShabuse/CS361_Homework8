CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -O2 -pthread

SRCDIR 	 := src
INCDIR	 := include
BINDIR   := bin


TARGET   := $(BINDIR)/sequence

SOURCES  := $(SRCDIR)/main.cpp $(SRCDIR)/ns1d0.cpp
OBJECTS  := $(SOURCES:.cpp=.o)

.PHONY: all clean test7 test9 test11 test13

all: $(TARGET)

