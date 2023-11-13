#include <iostream>
#include "PageEntity.h"
#include "VirtualAddressEntity.h"
#include "VirtualMemoryManagement.h"

PageEntity currently_page; 
VirtualMemoryManagement virtual_memory;
int getPage(int page_numb);
int main() { 
  virtual_memory.virtualMain();
  int pg_number;
  for (;;){
    std::cout<<"Enter wanted page:\n"<<std::endl;
    std::cin>>pg_number;
    getPage(pg_number);
    virtual_memory.showSimulatedAdresses(); 
  }
  return 0;
}
int getPage(int page_numb){
  currently_page = virtual_memory.getPage2(page_numb);
  return 0;
}
