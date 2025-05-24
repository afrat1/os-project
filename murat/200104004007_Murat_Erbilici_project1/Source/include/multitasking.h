 
#ifndef __MYOS__MULTITASKING_H
#define __MYOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

enum TaskStatus{READY, BLOCKED, RUNNING, FINISHED};

namespace myos
{
    
    struct CPUState
    {
        common::uint32_t eax;
        common::uint32_t ebx;
        common::uint32_t ecx;
        common::uint32_t edx;

        common::uint32_t esi;
        common::uint32_t edi;
        common::uint32_t ebp;

        /*
        common::uint32_t gs;
        common::uint32_t fs;
        common::uint32_t es;
        common::uint32_t ds;
        */
        common::uint32_t error;

        common::uint32_t eip;
        common::uint32_t cs;
        common::uint32_t eflags;
        common::uint32_t esp;
        common::uint32_t ss;        
    } __attribute__((packed));
    
    class Task
    {
    friend class TaskManager;
    private:
        static common::uint32_t nextPID;
        common::uint8_t stack[4096]; // 4 KiB
        CPUState* cpustate;
        TaskStatus taskStatus = READY;
        common::uint32_t priority;
        common::uint32_t runningTime;
        common::uint32_t pid = 0;
        common::uint32_t parentPid = 0;
        common::uint32_t waitPid = 0;
    public:
        Task(GlobalDescriptorTable *gdt, void entrypoint(), common::uint32_t pid, common::uint32_t parentPid, common::uint32_t priority);
        common::uint32_t getPid();
        ~Task();
        Task();
        void InitTask(GlobalDescriptorTable *gdt, void entrypoint(), common::uint32_t pid, common::uint32_t parentPid, common::uint32_t priority);
    };
    
    
    class TaskManager
    {
    private:
        Task tasks[256];
        int numTasks;
        int currentTask;
        GlobalDescriptorTable* gdt;
    public:
        TaskManager(GlobalDescriptorTable* gdt);
        ~TaskManager();
        bool AddTask(Task* task);
        CPUState* Schedule(CPUState* cpustate);
        CPUState* Schedule_Strategy_1_2(CPUState* cpustate);
        CPUState* Schedule_Strategy_3(CPUState* cpustate);
        CPUState* Schedule_Strategy_Dynamic(CPUState* cpustate);
        common::uint32_t getPid();
        bool exit();
        common::uint32_t fork(CPUState* cpu);
        common::uint32_t execve(void (*entrypoint)());
        bool AddForkedTask(Task* child, Task* parent);
        void CopyTask(Task *src, Task *dest);
        bool waitPid(CPUState* cpu);
        int getTaskIndex(common::uint32_t pid);
        void printTask(int taskIndex);
        void printCpuStates(int taskIndex);
    };
    
    
    
}


#endif