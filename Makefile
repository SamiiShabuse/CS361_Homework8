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

$(TARGET): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

test7: $(TARGET)
	$(TARGET) 7 seq7.txt

test9: $(TARGET)
	$(TARGET) 9 seq9.txt

test11: $(TARGET)
	$(TARGET) 11 seq11.txt

test13: $(TARGET)
	$(TARGET) 13 seq13.txt

clean:
	rm -f $(SRCDIR)/*.o
	rm -rf $(BINDIR)