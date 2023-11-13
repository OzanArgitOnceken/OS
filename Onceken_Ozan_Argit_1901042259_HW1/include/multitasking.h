 
#ifndef __MYOS__MULTITASKING_H
#define __MYOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

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
        bool isWaitingForExecve=false;
        bool isExeced=false;
        bool isRunnedAfterWait;
        bool isWaiting=false;
        //bool amIFinished=false;
        int taskState;
        int id;
        int parentId=-1;
        int childId=-1;
        bool isForked = false;
        common::uint8_t stack[4096]; // 4 KiB
        CPUState* cpustate;
    public:
        int getStateOfTask();
        Task(Task* c);
        Task(GlobalDescriptorTable *gdt, void entrypoint());  
        // Task(GlobalDescriptorTable *gdt, bool entrypoint());
        Task(GlobalDescriptorTable *gdt, Task*); 
        Task();
        ~Task();
    };
    
    
    class TaskManager
    {
    private:
        Task* tasks[256];
        int numTasks;
        int currentTask; 
    public:
        void waitpid(int);
        void killId(int);//this needed because waitpid makes parent process wait if there are no parent this method will be use
        int fork(GlobalDescriptorTable*);
        TaskManager();
        ~TaskManager();
        int AddTask(Task* task);
        CPUState* Schedule(CPUState* cpustate);
        void execve(int);
        void kill();
    };
}
#endif