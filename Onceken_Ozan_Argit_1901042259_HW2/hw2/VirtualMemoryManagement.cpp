#include "VirtualMemoryManagement.h"



/**
 * 1- Second Chance
 * 2- LRU
 * 3- WSClock
 * 4- Fifo
*/
int VirtualMemoryManagement::page_replacement_algorithm ; 
void VirtualMemoryManagement::virtualMain(){
    
    std::cout<<"Algorithms did not worked and the virtual adresses is:\n"<<std::endl;
    VirtualMemoryManagement::showSimulatedAdresses();
    std::cout<<"Enter number for algorithm:\n"<<std::endl;
    std::cout<<"1-Second Chance Algorithm:\n"<<std::endl;
    std::cout<<"2-LRU Algorithm:\n"<<std::endl;
    std::cout<<"3-WSClock Algorithm:\n"<<std::endl;
    std::cout<<"4-FIFO Algorithm:\n"<<std::endl;
    std::cin>>page_replacement_algorithm;
    if(page_replacement_algorithm==1)
        std::cout<<"Second Chance Algorithm selected.\n"<<std::endl;
    else if(page_replacement_algorithm==2)
        std::cout<<"LRU Algorithm selected.\n"<<std::endl;
    else if(page_replacement_algorithm==3)
        std::cout<<"WSClock Algorithm selected.\n"<<std::endl;
    else if(page_replacement_algorithm==4)
        std::cout<<"FIFO Algorithm selected.\n"<<std::endl;
}

VirtualMemoryManagement::VirtualMemoryManagement()
{
    // Constructor
    initializeDisk();
    fillRandomVirtualAddress();

}

void VirtualMemoryManagement::setPageTableEntry(int input, int output) {
    pageTable[input] = output;
}



int VirtualMemoryManagement::getPageTableEntry(int page_number) {
    auto it = pageTable.find(page_number);
    if (it != pageTable.end()) {
        return it->second; // page numberi alir virtual addressi verir
    } else {
        return -1; // İlgili input değeri bulunamadı ise, -1 döndür
    }
}

int VirtualMemoryManagement::getPageFrameToVirtualAddress(int page_number) {
    auto it = virtual_frame_table.find(page_number);
    if (it != virtual_frame_table.end()) {
        return it->second; // page numberi alir virtual addressi verir
    } else {
        return -1; // İlgili input değeri bulunamadı ise, -1 döndür
    }
}


bool VirtualMemoryManagement::isPhysicalMemoryFull(){
    return PHYSICAL_MEMORY == physical_mem_queue
    .size();
}

/**
 * Bu method da ilk basta tek tek dolduracagi icin, hic bir eleman olmadigi icin
 * fiziksel memory de o yuzden queue nin sizeni dondurur, 0
*/
int VirtualMemoryManagement::find_empty_physical_mem()
{
    if (physical_mem_queue.size()>=PHYSICAL_MEMORY)
        return -1;
    
    return physical_mem_queue.size();
}

int VirtualMemoryManagement::getPageFromDiskToPhyMem(int disk_index, int ram_index)
{
    physicalMemory[ram_index] = disk[disk_index];   //diskten alir
    return 0;
}

void VirtualMemoryManagement::updateOldVirtualAddress(int old_page_numb){
    int virtual_address = getPageFrameToVirtualAddress(old_page_numb);
    if (virtual_address<0 || virtual_address>=VIRTUAL_ADDRESS){
        std::cerr<<"virtual address hatali en az gecerli bir virtual address olmali"<<std::endl;
        std::exit(1);
    }

    VirtualAddressEntity& v= virtualAddress[virtual_address]; 
    v.setAbsent(true);
    v.setModified(false);
    v.setRead(false);
    v.setWhereInPhysicalMem(-1);
}

void VirtualMemoryManagement::updateNewVirtualAddress(int new_page_numb, int physical_address){
    int virtual_address = getPageFrameToVirtualAddress(new_page_numb);
    VirtualAddressEntity& v = virtualAddress[virtual_address];
    v.setAbsent(false);
    v.setModified(false);
    v.setPageNumber(new_page_numb);
    v.setRead(false);
    v.setWhereInPhysicalMem(physical_address);
    v.setTimestamp();
    v.delaySimulation(1);
}

void VirtualMemoryManagement::updateInitVirtualAddress(int disk_index, int ram_index, int virtual_address){
        
    VirtualAddressEntity& virtual_address_entity = virtualAddress[virtual_address];
    
    if(virtual_address_entity.isAbsent()==false){
        std::cerr<<"!@#Virtual adres memoryde olmasi lazimm false"<<std::endl;
        std::exit(1);
    }
    else{
        virtual_address_entity.setAbsent(false);
        virtual_address_entity.setRead(false);
        virtual_address_entity.setModified(false);
        virtual_address_entity.setPageNumber(disk_index);
        virtual_address_entity.setWhereInPhysicalMem(ram_index);
        virtual_address_entity.setTimestamp();
    }
}


void VirtualMemoryManagement::updateVirtualAddress(int disk_index, int ram_index, int virtual_address){
        
    VirtualAddressEntity& virtual_address_entity = virtualAddress[virtual_address];
    
    if(virtual_address_entity.isAbsent()==true){
        std::cerr<<"!@#Virtual adres memoryde olmasi lazimm"<<std::endl;
        std::exit(1);
    }
    else{
        virtual_address_entity.setAbsent(false);
        virtual_address_entity.setRead(true);
        virtual_address_entity.setModified(false);
        virtual_address_entity.setPageNumber(disk_index);
        virtual_address_entity.setWhereInPhysicalMem(ram_index);
    }

}

void VirtualMemoryManagement::fifoWay(int new_page_num){
    //burda fifo yontemiyle cozucez
    int first_in = physical_mem_queue.front();
    physical_mem_queue.pop();
    int physical_address = first_in;
    int old_page_numb= physicalMemory[physical_address].getPageNumb();

    int virtual_address = getPageTableEntry(old_page_numb);
    VirtualAddressEntity& v  = virtualAddress[virtual_address];

    //icinde olan veri modified ise diske yaz
    if (v.isModified())
        physicalMemtoDisk(physical_address, old_page_numb);
    
    //diskten veriyi al olan yere yaz first_in 
    getPageFromDiskToPhyMem(new_page_num, physical_address);
    //virtual adresi guncelle
    updateVirtualAddress(new_page_num,physical_address, virtual_address);
    //page table i guncelle
    setPageTableEntry(new_page_num, virtual_address);
    physical_mem_queue.push(physical_address);
}

void VirtualMemoryManagement::secondChanceAlgorithm(int new_page_number){

    for (int i = 0; i < PHYSICAL_MEMORY; i++){
        PageEntity& p = physicalMemory[i];int current_page = p.getPageNumb();
        int current_virtual_address = getPageFrameToVirtualAddress(current_page);        
        VirtualAddressEntity& current_v = virtualAddress[current_virtual_address];
        if (current_v.isAbsent()==true){
            std::cerr<<"Hata var mutlaka ram de olmasi lazim"<<std::endl;//bu hiç görülmeyecek ama 2. bir test amaçlı kaydettim
            std::exit(1);
        }
        if (current_v.isRead())//buna bir sans daha ver ve read kismini false yap
            {
            current_v.setRead(false);        
            }
        else{//ramdeki yeri bosalticam
            int physical_address = current_v.getWhereInPhysicalMem();//degistirilecek olan fiziksel yer
            //modifiye edildiyse diske yazcam
            if(current_v.isModified()){
                physicalMemtoDisk(physical_address,current_v.getPageNumber());//diske kaydediyorum
            }
            //rame yeni page framei atıyorum
            getPageFromDiskToPhyMem(new_page_number, physical_address);
            //yeni virtual adresi guncelliyorum
            updateNewVirtualAddress(new_page_number, physical_address);
            //eski virtual adresi guncelliyorum
            updateOldVirtualAddress(current_v.getPageNumber());
            return;
        }
    }
}

void VirtualMemoryManagement::lruAlgorithm(int new_page_number)
{
    auto min_timestamp = std::chrono::system_clock::now();
    int min_timestamp_index = -1;

    for (int i = 0; i < VIRTUAL_ADDRESS; i++)
    {
        if (!virtualAddress[i].isAbsent() && virtualAddress[i].getTimestamp() < min_timestamp)
        {
            min_timestamp = virtualAddress[i].getTimestamp();
            min_timestamp_index = i;
        }
    }

    if (min_timestamp_index == -1)
    {
        std::cerr << "Hata var, mutlaka RAM'de olması lazım" << std::endl;
        std::exit(1);
    }

    VirtualAddressEntity& victim_page = virtualAddress[min_timestamp_index];
    victim_page.whoAmI();

    int physical_address = victim_page.getWhereInPhysicalMem();

    if (victim_page.isModified())
    {
        physicalMemtoDisk(physical_address, victim_page.getPageNumber());
    }

    std::cout << "physical address: " << physical_address << std::endl;

    getPageFromDiskToPhyMem(new_page_number, physical_address);

    updateNewVirtualAddress(new_page_number, physical_address);
    updateOldVirtualAddress(victim_page.getPageNumber());
}


void VirtualMemoryManagement::WSClockAlgorithm(int new_page_number){
    for (int i = 0; i < PHYSICAL_MEMORY; i++){
        PageEntity& p = physicalMemory[i];
        
        int current_page = p.getPageNumb();
        int current_virtual_address = getPageFrameToVirtualAddress(current_page);        
        VirtualAddressEntity& current_v = virtualAddress[current_virtual_address];
        if (current_v.isAbsent()==true){
            std::cerr<<"Hata var mutlaka ram de olmasi lazim"<<std::endl;
            std::exit(1);
        }
        else{//ramdeki yeri bosalticaz
            int physical_address = current_v.getWhereInPhysicalMem();
            //modifiye edildiyse diske yazicaz
            if(current_v.isModified()==false){
                physicalMemtoDisk(physical_address,current_v.getPageNumber());//diske kaydet//rame yeni page frami at
                getPageFromDiskToPhyMem(new_page_number, physical_address);
                //yeni virtual adresi guncelle
                updateNewVirtualAddress(new_page_number, physical_address);
                //eski virtual adresi guncelle
                updateOldVirtualAddress(current_v.getPageNumber());
                return;
            }
        }
    }
    PageEntity& p = physicalMemory[0];
        
    int current_page = p.getPageNumb();
    int current_virtual_address = getPageFrameToVirtualAddress(current_page);        
    VirtualAddressEntity& current_v = virtualAddress[current_virtual_address];
    if (current_v.isAbsent()==true){
        std::cerr<<"Hata var mutlaka ram de olmasi lazim"<<std::endl;
        std::exit(1);
    }
    else{//ramdeki yeri bosalticaz
        int physical_address = current_v.getWhereInPhysicalMem();
        //modifiye edildiyse diske yazicaz
        if(current_v.isModified()==false){
            physicalMemtoDisk(physical_address,current_v.getPageNumber());//diske kaydet//rame yeni page frami at
            getPageFromDiskToPhyMem(new_page_number, physical_address);
            //yeni virtual adresi guncelle
            updateNewVirtualAddress(new_page_number, physical_address);
            //eski virtual adresi guncelle
            updateOldVirtualAddress(current_v.getPageNumber());
            return;
        }
    }
}


void VirtualMemoryManagement::fifoWay2(int new_page_num){
    //burda fifo yontemiyle cozucez
    int first_in = physical_mem_queue.front(); 
    physical_mem_queue.pop();
    int physical_address = first_in;
    int old_page_numb= physicalMemory[physical_address].getPageNumb();
    // int new_virtual_address = getPageFrameToVirtualAddress(new_page_num);

    int old_virtual_address = getPageTableEntry(old_page_numb);
    
    VirtualAddressEntity& v_old  = virtualAddress[old_virtual_address];

    //icinde olan veri modified ise diske yaz
    if (v_old.isModified())
        physicalMemtoDisk(physical_address, old_page_numb);
    
    //diskten veriyi al olan yere yaz first_in 
    getPageFromDiskToPhyMem(new_page_num, physical_address);
    //virtual adresi guncelle
    updateOldVirtualAddress(old_page_numb);
    updateNewVirtualAddress(new_page_num,physical_address);
    physical_mem_queue.push(physical_address);
}



void VirtualMemoryManagement::physicalMemtoDisk(int mem_index, int disk_index)
{
    PageEntity& de = disk[disk_index];
    PageEntity& me = physicalMemory[mem_index];
    if (de.getPageNumb()!=me.getPageNumb()){
        std::cerr<<"There is an error: Page numbers are not stable! Check page table"<<std::endl;
        std::exit(1);
    }
    de = me;
    // disk[disk_index] = physicalMemory[mem_index];
}

void VirtualMemoryManagement::fillRandomVirtualAddress(){
    std::vector<int> array;

    for (int i = 0; i < VIRTUAL_ADDRESS; i++)
        array.push_back(i);
    

    std::random_device rd;
    std::mt19937 gen(rd());

    std::shuffle(array.begin(), array.end(), gen);

    for (int i = 0; i < VIRTUAL_ADDRESS; i++)
    {
        int key = array[i];
        virtualAddress[i].setPageNumber(key);
        virtual_frame_table.insert(std::make_pair(key, i));
    }
    

}

void VirtualMemoryManagement::pageInfo(int page_number){
    int virt_addr = getPageTableEntry(page_number);
    virtualAddress[virt_addr].whoAmI();
}

PageEntity VirtualMemoryManagement::getPage2(int page_number)
{
    int virtual_address = getPageFrameToVirtualAddress(page_number);
    VirtualAddressEntity& virtual_entity = virtualAddress[virtual_address];
    // virtual_entity.getWhereInPhysicalMem

    if (virtual_entity.isAbsent()){//there is not already taken to physical memory
        std::cout<<"Not on the physical memory, page fault"<<std::endl;
        //Page fault
        
        return pageFault2(page_number);         
    }
    else{
        VirtualAddressEntity& v = virtualAddress[virtual_address];
        v.setRead(true);
        v.setTimestamp();
        int index_in_physical_mem = virtualAddress[virtual_address].getWhereInPhysicalMem();
        return  physicalMemory[index_in_physical_mem];
    }
    
    return PageEntity();
}

PageEntity VirtualMemoryManagement::pageFault2(int page_number)
{
    PageEntity p;


    if (isPhysicalMemoryFull()){ 

            if (VirtualMemoryManagement::page_replacement_algorithm == 0){//fifo algorithm
                fifoWay(page_number);
            }
            else if (page_replacement_algorithm==1){    //second chance algorithm
                secondChanceAlgorithm(page_number);
            }
            else if(page_replacement_algorithm==2){     //LRU algorithm
                lruAlgorithm(page_number);
            }else if(page_replacement_algorithm==3){     //WSClockAlgorithm algorithm
                WSClockAlgorithm(page_number);
            }else if(page_replacement_algorithm==4){     //fifo algorithm
                fifoWay2(page_number);
            }

        }
    else{
        //memi dolduruyorm
        int ram_index = find_empty_physical_mem();

        int virtual_address = getPageFrameToVirtualAddress(page_number); // gerekli virtual adresi al
        if (ram_index==-1){
            std::cerr << "!@#Fiziksel memoryde yer yok!!!! bir onceki if kismina girmeli ama girmedi!!!!" << std::endl;
            std::exit(1);
        }
        //fiziksel memorye koy
        getPageFromDiskToPhyMem(page_number, ram_index);        
        //page tablei guncelle
        setPageTableEntry(page_number, virtual_address);
        //virtual addresi guncelle
        updateInitVirtualAddress(page_number, ram_index, virtual_address);
        //queue yi arttir
        physical_mem_queue.push(ram_index);

        return physicalMemory[ram_index];
    }

    int return_virt_add = getPageFrameToVirtualAddress(page_number);


    return physicalMemory[virtualAddress[return_virt_add].getWhereInPhysicalMem()];
}

void VirtualMemoryManagement::setPage(int page_number, int value){
    int virtual_address = getPageFrameToVirtualAddress(page_number);
    VirtualAddressEntity& v = virtualAddress[virtual_address];
    
    if (v.isAbsent()){//there is not already taken to physical memory
        std::cout<<"Not on the physical memory"<<std::endl;
        fifoWay2(page_number);
    }
    int index_in_physical_mem = virtualAddress[virtual_address].getWhereInPhysicalMem();
    
    virtualAddress[virtual_address].setModified(true);

    //TODO: simdi degistirilecek
    physicalMemory[index_in_physical_mem].setMem(0, 31);
}




void VirtualMemoryManagement::showSimulatedAdresses(int restriction){

    for (int i = 0; i < restriction; i++)
    {
        std::cout<<i<<"- "<< virtualAddress[i].getPageNumber();
        
        if (i<PHYSICAL_MEMORY){
            std::cout<<"\t - "<< physicalMemory[i].getPageNumb()<<std::endl;    
        }
        else
            std::cout<<std::endl;
    }
}

void VirtualMemoryManagement::initializeVirtualMemory(){
    this->initializeDisk();
    this->initializePhysicalMemory();
    this->setupVirtualAdress();
}

void VirtualMemoryManagement::initializeDisk()
{
    for (int i = 0; i < DISK_MEMORY; i++){
        
        this->disk[i].setPageNumb(i);
        // std::cout<<"i: "<<i<<std::endl;
        this->disk[i].initRandom();
    }
}

void VirtualMemoryManagement::initializePhysicalMemory(){
    for (int i = 0; i < PHYSICAL_MEMORY; i++){
        physicalMemory[i] = disk[i];
        physical_mem_queue.push(i);
    }
}

void VirtualMemoryManagement::setupVirtualAdress(){
    for (int i = 0; i < PHYSICAL_MEMORY; i++){
        connectVirtToPhysicalMem(i, i);
    }
    
}

//fiziksel memorydeki page frame i virtual page e baglar
//suana kadar page fault falan yok
void VirtualMemoryManagement::connectVirtToPhysicalMem(int virt_addr, int physical_addr){
    // virtualAddress->setWhereInPhysicalMem(i);
    VirtualAddressEntity* va = &virtualAddress[virt_addr];  //suan fiziksel memoryde
    va->setAbsent(false);   //artik memoryde
    va->setWhereInPhysicalMem(physical_addr);
    this->setPageTableEntry(physicalMemory[physical_addr].getPageNumb(),virt_addr );
    va->setPageNumber(physicalMemory[physical_addr].getPageNumb());
}