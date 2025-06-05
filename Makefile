# GTU-C312 Operating System Project Makefile
# Comprehensive build system for CPU simulator and OS

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
LDFLAGS = 

# Directories
CPU_DIR = CPU
OS_DIR = OS
BIN_DIR = bin

# Source files
CPU_SRC = $(CPU_DIR)/gtu_c312_cpu.cpp
OS_FILE = $(OS_DIR)/os.txt

# Target executable
CPU_EXECUTABLE = $(BIN_DIR)/gtu_c312_cpu

# Create directories if they don't exist
$(shell mkdir -p $(BIN_DIR))

# Default target
.PHONY: all
all: $(CPU_EXECUTABLE)

# Compile the CPU simulator
$(CPU_EXECUTABLE): $(CPU_SRC)
	@echo "Compiling GTU-C312 CPU Simulator..."
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✅ CPU simulator compiled successfully!"

# Run the OS with default settings
.PHONY: run
run: $(CPU_EXECUTABLE)
	@echo "🚀 Running GTU-C312 Operating System..."
	./$(CPU_EXECUTABLE) $(OS_FILE)

# Run with debug level 0 (final memory only)
.PHONY: run-final
run-final: $(CPU_EXECUTABLE)
	@echo "🚀 Running OS (final memory dump only)..."
	./$(CPU_EXECUTABLE) $(OS_FILE) -D 0

# Run with debug level 1 (memory after each instruction)
.PHONY: run-debug
run-debug: $(CPU_EXECUTABLE)
	@echo "🚀 Running OS (memory dump after each instruction)..."
	./$(CPU_EXECUTABLE) $(OS_FILE) -D 1

# Run with debug level 2 (step-by-step execution)
.PHONY: run-step
run-step: $(CPU_EXECUTABLE)
	@echo "🚀 Running OS (step-by-step execution)..."
	./$(CPU_EXECUTABLE) $(OS_FILE) -D 2

# Run with debug level 3 (thread table monitoring)
.PHONY: run-threads
run-threads: $(CPU_EXECUTABLE)
	@echo "🚀 Running OS (thread table monitoring)..."
	./$(CPU_EXECUTABLE) $(OS_FILE) -D 3

# Save output to file for analysis
.PHONY: run-save
run-save: $(CPU_EXECUTABLE)
	@echo "🚀 Running OS and saving output to results.txt..."
	./$(CPU_EXECUTABLE) $(OS_FILE) > results.txt 2>&1
	@echo "✅ Output saved to results.txt"

# Performance test - run and time execution
.PHONY: performance
performance: $(CPU_EXECUTABLE)
	@echo "⏱️  Performance testing..."
	time ./$(CPU_EXECUTABLE) $(OS_FILE)

# Build with different optimization levels
.PHONY: debug
debug: CXXFLAGS += -DDEBUG -O0
debug: $(CPU_EXECUTABLE)
	@echo "✅ Debug build completed!"

# Clean build artifacts
.PHONY: clean
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(BIN_DIR)
	rm -f results.txt
	@echo "✅ Clean completed!"

# Project statistics
.PHONY: stats
stats:
	@echo "📊 Project Statistics:"
	@echo "CPU Source Lines: $$(wc -l < $(CPU_SRC))"
	@echo "OS Definition Lines: $$(wc -l < $(OS_FILE))"
	@echo "Total C++ Files: $$(find . -name '*.cpp' | wc -l)"
	@echo "Total Header Files: $$(find . -name '*.h' -o -name '*.hpp' | wc -l)"
	@echo "Total OS Files: $$(find . -name '*.txt' | wc -l)"

# Show help
.PHONY: help
help:
	@echo "GTU-C312 Operating System Project"
	@echo "=================================="
	@echo ""
	@echo "Build Targets:"
	@echo "  all          - Build the CPU simulator (default)"
	@echo "  debug        - Build with debug symbols and no optimization"
	@echo "  clean        - Remove all build artifacts"
	@echo ""
	@echo "Execution Targets:"
	@echo "  run          - Run the OS with default settings"
	@echo "  run-final    - Run with final memory dump only"
	@echo "  run-debug    - Run with memory dump after each instruction"
	@echo "  run-step     - Run with step-by-step execution"
	@echo "  run-threads  - Run with thread table monitoring"
	@echo "  run-save     - Run and save output to results.txt"
	@echo ""
	@echo "Testing Targets:"
	@echo "  performance  - Run performance test"
	@echo ""
	@echo "Analysis Targets:"
	@echo "  stats        - Show project statistics"
	@echo ""
	@echo "Utility Targets:"
	@echo "  help         - Show this help message"

# Check if all required files exist
.PHONY: check
check:
	@echo "🔍 Checking project files..."
	@if [ -f "$(CPU_SRC)" ]; then echo "✅ CPU source found"; else echo "❌ CPU source missing"; fi
	@if [ -f "$(OS_FILE)" ]; then echo "✅ OS definition found"; else echo "❌ OS definition missing"; fi
	@if [ -d "$(CPU_DIR)" ]; then echo "✅ CPU directory found"; else echo "❌ CPU directory missing"; fi
	@if [ -d "$(OS_DIR)" ]; then echo "✅ OS directory found"; else echo "❌ OS directory missing"; fi

# Show current configuration
.PHONY: config
config:
	@echo "📋 Current Configuration:"
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "CPU Source: $(CPU_SRC)"
	@echo "OS File: $(OS_FILE)"
	@echo "Executable: $(CPU_EXECUTABLE)" 