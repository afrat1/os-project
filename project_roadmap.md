# GTU-C312 Operating System Project Roadmap

## Phase 1: Operating System Core (Week 1-2)
### 1.1 Thread Table Implementation
- [ ] Design thread table structure (10 entries)
- [ ] Initialize thread states and registers
- [ ] Implement thread creation and management

### 1.2 Scheduler Implementation
- [ ] Round-robin scheduling algorithm
- [ ] Context switching mechanism
- [ ] Time slice management

### 1.3 System Call Infrastructure
- [ ] SYSCALL YIELD handler
- [ ] SYSCALL HLT handler
- [ ] System call dispatcher

## Phase 2: Thread Programs (Week 2-3)
### 2.1 Sorting Thread (Address 1000-1999)
- [ ] Bubble sort or selection sort algorithm
- [ ] Input: N numbers from data segment
- [ ] Output: Sorted numbers via PRN

### 2.2 Search Thread (Address 2000-2999)
- [ ] Linear search algorithm
- [ ] Input: N numbers + search key
- [ ] Output: Found position via PRN

### 2.3 Custom Thread (Address 3000-3999)
- [ ] Design custom algorithm (suggestions: factorial, fibonacci, prime numbers)
- [ ] Include loops and conditional logic
- [ ] Multiple PRN outputs

## Phase 3: Integration & Enhanced Features (Week 3-4)
### 3.1 Complete OS Integration
- [ ] OS bootup and thread initialization
- [ ] Memory layout management
- [ ] Inter-thread communication (if needed)

### 3.2 Enhanced Debugging
- [ ] Debug Mode 3: Thread table printing
- [ ] Thread execution statistics
- [ ] Performance monitoring

### 3.3 Advanced Features
- [ ] Thread blocking for PRN (100 instruction cycles)
- [ ] Error recovery and thread termination
- [ ] Resource management

## Phase 4: Testing & Documentation (Week 4)
### 4.1 Comprehensive Testing
- [ ] Unit tests for each thread
- [ ] Integration tests for OS
- [ ] Performance benchmarks

### 4.2 Documentation
- [ ] Technical report
- [ ] User manual
- [ ] AI interaction logs

## Current Status: ✅ CPU Simulator Complete (25% done)
## Next Milestone: 🎯 OS Core Implementation 