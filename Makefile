CC = gcc
CXX = g++

CCFLAGS = -Wno-sign-compare -Wall -O2 -fopenmp -lpthread
CXXFLAGS = -Wno-sign-compare -Wall -O2 -fopenmp -lpthread -std=c++11
LDFLAGS = -fPIC -c
CCFLAGS += -g
CXXFLAGS += -g

INCLUDE_FLAGS = -I/usr/include/x86_64-linux-gnu/qt5
LINK_FLAGS = -lQt5Widgets -lQt5Gui -lQt5Core -lboost_program_options

SOURCES = Layout.cpp InFileReader.cpp Router.cpp ERouter.cpp
OBJECTS = $(SOURCES:.cpp=.o)

ERouter: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o ERouter $(LINK_FLAGS)
	rm *.o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(INCLUDE_FLAGS) $< -o $@

clean:
	rm *.o ERouter
