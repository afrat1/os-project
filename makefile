# Makefile for GTU-C312 CPU Simulator

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g
TARGET = gtu_c312_sim
SOURCE = gtu_c312_cpu.cpp

# Default target
all: $(TARGET)

# Compile the simulator
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Test with the sample program
test: $(TARGET)
	./$(TARGET) test_program.txt

# Test with debug mode 1 (memory after each instruction)
test-debug1: $(TARGET)
	./$(TARGET) test_program.txt -D 1

# Test with debug mode 2 (step by step)
test-debug2: $(TARGET)
	./$(TARGET) test_program.txt -D 2

# Create test program file
test-program:
	@echo "Creating test_program.txt..."
	@echo "# Test program - Sum from 1 to 10" > test_program.txt
	@echo "Begin Data Section" >> test_program.txt
	@echo "0 0     # program counter" >> test_program.txt
	@echo "1 0     # stack pointer" >> test_program.txt
	@echo "2 0     # syscall result" >> test_program.txt
	@echo "3 0     # instruction count" >> test_program.txt
	@echo "50 10   # i = 10 (counter)" >> test_program.txt
	@echo "51 0    # sum = 0 (result)" >> test_program.txt
	@echo "End Data Section" >> test_program.txt
	@echo "" >> test_program.txt
	@echo "Begin Instruction Section" >> test_program.txt
	@echo "0 SET 10 50      # i = 10" >> test_program.txt
	@echo "1 SET 0 51       # sum = 0" >> test_program.txt
	@echo "2 ADDI 51 50     # sum = sum + i (fixed)" >> test_program.txt
	@echo "3 ADD 50 -1      # i = i - 1" >> test_program.txt
	@echo "4 JIF 50 7       # if i <= 0 goto 7" >> test_program.txt
	@echo "5 JMP 2          # Jump back to line 2" >> test_program.txt
	@echo "6 HLT            # This shouldn't execute" >> test_program.txt
	@echo "7 SYSCALL PRN 51 # print the sum (should be 55)" >> test_program.txt
	@echo "8 HLT            # end program" >> test_program.txt
	@echo "End Instruction Section" >> test_program.txt

# Create comprehensive test program
test-comprehensive:
	@echo "Creating comprehensive_test.txt..."
	@echo "# Comprehensive CPU test" > comprehensive_test.txt
	@echo "Begin Data Section" >> comprehensive_test.txt
	@echo "0 0     # program counter" >> comprehensive_test.txt
	@echo "1 0     # stack pointer" >> comprehensive_test.txt
	@echo "2 0     # syscall result" >> comprehensive_test.txt
	@echo "3 0     # instruction count" >> comprehensive_test.txt
	@echo "1000 42   # test value in user space" >> comprehensive_test.txt
	@echo "1001 0    # result storage" >> comprehensive_test.txt
	@echo "1002 5    # test value 2" >> comprehensive_test.txt
	@echo "End Data Section" >> comprehensive_test.txt
	@echo "" >> comprehensive_test.txt
	@echo "Begin Instruction Section" >> comprehensive_test.txt
	@echo "0 SET 10 1001      # Set result = 10" >> comprehensive_test.txt
	@echo "1 CPY 1000 1001    # Copy test value to result" >> comprehensive_test.txt
	@echo "2 ADDI 1001 1002   # Add test value 2 to result" >> comprehensive_test.txt
	@echo "3 PUSH 1001        # Push result to stack" >> comprehensive_test.txt
	@echo "4 SET 0 1001       # Clear result" >> comprehensive_test.txt
	@echo "5 POP 1001         # Pop back to result" >> comprehensive_test.txt
	@echo "6 SYSCALL PRN 1001 # Print result (should be 47)" >> comprehensive_test.txt
	@echo "7 HLT              # End program" >> comprehensive_test.txt
	@echo "End Instruction Section" >> comprehensive_test.txt

# Test comprehensive program
test-comp: $(TARGET)
	./$(TARGET) comprehensive_test.txt

# Clean up
clean:
	rm -f $(TARGET) test_program.txt comprehensive_test.txt

# Help
help:
	@echo "Available targets:"
	@echo "  all           - Compile the simulator"
	@echo "  test          - Run basic test"
	@echo "  test-debug1   - Run test with debug mode 1"
	@echo "  test-debug2   - Run test with debug mode 2" 
	@echo "  test-program  - Create test program file"
	@echo "  test-comprehensive - Create comprehensive test program"
	@echo "  test-comp     - Run comprehensive test"
	@echo "  clean         - Remove compiled files"
	@echo "  help          - Show this help"

.PHONY: all test test-debug1 test-debug2 test-program test-comprehensive test-comp clean help