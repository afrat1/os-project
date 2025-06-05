# GTU-C312 Operating System Project

A CPU simulator and operating system implementation with multi-threading support, featuring a custom instruction set and thread management system.

## Project Overview

This project implements a simulated CPU and operating system with the following key features:

- Custom CPU simulator with a defined instruction set
- Multi-threading support (up to 10 threads)
- Memory protection (kernel/user mode separation)
- Thread scheduling and context switching
- System calls for I/O and thread management
- Example programs demonstrating sorting, searching, and counting

## Project Structure

```
.
├── CPU/
│   └── gtu_c312_cpu.cpp    # CPU simulator implementation
├── OS/
│   └── os.txt             # OS definition and program code
├── bin/                   # Compiled executable directory
├── Makefile              # Build system
└── README.md             # This file
```

## Building the Project

The project uses a Makefile for building. Available make targets:

```bash
# Build the CPU simulator (default)
make

# Clean build artifacts
make clean

# Show project statistics
make stats

# Check project files
make check

# Show current configuration
make config
```

## Running the Simulator

The simulator can be run with different debug levels:

```bash
# Run with default settings
make run

# Run with final memory dump only
make run-final

# Run with memory dump after each instruction
make run-debug

# Run with step-by-step execution
make run-step

# Run with thread table monitoring
make run-threads

# Run and save output to file
make run-save

# Run performance test
make performance
```

## Features

### CPU Simulator
- Custom instruction set implementation
- Memory management with protection
- Register management (PC, SP)
- System call handling

### Operating System
- Multi-threading support (10 threads)
- Thread scheduling (Round Robin)
- Context switching
- Memory protection (kernel/user mode)
- System calls for:
  - Thread management (YIELD, HLT)
  - I/O operations (PRN)

### Example Programs
1. **Thread 0**: Bubble Sort Implementation
   - Sorts an array of 10 numbers
   - Demonstrates array manipulation and loops

2. **Thread 1**: Linear Search
   - Searches for a value in an array
   - Demonstrates array traversal

3. **Thread 2**: Counter
   - Simple counting program
   - Demonstrates basic arithmetic and loops

## Memory Layout

- **0-20**: System registers and OS area
- **21-999**: OS data area
- **100-199**: Thread table (10 threads, 10 words each)
- **1000-1999**: Thread 0 data and stack
- **2000-2999**: Thread 1 data and stack
- **3000-3999**: Thread 2 data and stack
- **4000-4999**: Thread 3 data and stack
- And so on for remaining threads...

## Instruction Set

The CPU supports the following instructions:
- SET: Set value at address
- CPY: Copy value between addresses
- CPYI: Indirect copy
- CPYI2: Double indirect copy
- ADD: Add value to address
- ADDI: Add indirect
- SUBI: Subtract indirect
- JIF: Conditional jump
- PUSH: Push to stack
- POP: Pop from stack
- CALL: Call subroutine
- RET: Return from subroutine
- USER: Switch to user mode
- SYSCALL: System call
- HLT: Halt execution

## System Calls

- **PRN**: Print value
- **YIELD**: Yield CPU to next thread
- **HLT**: Terminate thread

## Thread States

- 0: Inactive
- 1: Ready
- 2: Running
- 3: Blocked

## Requirements

- C++17 compatible compiler
- Make build system
- Linux/Unix environment

## Author

Ali Fırat Kaya