CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

.PHONY: clean

editor: editor.cpp
	$(CXX) $(CXXFLAGS) editor.cpp -o editor

clean:
	rm editor
