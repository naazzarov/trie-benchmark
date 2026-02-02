# Makefile for Trie Variants Research Paper Implementation
# Space-Time Trade-offs in Trie Variants for Large-Scale String Indexing

CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
INCLUDES = -I./include
SRCDIR = src
OBJDIR = obj
TARGET = trie_benchmark

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp) main.cpp
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SOURCES)))

# Default target
all: $(TARGET)

# Create obj directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Link
$(TARGET): $(OBJDIR) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)
	@echo "✓ Build complete! Run with: ./$(TARGET)"

# Compile
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the benchmark
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "✓ Cleaned build artifacts"

# Help
help:
	@echo "Trie Variants Benchmark - Makefile"
	@echo "=================================="
	@echo "Targets:"
	@echo "  make         - Build the project"
	@echo "  make run     - Build and run benchmarks"
	@echo "  make clean   - Remove build artifacts"
	@echo "  make help    - Show this help message"

.PHONY: all run clean help
