.PHONY: all

TARGET=main
CXX=g++
INC=-I./
SRC=$(shell ls *.cpp)
OBJS=$(SRC:%.cpp=%.o)
CXX_FLAGS=-fPIC -Wall -Werror -Wextra --std=c++17

all: $(OBJS)
	$(CXX) $(INC) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(INC) $(CXX_FLAGS) -c -o $@ $<

clean:
	rm *.o
	rm *.exe