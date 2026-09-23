CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = resource_monitor
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) process_report.txt

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
