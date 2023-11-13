#include "VirtualAddressEntity.h"
#include <iostream>

VirtualAddressEntity::VirtualAddressEntity() {
    modified = false;
    read = false; 
    absent = true; 
    where_in_physical_mem = -1;
    // std::cout<<"vad olusturuldu"<<std::endl;
}

// Now the implementations of the new methods
// Now the implementation of setTimestamp
void VirtualAddressEntity::setTimestamp() {
    this->timestamp = std::chrono::system_clock::now();
}

std::chrono::system_clock::time_point VirtualAddressEntity::getTimestamp() const {
    return this->timestamp;
}

bool VirtualAddressEntity::isTimestampLessThan(const VirtualAddressEntity& other) const {
    return this->timestamp < other.timestamp;
}

// Implementation of delaySimulation
void VirtualAddressEntity::delaySimulation(int delay_time) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_time));
}


void VirtualAddressEntity::setModified(bool value) {
    modified = value;
}

bool VirtualAddressEntity::isModified() const {
    return modified;
}

void VirtualAddressEntity::setRead(bool value) {
    read = value;
}

bool VirtualAddressEntity::isRead() const {
    return read;
}

void VirtualAddressEntity::setAbsent(bool value) {
    absent = value;
}

bool VirtualAddressEntity::isAbsent() const {
    return absent;
}

void VirtualAddressEntity::setWhereInPhysicalMem(int value) {
    where_in_physical_mem = value;
}

int VirtualAddressEntity::getWhereInPhysicalMem() const {
    return where_in_physical_mem;
}

void VirtualAddressEntity::setPageNumber(int value) {
    page_numb = value;
}

int VirtualAddressEntity::getPageNumber() const {
    return page_numb;
}

void VirtualAddressEntity::whoAmI() {
    std::cout << "Virtual Address Entity Information:" << std::endl;
    std::cout << "Modified: " << (modified ? "Yes" : "No") << std::endl;
    std::cout << "Read: " << (read ? "Yes" : "No") << std::endl;
    std::cout << "Absent: " << (absent ? "Yes" : "No") << std::endl;
    std::cout << "Where in Physical Memory: " << where_in_physical_mem << std::endl;
    std::cout << "Page Number: " << page_numb << std::endl;
}

