
#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <syscalls.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>

#include <drivers/amd_am79c973.h>
#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/tcp.h>


// #define GRAPHICSMODE


using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;
using namespace myos::net;


void intToString(int num, char* str) {
    int i = 0;
    bool isNegative = false;

    // Handle negative numbers
    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    // Extract digits in reverse order
    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while (num != 0);

    // Add '-' for negative numbers
    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Null terminate the string

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}



void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void printInt(int num) {
    char str[12];
    intToString(num, str);
    printf(str);
}
void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}
void printfHex16(uint16_t key)
{
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}
void printfHex32(uint32_t key)
{
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}





class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(char c)
    {
        char* foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
public:
    
    MouseToConsole()
    {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);        
    }
    
    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

        x += xoffset;
        if(x >= 80) x = 79;
        if(x < 0) x = 0;
        y += yoffset;
        if(y >= 25) y = 24;
        if(y < 0) y = 0;

        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }
    
};

class PrintfUDPHandler : public UserDatagramProtocolHandler
{
public:
    void HandleUserDatagramProtocolMessage(UserDatagramProtocolSocket* socket, common::uint8_t* data, common::uint16_t size)
    {
        char* foo = " ";
        for(int i = 0; i < size; i++)
        {
            foo[0] = data[i];
            printf(foo);
        }
    }
};


class PrintfTCPHandler : public TransmissionControlProtocolHandler
{
public:
    bool HandleTransmissionControlProtocolMessage(TransmissionControlProtocolSocket* socket, common::uint8_t* data, common::uint16_t size)
    {
        char* foo = " ";
        for(int i = 0; i < size; i++)
        {
            foo[0] = data[i];
            printf(foo);
        }
        
        
        
        if(size > 9
            && data[0] == 'G'
            && data[1] == 'E'
            && data[2] == 'T'
            && data[3] == ' '
            && data[4] == '/'
            && data[5] == ' '
            && data[6] == 'H'
            && data[7] == 'T'
            && data[8] == 'T'
            && data[9] == 'P'
        )
        {
            socket->Send((uint8_t*)"HTTP/1.1 200 OK\r\nServer: MyOS\r\nContent-Type: text/html\r\n\r\n<html><head><title>My Operating System</title></head><body><b>My Operating System</b> http://www.AlgorithMan.de</body></html>\r\n",184);
            socket->Disconnect();
        }
        
        
        return true;
    }
};


void sysprintfA(char* str)
{
    asm("int $0x80" : : "a" (10), "b" (str));
}

void sysprintfB(char* str)
{
    int deneme = 1;
    asm("int $0x80" : "=c" (deneme) : "a" (12), "b" (str));
}
int getPid() {
    int pid;
    asm("int $0x80" : "=c" (pid) : "a" (SYSCALLS::GETPID));
    return pid;
}
void execve(void (*entrypoint)()) {
    asm("int $0x80" : : "a" (SYSCALLS::EXECVE), "b" (entrypoint));
}
void exit() {
    asm("int $0x80" : : "a" (SYSCALLS::EXIT));
}
int fork(int *pid) {
    asm("int $0x80" : "=c" (*pid) : "a" (SYSCALLS::FORK));
    return *pid;
}
void waitPid(int pid) {
    asm("int $0x80" : : "a" (SYSCALLS::WAITPID), "b" (pid));
}

void sleep(int seconds) {
    const int iterations = 100000000;
    for (int sec = 0; sec < seconds; ++sec) {
        for (int i = 0; i < iterations; ++i) {
            ;
        }
    }
}
int long_running_program(int n){ 
    int result = 0;
    for(int i=0;i<n;++i){
        for(int j=0;j<n;++j){
            result += i*j;
        }
    }
    return result;
}
void collatz(int n, char* str) {
    printf(str);
    printInt(n); // Print the current number
    printf(str);
    printf(" ");
    while (n != 1) {
        sleep(1);
        if (n % 2 == 0) {
            // If n is even, divide it by 2
            n = n / 2;
        } else {
            // If n is odd, multiply it by 3 and add 1
            n = 3 * n + 1;
        }
        printf(str);
        printInt(n); // Print the current number
        printf(str);
        printf(" ");
    }
}
void taskExecve() {
    printf(" *EXECVE FUNC* \n");
    while(true)
        ;
}
void taskExecveGetPid(){
    int pid = getPid();
    printf("*Pid: ");
    printInt(pid);
    printf("  ");
    execve(taskExecve);
}

void callLRPWithFork(int (*func)(int), int num){
    int result;
    int pid;
    fork(&pid);
    if(pid == 0){
        result = func(num);
        printf(" *LRP Result: ");
        printInt(result);
        printf(" * ");
        exit();
    }
}

void callCollatzWithFork(void (*func)(int,char*), int num, char* str){
    int pid;
    fork(&pid);
    if(pid == 0){
        func(num, str);
        exit();
    }
}

void partA() {
    char *c[3] = {"a", "b", "c"};
    for(int i=0;i<3;++i) {
        callCollatzWithFork(collatz, (i+1)*30, c[i]);
    }
    for(int i=0 ;i<3;++i) {
        callLRPWithFork(long_running_program, (i+1)*2);
    }
    for(int i=1;i<=6;++i) {
        waitPid(i);
    }
    while(true)
        ;
}

void strategy_1(){
    char *c[10] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    for (int i = 1; i <= 10 ; i++)
    {
        callCollatzWithFork(collatz, i, c[i-1]);
    }
    for(int i=1;i<=10;++i) {
        waitPid(i);
    }

    printf("=========== ST-1 End ==========");
    while(true)
        ;
}

void strategy_2()
{   
    char *c[3] = {"a", "b", "c"};
    for (int i = 1; i <= 3; i++)
    {
        callLRPWithFork(long_running_program, i);
    }
     for (int i = 1; i <= 3; i++)
    {
        callCollatzWithFork(collatz, i, c[i-1]);
    }
    for(int i=1;i<=6;++i) {
        waitPid(i);
    }
    printf("=========== ST-2 End ==========");
    while(true)
        ;
    
}

// An iterative binary search function.
int binarySearch(int arr[], int low, int high, int x)
{
    while (low <= high) {
        int mid = low + (high - low) / 2;

        // Check if x is present at mid
        if (arr[mid] == x)
            return mid;

        // If x greater, ignore left half
        if (arr[mid] < x)
            low = mid + 1;

        // If x is smaller, ignore right half
        else
            high = mid - 1;
    }

    // If we reach here, then element was not present
    return -1;
}

void binarySearchProgram() {
    int arr[] = { 2, 3, 4, 10, 40 };
    int query = 10;
    int n = sizeof(arr) / sizeof(arr[0]);
    int result = binarySearch(arr, 0, n - 1, query);
    if(result == -1) {
        printf(" *Element doesn't exist in the array.(Binary Search)* ");
    }
    else {
        printf(" *The Element(10) is found, index: ");
        printInt(result);
        printf(".(Binary Search)* ");
    }
    exit();
}

int linearSearch(int arr[], int N, int x)
{
    for (int i = 0; i < N; i++)
        if (arr[i] == x)
            return i;
    return -1;
}

void linearSearchProgram() {
    int arr[] = { 2, 3, 4, 10, 40 };
    int x = 40 ;
    int N = sizeof(arr) / sizeof(arr[0]);

    // Function call
    int result = linearSearch(arr, N, x);
    if(result == -1) {
        printf(" *Element doesn't exist in the array.(LinearSearch)* ");
    }
    else {
        printf(" *The Element(40) is found, index: ");
        printInt(result);
        printf(".(Linear Search)* ");
    }
    exit();
}

void collatzProgram() {
    collatz(200, "-");
    exit();
}

void lrpProgram() {
    int result = long_running_program(4);
    printf("  LRP Result: ");
    printInt(result);
    printf("  ");
    exit();
}

void strategy_3()
{
    int pid1,pid2,pid3,pid4,pid5;
    fork(&pid1);
    if(pid1 == 0) {
        collatzProgram();
    }
    else {
        fork(&pid2);
        if(pid2 == 0) {
            linearSearchProgram();          
        }
        else {
            fork(&pid3);
            if(pid3 == 0) {
                binarySearchProgram();
            }
            else {
                fork(&pid4);
                if(pid4 == 0) {
                    lrpProgram();
                }
                else {
                    fork(&pid5);
                    if(pid5 == 0){
                        collatz(200, "*");         
                        exit();
                    }
                }
            }
        }
    }
    waitPid(pid1);
    waitPid(pid2);
    waitPid(pid3);
    waitPid(pid4);
    waitPid(pid5);
    printf(" END ");
    while(true)
        ;
}
void strategy_dynamic() {
    int pid1,pid2,pid3,pid4,pid5;
    fork(&pid1);
    if(pid1 == 0) {
        collatzProgram();
    }
    else {
        fork(&pid2);
        if(pid2 == 0) {
            linearSearchProgram();
        }
        else {
            fork(&pid3);
            if(pid3 == 0) {
                binarySearchProgram();
            }
            else {
                fork(&pid4);
                if(pid4 == 0) {
                    lrpProgram();
                }
                else {
                    fork(&pid5);
                    if(pid5 == 0){
                        collatz(200, "*");         
                        exit();
                    }
                }
            }
        }
    }
    waitPid(pid1);
    waitPid(pid2);
    waitPid(pid3);
    waitPid(pid4);
    waitPid(pid5);
    printf(" END ");
    while(true)
        ;
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}


// Define the ports for the PIT
#define PIT_CONTROL_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
// Base frequency of the PIT (1.193182 MHz)
#define PIT_BASE_FREQUENCY 1193182
// Function to set the PIT frequency
void setPITFrequency(uint32_t frequency) {
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;  // Calculate the divisor

    // Send command byte to PIT control port
    Port8Bit controlPort(PIT_CONTROL_PORT);
    controlPort.Write(0x36);

    // Send frequency divisor to PIT channel 0 data port
    Port8Bit channel0Port(PIT_CHANNEL0_PORT);
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

    channel0Port.Write(low);
    channel0Port.Write(high);
}
extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{

    GlobalDescriptorTable gdt;
    
    
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);
    
    void* allocated = memoryManager.malloc(1024);
    
    TaskManager taskManager(&gdt);
    
    // to test, please create only one task and add it to the manager(comment out the task you will test).

    Task taskPartA(&gdt, partA, 0, 0, 3);
    //Task st_1(&gdt, strategy_1, 0, 0, 3);
    //Task st_2(&gdt, strategy_2, 0, 0, 3);
    //Task st_3(&gdt, strategy_3, 0, 0, 3);
    //Task st_dynamic(&gdt, strategy_dynamic, 0, 0, 10);
    //Task testpidexecve(&gdt, taskExecveGetPid, 0, 0, 3);

    taskManager.AddTask(&taskPartA);
    //taskManager.AddTask(&st_1);
    //taskManager.AddTask(&st_2);
    //taskManager.AddTask(&st_3);
    //taskManager.AddTask(&st_dynamic);
    //taskManager.AddTask(&testpidexecve);

    InterruptManager interrupts(0x20, &gdt, &taskManager);
    SyscallHandler syscalls(&interrupts, 0x80);
    
    //setPITFrequency(10000);

    #ifdef GRAPHICSMODE
        Desktop desktop(320,200, 0x00,0x00,0xA8);
    #endif
    
    DriverManager drvManager;
    
        #ifdef GRAPHICSMODE
            KeyboardDriver keyboard(&interrupts, &desktop);
        #else
            PrintfKeyboardEventHandler kbhandler;
            KeyboardDriver keyboard(&interrupts, &kbhandler);
        #endif
        drvManager.AddDriver(&keyboard);
        
    
        #ifdef GRAPHICSMODE
            MouseDriver mouse(&interrupts, &desktop);
        #else
            MouseToConsole mousehandler;
            MouseDriver mouse(&interrupts, &mousehandler);
        #endif
        drvManager.AddDriver(&mouse);
        
        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);

        #ifdef GRAPHICSMODE
            VideoGraphicsArray vga;
        #endif
        
        drvManager.ActivateAll();
        

    #ifdef GRAPHICSMODE
        vga.SetMode(320,200,8);
        Window win1(&desktop, 10,10,20,20, 0xA8,0x00,0x00);
        desktop.AddChild(&win1);
        Window win2(&desktop, 40,15,30,30, 0x00,0xA8,0x00);
        desktop.AddChild(&win2);
    #endif
        
    amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);

    interrupts.Activate();
    
    while(1)
    {
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }
}
