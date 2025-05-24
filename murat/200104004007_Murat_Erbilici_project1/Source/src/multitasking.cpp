
#include <multitasking.h>

using namespace myos;
using namespace myos::common;

common::uint32_t Task::nextPID = 0;
void printf(char* str);
void printInt(int num);
Task::Task()
{
    InitTask(0, 0, 0, 0, 0);
}
Task::Task(GlobalDescriptorTable *gdt, void entrypoint(), common::uint32_t pid, common::uint32_t parentPid, common::uint32_t priority)
{
    InitTask(gdt, entrypoint, pid, parentPid, priority);
}
void Task::InitTask(GlobalDescriptorTable *gdt, void entrypoint(), common::uint32_t pid, common::uint32_t parentPid, common::uint32_t priority) {
    this->parentPid = parentPid;
    this->pid = pid;
    taskStatus = READY;
    waitPid = 0;
    this->priority=priority;
    if(this->pid == 1) {
        this->priority = 39; // Let the collatz program to be lowest priority.
    }
    runningTime=0;

    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));

    cpustate -> eax = 0;
    cpustate -> ebx = 0;
    cpustate -> ecx = 0;
    cpustate -> edx = 0;

    cpustate -> esi = 0;
    cpustate -> edi = 0;
    cpustate -> ebp = 0;
    
    cpustate -> eip = (uint32_t)entrypoint;
    cpustate -> cs = gdt->CodeSegmentSelector();
    cpustate -> eflags = 0x202;
}


Task::~Task()
{
}

        
TaskManager::TaskManager(GlobalDescriptorTable* gdt)
{
    this->gdt = gdt;
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager()
{

}
void TaskManager::CopyTask(Task *src, Task *dest) {
    dest->taskStatus = src->taskStatus;
    dest->pid = src->pid;
    dest->parentPid = src->parentPid;
    dest->waitPid = src->waitPid;
    dest->priority = src->priority;
    dest->runningTime = src->runningTime;

    for (int i = 0; i < sizeof(src->stack); ++i)
        dest->stack[i] = src->stack[i];

    *(dest->cpustate) = *(src->cpustate);
}
bool TaskManager::AddTask(Task* task)
{
    if(numTasks >= 256)
        return false;
    task->pid = Task::nextPID;
    CopyTask(task, &tasks[numTasks]);
    ++Task::nextPID;
    ++numTasks;
    
    return true;
}

void sleep(int seconds);
int time =0;
CPUState* TaskManager::Schedule_Strategy_1_2(CPUState* cpustate)
{
    for(int i=0;i<numTasks;++i){
        //printTask(i);
        //printCpuStates(i);
    }
    if(numTasks <= 0) {
        return cpustate;
    }
    if(currentTask >= 0)
        tasks[currentTask].cpustate = cpustate;
    
    int nextTaskIndex = -1;
    if(tasks[currentTask].taskStatus == RUNNING) {
        tasks[currentTask].taskStatus = READY;
    }
    for (int i = 0; i < numTasks; ++i)
    {
        currentTask = (currentTask + 1) % numTasks;
        if(tasks[currentTask].taskStatus == BLOCKED) {
            if(tasks[currentTask].waitPid == 0) {
                tasks[currentTask].taskStatus = READY;
                nextTaskIndex = currentTask;
                break;
            }
        }
        if (tasks[currentTask].taskStatus == READY && currentTask != nextTaskIndex)
        {
            nextTaskIndex = currentTask;
            break;
        }
    }
    
    if (nextTaskIndex == -1) {
        tasks[currentTask].taskStatus = READY;
        return cpustate;
    }
    tasks[nextTaskIndex].taskStatus = RUNNING;
    return tasks[nextTaskIndex].cpustate;
}

CPUState* TaskManager::Schedule_Strategy_3(CPUState* cpustate)
{
    int isCollatz = 0;  
    for(int i=0;i<numTasks;++i){
        // printTask(i);0
        //printCpuStates(i);
    }
    if(numTasks <= 0) {
        return cpustate;
    }
    if(currentTask >= 0)
        tasks[currentTask].cpustate = cpustate;
    
    int highestPriority = 40;
    int nextTaskIndex = -1;
    if(tasks[currentTask].taskStatus == RUNNING) {
        tasks[currentTask].taskStatus = READY;
    }
    for (int i = 0; i < numTasks; ++i)
    {
        currentTask = (currentTask + 1) % numTasks;
        if(tasks[currentTask].taskStatus == BLOCKED) {
            if(tasks[currentTask].waitPid == 0) {
                tasks[currentTask].taskStatus = READY;
            }
        }
        if(tasks[currentTask].pid == 1) {
            isCollatz = 1;
        } 
        else {
            isCollatz = 0;
        }
        if (isCollatz == 1 && time <= 50)
        {
            if(tasks[currentTask].taskStatus == READY && currentTask != nextTaskIndex)
            {
                nextTaskIndex = currentTask;
                break;
            }
        }
        if(isCollatz == 0 && time <= 50 && tasks[currentTask].pid != 0)
            continue;                
        if (tasks[currentTask].taskStatus == READY && tasks[currentTask].priority < highestPriority)
        {   
            highestPriority = tasks[currentTask].priority;
            nextTaskIndex = currentTask;
        }
    } 
    if(time < 500)
        time++;
    currentTask = nextTaskIndex;
    if (nextTaskIndex == -1) {
        tasks[currentTask].taskStatus = READY;
        return cpustate;
    }
    tasks[nextTaskIndex].taskStatus = RUNNING;
    return tasks[nextTaskIndex].cpustate;
}
CPUState* TaskManager::Schedule_Strategy_Dynamic(CPUState* cpustate)
{
    int isCollatz = 0;  
    for(int i=0;i<numTasks;++i){
        // printTask(i);0
        //printCpuStates(i);
    }
    if(numTasks <= 0) {
        return cpustate;
    }
    if(currentTask >= 0)
        tasks[currentTask].cpustate = cpustate;
    
    int highestPriority = 40;
    int nextTaskIndex = -1;
    if(tasks[currentTask].taskStatus == RUNNING) {
        tasks[currentTask].taskStatus = READY;
    }
    for (int i = 0; i < numTasks; ++i)
    { 
        currentTask = (currentTask + 1) % numTasks;
        if(tasks[currentTask].taskStatus == BLOCKED) {
            if(tasks[currentTask].waitPid == 0) {
                tasks[currentTask].taskStatus = READY;
            }
        }
        if(tasks[currentTask].pid == 1) {
            isCollatz = 1;
        } 
        else {
            isCollatz = 0;
        }
        if (isCollatz == 1 && time <= 50)
        {
            if(tasks[currentTask].taskStatus == READY && currentTask != nextTaskIndex)
            {
                nextTaskIndex = currentTask;
                break;
            }
        }
        if(isCollatz == 1 && time % 20 == 0)
            if(tasks[currentTask].priority >= 1)
                tasks[currentTask].priority-=12;      
        if (tasks[currentTask].taskStatus == READY && tasks[currentTask].priority < highestPriority)
        {   
            highestPriority = tasks[currentTask].priority;
            nextTaskIndex = currentTask;
        }
    } 
    if(time < 1000000)
        time++;
    else {
        time = 100;
    }
    currentTask = nextTaskIndex;
    if (nextTaskIndex == -1) {
        tasks[currentTask].taskStatus = READY;
        return cpustate;
    }
    tasks[nextTaskIndex].taskStatus = RUNNING;
    return tasks[nextTaskIndex].cpustate;
}

CPUState* TaskManager::Schedule(CPUState* cpustate) {
    return Schedule_Strategy_1_2(cpustate);
    //return Schedule_Strategy_3(cpustate);
    //return Schedule_Strategy_Dynamic(cpustate);
}


common::uint32_t TaskManager::getPid() {
    return tasks[currentTask].getPid();
}
bool TaskManager::exit() {
    if(tasks[currentTask].parentPid != 0) {
        int index = getTaskIndex(tasks[currentTask].parentPid);
        if(index != -1 && tasks[index].waitPid == tasks[currentTask].pid) {
            tasks[index].waitPid = 0;
        }
    }
    tasks[currentTask].taskStatus = FINISHED;
    return true;
}
common::uint32_t TaskManager::fork(CPUState* cpu) {
    printf(" *FORK* ");

    if (numTasks >= 256)
        return -1;

    Task *parent = &tasks[currentTask];
    Task *child = &tasks[numTasks];
    child->InitTask(gdt, (void(*)()) parent->cpustate->eip, Task::nextPID, parent->pid, parent->priority);
    *(child->cpustate) = *cpu; 

    tasks[numTasks].taskStatus=READY;
    tasks[numTasks].parentPid=tasks[currentTask].pid;
    tasks[numTasks].pid=Task::nextPID;

    for (int i = 0; i < sizeof(tasks[currentTask].stack); i++)
    {
        tasks[numTasks].stack[i]=tasks[currentTask].stack[i];
    }
    //Stackten yer alında cpustate'in konumu değişiyor bu nedenle şuanki taskın offsetini hesaplayıp yeni oluşan process'in cpu statenin konumunu ona göre düzenliyorum. Bu işlemi yapmazsam process düzgün şekilde devam etmiyor.
    common::uint32_t currentTaskOffset=(((common::uint32_t)cpu - (common::uint32_t) tasks[currentTask].stack));
    tasks[numTasks].cpustate=(CPUState*)(((common::uint32_t) tasks[numTasks].stack) + currentTaskOffset);
 
    //Burada ECX' yeni taskın process id'sini atıyorum. Syscall'a return edebilmek için.
    tasks[numTasks].cpustate->ecx = 0;
    ++Task::nextPID;
    ++numTasks;
    return tasks[numTasks-1].pid;
}

common::uint32_t TaskManager::execve(void (*entrypoint)()) {
    printf(" *EXECVE* ");
    Task *curr = &tasks[currentTask]; 
    curr->InitTask(gdt, entrypoint, curr->pid, curr->parentPid, curr->priority);
    return (common::uint32_t) curr->cpustate;
}
common::uint32_t Task:: getPid() {
    return pid;
}
bool TaskManager::waitPid(CPUState* cpu) {
    printf(" *WAIT* ");
    common::uint32_t pid = cpu->ebx;
    if(pid != tasks[currentTask].pid){
        int index = getTaskIndex(pid);
        if(index != -1 && tasks[index].taskStatus != FINISHED) {
            tasks[currentTask].waitPid = pid;
            tasks[currentTask].taskStatus = BLOCKED;
            return true;
        }
    }
    return false;
}

int TaskManager::getTaskIndex(common::uint32_t pid) {
    for(int i=0;i<numTasks;++i) {
        if(tasks[i].pid == pid)
            return i;
    }
    return -1;
}

void TaskManager::printTask(int taskIndex) 
{
    Task *task = &tasks[taskIndex];
    printf("\nCurrent process pid: ");
    printInt(task->pid);
    printf("\n"); 
    printf("Current parent pid: ");
    printInt(task->parentPid);
    printf("\n");
    printf("Current Priority Num: ");
    printInt(task->priority);
    printf("\n");
    printf("Process State: ");
    switch (task->taskStatus) {
        case READY:         printf("Ready\n"); break;
        case RUNNING:       printf("Running\n"); break;
        case BLOCKED:       printf("Blocked\n"); break;
        case FINISHED:    printf("Finished\n"); break;
    }
    printf("\n");
}

void TaskManager::printCpuStates(int taskIndex) {
    CPUState* cpustate = tasks[taskIndex].cpustate;
    printf("Process id: ");
    printInt(tasks[taskIndex].pid);
    printf(", neax: ");
    printInt((uint32_t) cpustate->eax);
    printf(", ebx: ");
    printInt((uint32_t) cpustate->ebx);
    printf(", ecx: ");
    printInt((uint32_t) cpustate->ecx);
    printf(", edx: ");
    printInt((uint32_t) cpustate->edx);
    printf(", esi: ");
    printInt((uint32_t) cpustate->esi);
    printf(", edi: ");
    printInt((uint32_t) cpustate->edi);
    printf(", ebp: ");
    printInt((uint32_t) cpustate->ebp);
    printf(", error: ");
    printInt((uint32_t) cpustate->error);
    printf(", eip: ");
    printInt((uint32_t) cpustate->eip);
    printf(", cs: ");
    printInt((uint32_t) cpustate->cs);
    printf(", eflags: ");
    printInt((uint32_t) cpustate->eflags);
    printf(", esp: ");
    printInt((uint32_t) cpustate->esp);
    printf(", ss: ");
    printInt((uint32_t) cpustate->ss);
    printf("\n");
}



    