
// #include <multitasking.h>
// //#include <print.h>


// using namespace myos;
// using namespace myos::common;


// Task::Task(GlobalDescriptorTable *gdt, void entrypoint())
// {
//     cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    
//     cpustate -> eax = 0;
//     cpustate -> ebx = 0;
//     cpustate -> ecx = 0;
//     cpustate -> edx = 0;

//     cpustate -> esi = 0;
//     cpustate -> edi = 0;
//     cpustate -> ebp = 0;
    
//     /*
//     cpustate -> gs = 0;
//     cpustate -> fs = 0;
//     cpustate -> es = 0;
//     cpustate -> ds = 0;
//     */
    
//     // cpustate -> error = 0;    
   
//     // cpustate -> esp = ;
//     cpustate -> eip = (uint32_t)entrypoint;
//     cpustate -> cs = gdt->CodeSegmentSelector();
//     // cpustate -> ss = ;
//     cpustate -> eflags = 0x202;
    
// }

// Task::Task(Task* oldOne){

//     cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));

//     cpustate -> eax = oldOne->cpustate->eax;
//     cpustate -> ebx = oldOne->cpustate->ebx;
//     cpustate -> ecx = oldOne->cpustate->ecx;
//     cpustate -> edx = oldOne->cpustate->edx;

//     cpustate -> esi = oldOne->cpustate->esi;
//     cpustate -> edi = oldOne->cpustate->edi;
//     cpustate -> ebp = oldOne->cpustate->ebp;


//     cpustate -> eip = (uint32_t)oldOne->cpustate->eip;

//     cpustate -> cs = oldOne->cpustate->cs;

//     cpustate -> eflags = 0x202;

//     // for (uint32_t i = 0; i < 4096; i++){
//     //     stack[i] = oldOne->stack[i];
//     // }

// }

// Task::Task(){

// }

// Task::~Task()
// {
// }

        
// TaskManager::TaskManager()
// {
//     numTasks = 0;
//     currentTask = -1;
// }

// TaskManager::~TaskManager()
// {
// }


// // void TaskManager::exit(int id)
// // {
// //     tasks[id]->status = -1;

// // }

// // void TaskManager::exit()
// // {
// //     tasks[currentTask]->status = -1;

// // }

// int TaskManager::fork(GlobalDescriptorTable *gdt)
// {


//     Task* curr_task = tasks[currentTask];

//     // if(curr_task->is_child==true)
//     //     return -1;

//     Task* newOne = new Task(curr_task);

//     // newOne->is_child = true;
//     // // newOne->status=-1;

//     newOne-> cpustate -> cs = gdt->CodeSegmentSelector();


    
//     if( !AddTask(newOne))
//         return -1;
    
//     curr_task->id = numTasks-1;
//     return curr_task->id;
// }


// bool TaskManager::AddTask(Task* task)
// {
//     if(numTasks >= 256)
//         return false;
//     tasks[numTasks++] = task;
//     task->id = numTasks-1;
//     return true;
// }

// CPUState* TaskManager::Schedule(CPUState* cpustate)
// {


//     // for (int i = 0; i < 70; i++){
//     //     printf("current task: ");
//     //     printfHex(currentTask);
//     //     printf("\nNumTask: ");
//     //     printfHex(numTasks);
//     //     printf("------------------------------------------------------------\n");
//     // }

//     if(numTasks <= 0)
//         return cpustate;


//     // printfHex(currentTask);
//     // for (int i = 0; i < 110; i++)
//     // {
//     //     printfHex(currentTask);
//     //     printf("current task\n");
//     // }
    
    


//     // if(currentTask >= 0){
//     //     if (tasks[currentTask]->status!=-1){
//     //         tasks[currentTask]->cpustate = cpustate;
//     //         tasks[currentTask]->status = 1;            
//     //     }
//     //     else
//     //     {
//     //         currentTask=2;
//     //     }
        

//     //     }

//     // else{
//     //     currentTask=1;
//     // }
    

//     //     for (int i = 0; i < 10; i++){
//     //     printf("current task: ");
//     //     printfHex(currentTask);
//     //     printf("\nNumTask: ");
//     //     printfHex(numTasks);
//     //     printf("------------------------------------------------------------\n");
//     // }



//     if (currentTask>=0)
//         tasks[currentTask]->cpustate = cpustate;


//     // if (currentTask==0)
//     // {
//     //     currentTask =1;
//     // }
    

//     if(++currentTask >= numTasks)
//         currentTask %= numTasks;
    
//     while (tasks[currentTask]->status==-1)
//     {
//         currentTask++;
//         if (currentTask >= numTasks)
//             currentTask %= numTasks;
        
//     }
    
    
//     return tasks[currentTask]->cpustate;
// }

    