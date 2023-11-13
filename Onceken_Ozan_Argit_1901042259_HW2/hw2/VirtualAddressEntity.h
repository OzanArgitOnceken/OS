#ifndef VIRTUALADDRESSENTITY_H
#define VIRTUALADDRESSENTITY_H
#include <chrono>
#include <thread>
class VirtualAddressEntity {
private:
    bool modified;
    bool read;
    bool absent = true;
    int where_in_physical_mem =-1;
    int page_numb;
    std::chrono::system_clock::time_point timestamp;

public:
    VirtualAddressEntity();
    void setModified(bool value);
    bool isModified() const;

    void setRead(bool value);
    bool isRead() const;

    void setAbsent(bool value);
    bool isAbsent() const;

    void setWhereInPhysicalMem(int value);
    int getWhereInPhysicalMem() const;

    void setPageNumber(int value);
    int getPageNumber() const;

    void setTimestamp();
    std::chrono::system_clock::time_point getTimestamp() const;
    bool isTimestampLessThan(const VirtualAddressEntity& other) const;
    
    static void delaySimulation(int delay_time = 1);


    void whoAmI();

};

#endif
