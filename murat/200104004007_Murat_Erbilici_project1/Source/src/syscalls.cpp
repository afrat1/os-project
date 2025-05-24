
#include <syscalls.h>
 
using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;
 
SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber)
:    InterruptHandler(interruptManager, InterruptNumber  + interruptManager->HardwareInterruptOffset())
{
}

SyscallHandler::~SyscallHandler()
{
}


void printf(char*);
void printInt(int);



uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
    CPUState* cpu = (CPUState*)esp;
    
    switch(cpu->eax)
    {
        case SYSCALLS::GETPID:
            cpu->ecx = InterruptHandler::syscallGetPID();
            return InterruptHandler::HandleInterrupt(esp);
            break;
        case SYSCALLS::EXIT:
            InterruptHandler::syscallExit();
            return InterruptHandler::HandleInterrupt(esp);
            break;
        case SYSCALLS::FORK:
        {
            cpu->ecx = InterruptHandler::syscallFork(cpu);
            return InterruptHandler::HandleInterrupt((uint32_t)esp);
            break;
        }
        case SYSCALLS::EXECVE:
            esp = InterruptHandler::syscallExecve((void (*)()) cpu->ebx);
            break;
        case SYSCALLS::WAITPID:
            if(InterruptHandler::syscallWaitPid(cpu))
                return InterruptHandler::HandleInterrupt((uint32_t)esp);
            break;
        default:
            break;
    }

    
    return esp;
}

