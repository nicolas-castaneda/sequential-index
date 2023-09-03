#include "SequentialIndex.h"

int main(){
    SequentialIndex si("i.bin", "a.bin", "d.bin");
    si.search(Data(3));

    si.add(Data(2));
    si.add(Data(4));
    
    std::cout<<"INDEX FILE"<<std::endl;
    std::cout<<"ADD 6"<<std::endl;
    si.add(Data(6));


    std::cout<<"INDEX FILE"<<std::endl;
    si.printIndexFile();

    std::cout<<"AUX FILE"<<std::endl;
    si.printAuxFile();
    
    Response r = si.search(Data(7));

    std::cout<<"RESPONSE"<<std::endl;
    std::cout<<"Query time: "<<r.query_time<<std::endl;
    for (auto record : r.records) {
        std::cout<<record<<std::endl;
    }
    return 0;
}