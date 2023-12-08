CXX = g++
CXXFLAGS = -std=c++17 -I /usr/local/include

all: efficient_frontier

efficient_frontier: efficient_frontier.cpp parse.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f efficient_frontier 
