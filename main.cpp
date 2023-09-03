#include "SequentialIndex.h"

int main(){
    SequentialIndex si("i.bin", "a.bin", "r.bin");

    si.add(Data(1));
    si.add(Data(2));
    si.add(Data(3));

    std::cout<<"INDEX FILE"<<std::endl;
    si.printIndexFile();

    std::cout<<"AUX FILE"<<std::endl;
    si.printAuxFile();
    return 0;
}