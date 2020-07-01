CXX?=g++
CXXFLAGS+=-O3

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
