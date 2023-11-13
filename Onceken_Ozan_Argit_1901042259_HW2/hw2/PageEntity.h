#ifndef PAGEENTITY_H
#define PAGEENTITY_H

#include <iostream>
#include <random>

const int PAGE_SIZE = 4;


class PageEntity {
private:
    int page_numb;
    int content[PAGE_SIZE];
    int generateRandomNumber(int numb);

public:
    PageEntity();
    void printContent();
    int& operator[](int index);
    void initRandom();
    void whoAmI();
    void setPageNumb(int pageNumb);
    int getPageNumb()const{return page_numb;}
    PageEntity& operator=(const PageEntity& other);
    void setMem(int index, int value);
};

#endif
