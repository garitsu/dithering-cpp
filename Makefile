# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# OpenCV installation directory
OPENCV_DIR := /opt/homebrew/opt/opencv

# OpenCV includes and libraries
OPENCV_INCLUDES = -I$(OPENCV_DIR)/include/opencv
OPENCV_LIBS = -L$(OPENCV_DIR)/lib \
              -lopencv_core \
              -lopencv_imgproc \
              -lopencv_imgcodecs \


# Target executable name
TARGET = dithering

# Source files
SOURCES = dithering.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Linking the target executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(OPENCV_LIBS)

# Compiling source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OPENCV_INCLUDES) -c $<

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean

# Print OpenCV and ffmpeg information
print-info:
	@echo "OpenCV Directory: $(OPENCV_DIR)"
	@echo "OpenCV Includes: $(OPENCV_INCLUDES)"
	@echo "OpenCV Libraries: $(OPENCV_LIBS)"
