#ifndef VIRTUALMEMORYMANAGEMENT_H
#define VIRTUALMEMORYMANAGEMENT_H

#include "PageEntity.h"
#include "VirtualAddressEntity.h"
#include <iostream>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <random>
#include <chrono>
#include <vector>
// const int PAGE_SIZE  = 4;       //Her bir pagein array sayisi 

const int DISK_MEMORY = 32;
const int PHYSICAL_MEMORY = 4;
const int VIRTUAL_ADDRESS = 16;
const int DISK_CAPACITY = DISK_MEMORY - PHYSICAL_MEMORY; //Bu kapasite toplam, top seviyeyi kullanir, disk te backup islemleri icin bir fiziksel memorylik yer ayirilir


class VirtualMemoryManagement {
private:
    PageEntity disk[DISK_MEMORY];
    PageEntity physicalMemory[PHYSICAL_MEMORY];
    VirtualAddressEntity virtualAddress[VIRTUAL_ADDRESS];
    std::unordered_map<int, int> pageTable; //page number girilir output virtual address olur
                                            //virtual addresste de o pagein ram de nerde tutuldugu vs. bilgileri saklidir
    
    std::unordered_map<int, int> virtual_frame_table;//virtual addresslerin hangi frame denk gelidigini gosterir, frame veririz virtual adress verir
    
    std::queue<int> physical_mem_queue;
    
    void connectVirtToPhysicalMem(int virt_addr, int physical_addr);
    void initializeDisk();
    void initializePhysicalMemory();
    void setupVirtualAdress();
    void setPageTableEntry(int page_number, int virtual_address);
    int getPageTableEntry(int page_number);
    int getPageFrameToVirtualAddress(int page_number);
    bool isPhysicalMemoryFull();
    int find_empty_physical_mem();  //fiziksel mem.deki siradaki bos bir index bulur
    int getPageFromDiskToPhyMem(int disk_index, int ram_index);
    void updateOldVirtualAddress(int old_page_numb);
    void updateNewVirtualAddress(int old_page_numb, int physical_address);

    void updateVirtualAddress(int disk_index, int ram_index, int virtual_address);
    void updateInitVirtualAddress(int disk_index, int ram_index, int virtual_address);
    void fifoWay(int disk_index);
    void secondChanceAlgorithm(int page_number);
    void lruAlgorithm(int page_number);
    void WSClockAlgorithm(int page_number);
    void fifoWay2(int new_page_num);
    void physicalMemtoDisk(int mem_index, int disk_index);
    void fillRandomVirtualAddress();
    PageEntity pageFault2(int page_number);

    

public:
    VirtualMemoryManagement();

    void virtualMain();
    //setter getter
    void pageInfo(int pageNumber);
    static int page_replacement_algorithm;
    PageEntity getPage2(int normal_address);
    void setPage(int normal_address, int value);
    void showSimulatedAdresses(int restriction = VIRTUAL_ADDRESS);
    //set up

    void initializeVirtualMemory(); //burda diski, fiziksel memoryi ve virtual adressi initialize edicez baslamadan once
 //virtual adress ile fiziksel memoryi baglar

};

#endif
