#include "SequentialIndex.h"

int main(){
    SequentialIndex si("i.bin", "a.bin", "r.bin");
    std::cout<<"INDEX FILE"<<std::endl;
    si.printIndexFile();

    std::cout<<"AUX FILE"<<std::endl;
    si.printAuxFile();
    return 0;
}