CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
SRCS = main.cpp hermesfs.cpp
OBJS = $(SRCS:.cpp=.o)
EXECUTABLE = hermesfs

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE)

.PHONY: all clean