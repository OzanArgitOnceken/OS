
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
// #define GRAPHICSMODE
using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;
TaskManager taskManager;
bool linearSearchFinished=false;
bool binarySearchFinished=false;
bool collatzFinished=false;
int linearId;
int binaryId;
int collatzId;
int finishedTasks[10];
int globalForkHolder=0;
GlobalDescriptorTable gdt;
// Task*linearTask=new Task(&gdt,linearSearch);
// Task*binaryTask=new Task(&gdt,binarySearch);
// Task*collatzTask=new Task(&gdt,taskCollatz);
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
void my_sleep(){
    for (int i = 0; i < 99999999; i++){}
}
void sysprintf(char* str)
{
    asm("int $0x80" : : "a" (4), "b" (str));
}
 
void sysexit()
{
    asm("int $0x80" : : "a" (1));
}
void sysprintfIntDecimal(int num)
{
    char buffer[32]; // buffer to hold the string representation of the integer
    int i = 0;
    int is_negative = 0;

    // handle negative numbers
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // convert the integer to a string
    do {
        buffer[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    // add the negative sign if necessary
    if (is_negative) {
        buffer[i++] = '-';
    }

    // reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
    }

    // add a null terminator to the end of the string
    buffer[i] = '\0';

    // call sysprintf to print the string
    sysprintf(buffer);
}

void taskK()
{ 
    sysprintf("K");  
    for (;;){}
}
void taskHelloWorld()
{
    while (true)
        sysprintf("Hello world\n");
}



void execveHelper(){
    for (int i = 0; i < 3; i++){}
        my_sleep();
    
    sysprintf("This is Execve task\n");

    for ( int i = 0; i < 3; i++){}
        my_sleep();
    sysprintf("Execve is finished\n");
    
    while (true){}
}



void taskB()
{
    int x=0;
    x=taskManager.fork(&gdt);
    sysprintfIntDecimal(x);
    sysprintf(" ");
    sysexit();
    while (true){} 
}
void taskC()
{
    sysprintf("C\n");
    for (;;){}
    
}
void taskD()
{
    sysprintf("D");
    // for (;;){}
    
}
void taskF()
{
    sysprintf("F");
    for (;;){}
}
int binarySearchReal(int arr[], int size, int target) {
    int left = 0;
    int right = size - 1; 
    while (left <= right) {
        int mid = (left+right) / 2;
        if(arr[mid]==target)
            return mid;
        else if(arr[mid]<target)
            left = mid+1;
        else
            right=mid-1;
    }
    return -1;
}
void binarySearch(){
    int arr[100];
    for (int i = 0; i < 100; i++)
        arr[i]=i;
    int place=binarySearchReal(arr,100,99);
    sysprintf("Number's place in binary search is :");
    sysprintfIntDecimal(place);
    sysprintf("\n");
    binarySearchFinished=true; 
    for(;;){}
}
void taskCollatz(){
    int arr[100];
    for (int i = 0; i < 100; i++)
        arr[i]=i; 
    for (int i = 0; i < 100; i++) {
        int n = arr[i]; 
        while (n != 1) {
            if (n % 2 == 0) {
                n /= 2;
            } else {
                n = n * 3 + 1;
            }
        }
    }
    collatzFinished=true;
    sysprintf("\nCollatz done\n"); 
    while (true){}
    
}
void linearSearch() {

    int arr[100];
    for (int i = 0; i < 100; i++)
        arr[i]=i;
    int target=99;
    for (int i = 0; i < 100; i++) {
        if (arr[i] == target){
                sysprintf("\nThe value find at:");
                sysprintfIntDecimal(arr[i]);
                sysprintf("th index(linear search)\n");
                break;
            }
    }
    linearSearchFinished=true;
    for(;;){}
}

 


typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
} 

//test for fork wait and execve
void ForkWait(){
    int x=taskManager.fork(&gdt); 
    
        if(x!=0){
            sysprintf("This is parent process\n");
            for (int i = 0; i < 10; i++)
                my_sleep(); 
            sysprintf("Child process is finished \n");
            }
        else{
            sysprintf("This is a child process\n");
            taskManager.waitpid(x);
            }
    //execveHelper();
    while (true){}
}
void firstStrategy(){

    int x=taskManager.fork(&gdt);  
    int y=taskManager.fork(&gdt);  
    if(x==0)
        linearSearch();
    if(x!=0)
        binarySearch();
    while(true){
        taskManager.kill();
    }
}

void secondStrategy(){
    for (int i = 0; i < 10; i++){
        taskManager.fork(&gdt);
        }
    linearSearch();
    while(true){
        taskManager.kill();
    }
}
void binaryWithFork(){
    Task* tryBinary=new Task(&gdt,binarySearch);
    taskManager.fork(&gdt);
    taskManager.AddTask(tryBinary);
}
void linearWithFork(){
    Task* tryLinear=new Task(&gdt,linearSearch);
    taskManager.fork(&gdt);
    taskManager.AddTask(tryLinear);
}
void finalStrategy(){
    Task* binary1=new Task(&gdt,binarySearch);
    Task* binary2=new Task(&gdt,binarySearch);
    Task* binary3=new Task(&gdt,binarySearch);
    Task* linear1=new Task(&gdt,linearSearch);
    Task* linear2=new Task(&gdt,linearSearch);
    Task* linear3=new Task(&gdt,linearSearch);
    taskManager.AddTask(linear1);
    taskManager.AddTask(linear2);
    taskManager.AddTask(linear3);
    taskManager.AddTask(binary1);
    taskManager.AddTask(binary2);
    taskManager.AddTask(binary3);
    while (true)
    {
        taskManager.kill();
    }
    
}







extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("Hello to Ozan Onceken's OS!\n");

    
    
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);
    
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8 ) & 0xFF);
    printfHex((heap      ) & 0xFF);
    
    void* allocated = memoryManager.malloc(1024);
 
    
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xFF);
    printfHex(((size_t)allocated >> 16) & 0xFF);
    printfHex(((size_t)allocated >> 8 ) & 0xFF);
    printfHex(((size_t)allocated      ) & 0xFF);
    printf("\n");
    // MAIN

    Task*m=new Task(&gdt,taskC);
    taskManager.AddTask(m);


    Task *execed=new Task(&gdt,execveHelper);
    int execedId=taskManager.AddTask(execed);
    taskManager.execve(execedId);


    Task* tryForForkWait =new Task(&gdt,ForkWait );
    taskManager.AddTask(tryForForkWait );

    Task *tryFirstStrategy=new Task(&gdt,firstStrategy);
    taskManager.AddTask(tryFirstStrategy);

    Task *second=new Task(&gdt,secondStrategy);
    taskManager.AddTask(second);
    Task* tryFinalStrategy=new Task(&gdt,finalStrategy);
    taskManager.AddTask(tryFinalStrategy);

    //END OF MAIN
    InterruptManager interrupts(0x20, &gdt, &taskManager);
    SyscallHandler syscalls(&interrupts, 0x80);
    
    printf("Initializing Hardware, Stage 1\n");
    
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
        
    printf("Initializing Hardware, Stage 2\n");
        drvManager.ActivateAll();
        
    printf("Initializing Hardware, Stage 3\n");

    #ifdef GRAPHICSMODE
        vga.SetMode(320,200,8);
        Window win1(&desktop, 10,10,20,20, 0xA8,0x00,0x00);
        desktop.AddChild(&win1);
        Window win2(&desktop, 40,15,30,30, 0x00,0xA8,0x00);
        desktop.AddChild(&win2);
    #endif
    amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);
    eth0->Send((uint8_t*)"Hello Network", 13);
        

    interrupts.Activate();
    //interrupts.Deactivate();


    while(1)
    {
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }
}