#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>
#include <iomanip>
#include <stdexcept>

using namespace std;

class GTUC312CPU {
private:
    vector<long> memory;
    vector<string> instructions;
    bool halted;
    bool kernelMode;
    
    // Memory layout constants
    static const int PC_ADDR = 0;
    static const int SP_ADDR = 1;
    static const int SYSCALL_RESULT_ADDR = 2;
    static const int INSTRUCTION_COUNT_ADDR = 3;
    static const int KERNEL_BOUNDARY = 1000;
    
public:
    GTUC312CPU(int memorySize = 50000) : memory(memorySize, 0), halted(false), kernelMode(true) {
        // Initialize special registers
        memory[PC_ADDR] = 0;        // Program Counter
        memory[SP_ADDR] = 500;      // Stack Pointer - start after OS data area
        memory[SYSCALL_RESULT_ADDR] = 0;  // System call result
        memory[INSTRUCTION_COUNT_ADDR] = 0; // Instruction count
    }
    
    bool loadProgram(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Cannot open file " << filename << endl;
            return false;
        }
        
        string line;
        bool inDataSection = false;
        bool inInstructionSection = false;
        
        while (getline(file, line)) {
            // Remove comments
            size_t commentPos = line.find('#');
            if (commentPos != string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Trim whitespace
            line = trim(line);
            if (line.empty()) continue;
            
            // Check section markers
            if (line == "Begin Data Section") {
                inDataSection = true;
                continue;
            } else if (line == "End Data Section") {
                inDataSection = false;
                continue;
            } else if (line == "Begin Instruction Section") {
                inInstructionSection = true;
                continue;
            } else if (line == "End Instruction Section") {
                inInstructionSection = false;
                continue;
            }
            
            // Parse data section
            if (inDataSection) {
                parseDataLine(line);
            }
            
            // Parse instruction section
            if (inInstructionSection) {
                parseInstructionLine(line);
            }
        }
        
        file.close();
        return true;
    }
    
    void execute() {
        if (halted) return;
        
        long pc = memory[PC_ADDR];
        if (pc < 0 || pc >= static_cast<long>(instructions.size())) {
            cout << "Error: PC out of bounds: " << pc << endl;
            halted = true;
            return;
        }
        
        string instruction = instructions[pc];
        executeInstruction(instruction);
        
        // Increment instruction count
        memory[INSTRUCTION_COUNT_ADDR]++;
    }
    
    bool isHalted() const {
        return halted;
    }
    
    void printMemory(ostream& out = cerr) const {
        out << "=== Memory Contents (Instruction #" << memory[INSTRUCTION_COUNT_ADDR] << ") ===" << endl;
        out << "PC=" << memory[PC_ADDR] << ", SP=" << memory[SP_ADDR] 
            << ", SYSCALL_RESULT=" << memory[SYSCALL_RESULT_ADDR] << endl;
        
        for (size_t i = 0; i < memory.size(); i++) {
            if (memory[i] != 0 || i < 21) { // Show non-zero values and important registers
                out << "Memory[" << setw(4) << i << "] = " << setw(8) << memory[i] << endl;
            }
        }
        out << "===============================================" << endl;
    }
    
    void printInstructions() const {
        cout << "=== Loaded Instructions ===" << endl;
        for (size_t i = 0; i < instructions.size(); i++) {
            cout << setw(3) << i << ": " << instructions[i] << endl;
        }
        cout << "=========================" << endl;
    }
    
    void printStatistics() const {
        cout << "=== CPU Statistics ===" << endl;
        cout << "Instructions executed: " << memory[INSTRUCTION_COUNT_ADDR] << endl;
        cout << "Stack pointer: " << memory[SP_ADDR] << endl;
        cout << "Final PC: " << memory[PC_ADDR] << endl;
        cout << "Kernel mode: " << (kernelMode ? "Yes" : "No") << endl;
        cout << "======================" << endl;
    }
    
    void printThreadTable(ostream& out = cerr) const {
        out << "=== Thread Table (Instruction #" << memory[INSTRUCTION_COUNT_ADDR] << ") ===" << endl;
        out << "Current Thread: " << memory[21] << endl;  // Address 21 stores current thread
        out << "Active Threads: " << memory[22] << endl;  // Address 22 stores number of active threads
        out << "System Tick: " << memory[23] << endl;     // Address 23 stores system tick
        out << endl;
        
        // Print thread table (10 threads, 10 words each, starting at address 100)
        for (int thread = 0; thread < 6; thread++) {  // Show first 6 threads
            int baseAddr = 100 + (thread * 10);
            if (memory[baseAddr + 3] != 0) { // Only show active threads (state != 0)
                out << "Thread " << thread << ":" << endl;
                out << "  ID: " << memory[baseAddr + 0] << endl;
                out << "  Start Time: " << memory[baseAddr + 1] << endl;
                out << "  Instr Count: " << memory[baseAddr + 2] << endl;
                out << "  State: " << memory[baseAddr + 3];
                switch((int)memory[baseAddr + 3]) {
                    case 0: out << " (Inactive)"; break;
                    case 1: out << " (Ready)"; break;
                    case 2: out << " (Running)"; break;
                    case 3: out << " (Blocked)"; break;
                    default: out << " (Unknown)"; break;
                }
                out << endl;
                out << "  PC: " << memory[baseAddr + 4] << endl;
                out << "  SP: " << memory[baseAddr + 5] << endl;
                out << endl;
            }
        }
        out << "=============================================" << endl;
    }

private:
    string trim(const string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    void parseDataLine(const string& line) {
        istringstream iss(line);
        long address, value;
        if (iss >> address >> value) {
            if (address >= 0 && address < static_cast<long>(memory.size())) {
                memory[address] = value;
            }
        }
    }
    
    void parseInstructionLine(const string& line) {
        instructions.push_back(line);
    }
    
    void executeInstruction(const string& instruction) {
        istringstream iss(instruction);
        string lineNum, cmd;
        if (!(iss >> lineNum >> cmd)) {
            cout << "Error: Invalid instruction format: " << instruction << endl;
            halted = true;
            return;
        }
        
        // Check memory access permissions
        if (!kernelMode) {
            // In user mode, can only access memory >= 1000
            // This will be checked in individual instruction implementations
        }
        
        if (cmd == "SET") {
            long value, address;
            iss >> value >> address;
            checkMemoryAccess(address, true);
            memory[address] = value;
            memory[PC_ADDR]++;
            
        } else if (cmd == "CPY") {
            long src, dest;
            iss >> src >> dest;
            checkMemoryAccess(src, false);
            checkMemoryAccess(dest, true);
            memory[dest] = memory[src];
            memory[PC_ADDR]++;
            
        } else if (cmd == "CPYI") {
            long srcAddr, dest;
            iss >> srcAddr >> dest;
            checkMemoryAccess(srcAddr, false);
            long actualSrc = memory[srcAddr];
            checkMemoryAccess(actualSrc, false);
            checkMemoryAccess(dest, true);
            memory[dest] = memory[actualSrc];
            memory[PC_ADDR]++;
            
        } else if (cmd == "CPYI2") {
            long srcAddr, destAddr;
            iss >> srcAddr >> destAddr;
            checkMemoryAccess(srcAddr, false);
            checkMemoryAccess(destAddr, false);
            long actualSrc = memory[srcAddr];
            long actualDest = memory[destAddr];
            checkMemoryAccess(actualSrc, false);
            checkMemoryAccess(actualDest, true);
            memory[actualDest] = memory[actualSrc];
            memory[PC_ADDR]++;
            
        } else if (cmd == "ADD") {
            long address, value;
            iss >> address >> value;
            checkMemoryAccess(address, true);
            memory[address] += value;
            memory[PC_ADDR]++;
            
        } else if (cmd == "ADDI") {
            long addr1, addr2;
            iss >> addr1 >> addr2;
            checkMemoryAccess(addr1, true);
            checkMemoryAccess(addr2, false);
            memory[addr1] += memory[addr2];
            memory[PC_ADDR]++;
            
        } else if (cmd == "SUBI") {
            long addr1, addr2;
            iss >> addr1 >> addr2;
            checkMemoryAccess(addr1, false); // addr1 will be read from
            checkMemoryAccess(addr2, true);  // addr2 will be written to (receives result)
            memory[addr2] = memory[addr1] - memory[addr2]; // A1 - A2 -> A2 (correct according to project spec)
            memory[PC_ADDR]++;
            
        } else if (cmd == "JIF") {
            long conditionAddr, jumpAddr;
            iss >> conditionAddr >> jumpAddr;
            checkMemoryAccess(conditionAddr, false);
            if (memory[conditionAddr] <= 0) {
                memory[PC_ADDR] = jumpAddr;
            } else {
                memory[PC_ADDR]++;
            }
            
        } else if (cmd == "PUSH") {
            long address;
            iss >> address;
            checkMemoryAccess(address, false);
            // Use memory-based stack - stack grows downwards
            long sp = memory[SP_ADDR];
            memory[SP_ADDR] = sp - 1; // Decrement SP first
            memory[sp - 1] = memory[address]; // Then store value
            memory[PC_ADDR]++;
            
        } else if (cmd == "POP") {
            long address;
            iss >> address;
            checkMemoryAccess(address, true);
            long sp = memory[SP_ADDR];
            if (sp < 500) { // Check if stack is not empty (assuming 500 is initial SP)
                memory[address] = memory[sp];
                memory[SP_ADDR] = sp + 1; // Increment SP after pop
            }
            memory[PC_ADDR]++;
            
        } else if (cmd == "CALL") {
            long subroutineAddr;
            iss >> subroutineAddr;
            // Push return address to memory stack - stack grows downwards
            long sp = memory[SP_ADDR];
            memory[SP_ADDR] = sp - 1;
            memory[sp - 1] = memory[PC_ADDR] + 1;
            memory[PC_ADDR] = subroutineAddr;
            
        } else if (cmd == "RET") {
            long sp = memory[SP_ADDR];
            if (sp < 500) { // Check if there's a return address on stack
                memory[PC_ADDR] = memory[sp];
                memory[SP_ADDR] = sp + 1;
            } else {
                memory[PC_ADDR]++;
            }
            
        } else if (cmd == "USER") {
            long address;
            iss >> address;
            kernelMode = false;
            checkMemoryAccess(address, false);
            memory[PC_ADDR] = memory[address]; // Jump to address stored at 'address'
            
        } else if (cmd == "SYSCALL") {
            string syscallType;
            iss >> syscallType;
            kernelMode = true; // Switch to kernel mode for system calls
            
            if (syscallType == "PRN") {
                long address;
                iss >> address;
                checkMemoryAccess(address, false);
                cout << memory[address] << endl;
                // Note: Blocking for 100 instructions will be handled by OS
                memory[PC_ADDR]++;
                
            } else if (syscallType == "HLT") {
                halted = true;
                
            } else if (syscallType == "YIELD") {
                // Simple YIELD: just continue to next instruction
                // This creates cooperative multitasking by allowing other operations
                memory[PC_ADDR]++;
            }
            
        } else if (cmd == "HLT") {
            halted = true;
            
        } else {
            cout << "Error: Unknown instruction: " << cmd << endl;
            memory[PC_ADDR]++;
        }
    }
    
    void checkMemoryAccess(long address, bool /* isWrite */) {
        if (address < 0 || address >= static_cast<long>(memory.size())) {
            throw runtime_error("Memory access out of bounds: " + to_string(address));
        }
        
        if (!kernelMode && address < KERNEL_BOUNDARY) {
            cout << "Error: User mode access violation at address " << address << endl;
            halted = true;
            throw runtime_error("Memory protection violation");
        }
    }
};

// Simple test program
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <program_file> [-D debug_level]" << endl;
        cout << "Debug levels: 0=final memory, 1=memory after each instruction, 2=step-by-step, 3=thread table" << endl;
        return 1;
    }
    
    string filename = argv[1];
    int debugLevel = 0;
    
    if (argc >= 4 && string(argv[2]) == "-D") {
        debugLevel = stoi(argv[3]);
    }
    
    GTUC312CPU cpu;
    
    if (!cpu.loadProgram(filename)) {
        return 1;
    }
    
    cout << "Program loaded successfully!" << endl;
    cpu.printInstructions();
    
    cout << "\n=== Starting Execution ===" << endl;
    
    while (!cpu.isHalted()) {
        cpu.execute();
        
        switch (debugLevel) {
            case 1:
                cpu.printMemory(cerr);
                break;
            case 2:
                cpu.printMemory(cerr);
                cout << "Press Enter to continue...";
                cin.get();
                break;
            case 3:
                cpu.printThreadTable(cerr);
                break;
        }
    }
    
    cout << "\n=== Execution Complete ===" << endl;
    
    if (debugLevel == 0) {
        cpu.printMemory(cerr);
    }
    
    cpu.printStatistics();
    
    return 0;
}