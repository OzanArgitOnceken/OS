#include "PageEntity.h"

// const int pagesize = 4;


PageEntity::PageEntity(){
}

void PageEntity::printContent(){
    for (int i = 0; i < PAGE_SIZE; i++) {
        std::cout << "content[" << i << "]: " << (*this)[i] << std::endl;
    }
}

int& PageEntity::operator[](int index) {
    if (index < 0 || index >= PAGE_SIZE) {
        throw std::out_of_range("Invalid index!");
    }

    return content[index];
}

void PageEntity::initRandom(){
    for (int i = 0; i < PAGE_SIZE; i++)
        (*this)[i] = generateRandomNumber(1000);
}

void PageEntity::whoAmI()
{
    std::cout<<"Page Entity"<<std::endl;
    std::cout<<"Page Number: "<<page_numb<<std::endl;
    printContent();
}

void PageEntity::setPageNumb(int pageNumb){
    this->page_numb = pageNumb;
}

int PageEntity::generateRandomNumber(int numb){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, numb);
    return dis(gen);
}


PageEntity& PageEntity::operator=(const PageEntity& other) {
    if (this != &other) {
        // Mevcut icerigi diger PageEntity'den kopyala
        page_numb = other.page_numb;
        for (int i = 0; i < PAGE_SIZE; i++) {
            content[i] = other.content[i];
        }
    }
    return *this;
}

void PageEntity::setMem(int index, int value){
    for (int i = 0; i < PAGE_SIZE; i++)
        (*this)[i] = value;
}
