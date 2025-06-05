#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>
#include <iomanip>
#include <stdexcept>
#include <map>

using namespace std;

class GTUC312CPU {
private:
    vector<long> memory;
    map<long, string> instructions; // Changed from vector to map for address-based lookup
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
        
        // Look up instruction by PC address in the map
        if (instructions.find(pc) == instructions.end()) {
            cout << "Error: No instruction at PC address: " << pc << endl;
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
        for (const auto& pair : instructions) {
            cout << "Addr " << setw(4) << pair.first << ": " << pair.second << endl;
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
        for (int thread = 0; thread < 10; thread++) {  // Show all 10 threads
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
        // Extract instruction address from the line (e.g. "1000 SYSCALL PRN 9999")
        istringstream iss(line);
        long address;
        if (iss >> address) {
            // Store the rest of the line as the instruction at this address
            string instruction = line;
            instructions[address] = instruction;
        }
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
            
            // Get current thread's initial SP to check if stack is not empty
            long currentThread = memory[21];  // Current thread ID
            long threadBaseAddr = 100 + (currentThread * 10);
            long threadInitialSP = memory[threadBaseAddr + 5];  // Thread's initial SP
            
            if (sp < threadInitialSP) { // Check if stack is not empty (SP < initial means items on stack)
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
            
            // Get current thread's initial SP to check if there's a return address on stack
            long currentThread = memory[21];  // Current thread ID
            long threadBaseAddr = 100 + (currentThread * 10);
            long threadInitialSP = memory[threadBaseAddr + 5];  // Thread's initial SP
            
            if (sp < threadInitialSP) { // Check if there's a return address on stack
                memory[PC_ADDR] = memory[sp];
                memory[SP_ADDR] = sp + 1;
            } else {
                memory[PC_ADDR]++;
            }
            
        } else if (cmd == "USER") {
            long address;
            iss >> address;
            // Stay in kernel mode to read the address
            checkMemoryAccess(address, false);
            long jumpAddress = memory[address]; // Read jump address while in kernel mode
            
            // Initialize stack pointer from current thread's table entry
            long currentThread = memory[21];  // Current thread ID
            if (currentThread >= 0 && currentThread < 10) {
                long threadBaseAddr = 100 + (currentThread * 10);  // Each thread uses 10 memory slots
                memory[SP_ADDR] = memory[threadBaseAddr + 5];  // Load thread's SP from table
            }
            
            kernelMode = false; // Now switch to user mode
            memory[PC_ADDR] = jumpAddress; // Jump to the address
            
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
                // THREAD TERMINATION - Not system halt
                long currentThread = memory[21];  // Current thread ID
                long threadBaseAddr = 100 + (currentThread * 10);
                
                // Set current thread to inactive
                memory[threadBaseAddr + 3] = 0;  // State = 0 (Inactive)
                
                // Check if any threads are still active (check all 10 threads)
                bool anyActive = false;
                for (int i = 0; i < 10; i++) {  // Check all 10 thread table entries
                    long checkThreadBase = 100 + (i * 10);
                    if (memory[checkThreadBase + 3] != 0) {  // Not inactive
                        anyActive = true;
                        break;
                    }
                }
                
                if (!anyActive) {
                    // All threads terminated - halt the system
                    halted = true;
                } else {
                    // Find next active thread (check all 10 threads)
                    long nextThread = (currentThread + 1) % 10;  // Round robin through all 10
                    int attempts = 0;
                    while (attempts < 10) {  // Try all 10 thread slots
                        long checkThreadBase = 100 + (nextThread * 10);
                        if (memory[checkThreadBase + 3] == 1 || memory[checkThreadBase + 3] == 2) {  // Ready or Running
                            break;
                        }
                        nextThread = (nextThread + 1) % 10;
                        attempts++;
                    }
                    
                    if (attempts >= 10) {
                        // No active threads found
                        halted = true;
                    } else {
                        // Switch to next thread
                        memory[21] = nextThread;
                        long nextThreadBase = 100 + (nextThread * 10);
                        memory[nextThreadBase + 3] = 2;  // Set to running
                        
                        // Restore context
                        memory[PC_ADDR] = memory[nextThreadBase + 4];
                        memory[SP_ADDR] = memory[nextThreadBase + 5];
                        kernelMode = false;  // Return to user mode
                    }
                }
                
            } else if (syscallType == "YIELD") {
                // PROPER THREAD SWITCHING IMPLEMENTATION
                
                // 1. Save current thread's context to thread table
                long currentThread = memory[21];  // Current thread ID (address 21)
                
                // Bounds check for thread ID
                if (currentThread < 0 || currentThread >= 10) {
                    cout << "Error: Invalid current thread ID: " << currentThread << endl;
                    memory[PC_ADDR]++;
                    return;
                }
                
                long threadBaseAddr = 100 + (currentThread * 10);  // Each thread uses 10 memory slots
                
                // Save current thread's PC and SP to thread table
                memory[threadBaseAddr + 4] = memory[PC_ADDR] + 1;  // Save next PC
                memory[threadBaseAddr + 5] = memory[SP_ADDR];      // Save SP
                memory[threadBaseAddr + 2]++;                      // Increment instruction count for this thread
                
                // 2. Find next ready thread (round robin through all 10 threads)
                long nextThread = (currentThread + 1) % 10;  // Round robin through all 10
                
                // Skip inactive threads (check all 10 thread slots)
                int attempts = 0;
                while (attempts < 10) {  // Try all 10 thread slots
                    long checkThreadBase = 100 + (nextThread * 10);
                    if (memory[checkThreadBase + 3] == 1 || memory[checkThreadBase + 3] == 2) {  // State 1=Ready or 2=Running
                        break;  // Found ready thread
                    }
                    nextThread = (nextThread + 1) % 10;
                    attempts++;
                }
                
                // If no ready threads found, halt
                if (attempts >= 10) {
                    cout << "No ready threads found - halting system" << endl;
                    halted = true;
                    return;
                }
                
                // 3. Switch to next thread
                memory[21] = nextThread;  // Update current thread
                long nextThreadBase = 100 + (nextThread * 10);
                
                // Set current thread to ready, next thread to running
                memory[threadBaseAddr + 3] = 1;      // Current thread -> Ready
                memory[nextThreadBase + 3] = 2;      // Next thread -> Running
                
                // 4. Restore next thread's context
                memory[PC_ADDR] = memory[nextThreadBase + 4];  // Restore PC
                memory[SP_ADDR] = memory[nextThreadBase + 5];  // Restore SP
                
                // 5. Return to user mode for thread execution
                kernelMode = false;
                
                // Don't increment PC here - we set it to the restored value
            } else if (cmd == "HLT") {
                halted = true;
                
            } else {
                cout << "Error: Unknown instruction: " << cmd << endl;
                memory[PC_ADDR]++;
            }
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