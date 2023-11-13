
#include <multitasking.h>

using namespace myos;
using namespace myos::common;




Task::Task(GlobalDescriptorTable *gdt, void entrypoint())
{
    this->childId=-1;
    this->parentId=-1;
    this->taskState=1;
    //this->isRunnedAfterWait=true;
    this->isWaiting=false;


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

// Task::Task(GlobalDescriptorTable *gdt, bool entrypoint())
// {
//     this->childId=-1;
//     this->parentId=-1;
//     this->taskState=1;
//     //this->isRunnedAfterWait=true;
//     this->isWaiting=false;


//     cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    
//     cpustate -> eax = 0;
//     cpustate -> ebx = 0;
//     cpustate -> ecx = 0;
//     cpustate -> edx = 0; 
//     cpustate -> esi = 0;
//     cpustate -> edi = 0;
//     cpustate -> ebp = 0;
//     cpustate -> eip = (uint32_t)entrypoint;
//     cpustate -> cs = gdt->CodeSegmentSelector();  
//     cpustate -> eflags = 0x202;
// } 

Task::Task(GlobalDescriptorTable *gdt,Task *c)
{

    this->parentId=c->id;
    this->taskState=1;
    this->isWaiting=false;

    this->cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    this->cpustate -> eax = c->cpustate->eax;
    this->cpustate -> ebx = c->cpustate->ebx;
    this->cpustate -> ecx = c->cpustate->ecx;
    this->cpustate -> edx = c->cpustate->edx;
    this->cpustate -> esi = c->cpustate->esi;
    this->cpustate -> edi = c->cpustate->edi;
    this->cpustate -> ebp = c->cpustate->ebp;
    this->cpustate -> eip = (uint32_t)c->cpustate->eip;
    this->cpustate -> cs = c->cpustate->cs;
    this->cpustate -> eflags = 0x202;
    // this-> cpustate -> cs = gdt->CodeSegmentSelector();


} 
Task::Task(Task* c){
    this->isWaiting=false;
    this->parentId=c->id;
    this->taskState=1;
    this->cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    this->cpustate -> eax = c->cpustate->eax;
    this->cpustate -> ebx = c->cpustate->ebx;
    this->cpustate -> ecx = c->cpustate->ecx;
    this->cpustate -> edx = c->cpustate->edx;
    this->cpustate -> esi = c->cpustate->esi;
    this->cpustate -> edi = c->cpustate->edi;
    this->cpustate -> ebp = c->cpustate->ebp;
    this->cpustate -> eip = (uint32_t)c->cpustate->eip;
    this->cpustate -> cs = c->cpustate->cs;
    this->cpustate -> eflags = 0x202;
    // this-> cpustate -> cs = gdt->CodeSegmentSelector();
}
Task::~Task(){}
Task::Task(){
    this->childId=-1; 
    this->isWaiting=false;
}
int TaskManager::AddTask(Task* task)
{
    if(numTasks >= 256)
        return false;
    // task->id=totalTaskCount++;//task id is totalTaskCount for special id
    tasks[numTasks++] = task;
    task->id = numTasks-1;
    return task->id;
}
/*
    holds the current task, copies it and creates a new task
*/
int TaskManager::fork(GlobalDescriptorTable *gdt)
{
    Task* current_Task=tasks[currentTask];
    if(current_Task->isForked==true)//if the process is child do not fork it
       return 0;
    Task *newTask=new Task(current_Task);
    newTask->parentId = 0;
    newTask->cpustate -> cs = gdt->CodeSegmentSelector();
    newTask->id=numTasks-1;
    newTask->parentId = currentTask;
    tasks[currentTask]->childId=newTask->id;
    newTask->isForked = true;
    int is_add_task = AddTask(newTask);
    if(is_add_task<=0){
        return 0;
    }
    return (newTask->id); 
}

TaskManager::TaskManager()
{ 
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager()
{
}
CPUState* TaskManager::Schedule(CPUState* cpustate)
{
    if(numTasks <= 0)
        return cpustate;
    if(currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;
    if(++currentTask >= numTasks)
        currentTask %= numTasks;
    //if the process is running and not waiting for any other process return that process
    if(tasks[currentTask]->getStateOfTask()==1&&tasks[currentTask]->isWaiting==false
    &&tasks[currentTask]->isWaitingForExecve==false){
        //if it is a child process that any parent waits it set it is runned and set 
        //its parent to free
        if(tasks[currentTask]->isRunnedAfterWait==false){
            tasks[currentTask]->isRunnedAfterWait=true;
            int parentId=tasks[currentTask]->parentId;
            for (int i = 0; i < numTasks; i++)
            {
                if(tasks[i]->id==parentId)
                    tasks[i]->isWaiting=false;
                    break;
            } 
        }
        /*
            if program finds the execed process so it will runs it and after 
            that it will makes the other processes available
            so that the program makes sure every program waits the execed one 
        */
        if(tasks[currentTask]->getStateOfTask()==1 &&tasks[currentTask]->isExeced==true
            &&tasks[currentTask]->isWaiting==false){
                tasks[currentTask]->isExeced=false;
                for (int i = 0; i < numTasks; i++)
                {
                    tasks[i]->isWaitingForExecve=false;
                }
                return tasks[currentTask]->cpustate;
            }

        return tasks[currentTask]->cpustate;
    }
    //if the current process is not free or waiting any other process find another one that
    //supplies the conditions
    else{
        while (true)
        {
            if(++currentTask >= numTasks)
                currentTask %= numTasks;
            
            if(tasks[currentTask]->getStateOfTask()==1&&tasks[currentTask]->isWaiting==false
            &&tasks[currentTask]->isWaitingForExecve==false)
                return tasks[currentTask]->cpustate;
            
        }
    }

}
int Task::getStateOfTask(){
    return this->taskState;
}
void TaskManager::kill(){
    tasks[currentTask]->taskState=0;
    // return this->taskState;
}
/*
    At first it finds the child and its parent.
    sets the childs isRunnedAfterWait variable to false
    it causes the scheduler knows it is a child process and does not runned after
    the waitpid

    after that method finds the parent of it and sets it isWaiting variable to true
    then the scheduler knows that this process is waiting for run its child.
*/
void TaskManager::waitpid(int idChild){ 

    int parentId=-1;
    for (int i = 0; i < numTasks; i++)
    {
        if(tasks[i]->id==idChild){
            parentId=tasks[i]->parentId;
            tasks[i]->isRunnedAfterWait=false;
        }
    }
    

    for (int i = 0; i < numTasks; i++){
        if(tasks[i]->id==parentId){
            tasks[i]->isWaiting=true;
            break;
        }
    }
}
/*
    Sets all processes waiting for execed process
    if the execed proceess does not runned all others will wait for it
*/
void TaskManager::execve(int id){
    for (int i = 0; i < numTasks; i++)
    {
        if(tasks[i]->id!=id)
            tasks[i]->isWaitingForExecve=true;
        else
            tasks[i]->isExeced=true;
    }
}

/*
    This method kills the process (makes its taskstate -1 and process does not works)
*/
void TaskManager::killId(int id){
    for (int i = 0; i < numTasks; i++)
    {
        if(tasks[i]->id==id){
            tasks[i]->taskState=-1;
            break;
            }
    }
    
}