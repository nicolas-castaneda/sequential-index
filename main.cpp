#include "SequentialIndex.h"

int main(){
    SequentialIndex si("i.bin", "a.bin", "d.bin");
    si.search(Data(3));

    //std::cout<<"INDEX FILE"<<std::endl;
    //std::cout<<"ADD 2"<<std::endl;
    si.add(Data(2));
   
    si.search(Data(1));
    si.search(Data(2));
    si.search(Data(3));
    si.search(Data(4));
    si.search(Data(5));
    si.search(Data(6));
    si.search(Data(7));
    
    std::cout<<"INDEX FILE"<<std::endl;
    std::cout<<"ADD 4"<<std::endl;
    si.add(Data(4));
    si.printIndexFile();

    si.search(Data(1));
    si.search(Data(2));
    si.search(Data(3));
    si.search(Data(4));
    si.search(Data(5));
    si.search(Data(6));
    si.search(Data(7));
    
    std::cout<<"INDEX FILE"<<std::endl;
    std::cout<<"ADD 6"<<std::endl;
    si.add(Data(6));
    si.search(Data(1));
    si.search(Data(2));
    si.search(Data(3));
    si.search(Data(4));
    si.search(Data(5));
    si.search(Data(6));
    si.search(Data(7));

    std::cout<<"INDEX FILE"<<std::endl;
    si.printIndexFile();

    std::cout<<"AUX FILE"<<std::endl;
    si.printAuxFile();
    
    return 0;
}