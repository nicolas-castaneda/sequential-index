#include "SequentialIndex.h"

void printAllFiles(SequentialIndex& si){
    std::cout<<"-----------"<<std::endl;
    std::cout<<"INDEX FILE"<<std::endl;
    si.printIndexFile();
    std::cout<<"AUX FILE"<<std::endl;
    si.printAuxFile();
    std::cout<<"DUPLICATES FILE"<<std::endl;
    si.printDuplicatesFile();
    std::cout<<"-----------"<<std::endl;
}

int main(){
    SequentialIndex si("i.bin", "a.bin", "d.bin");

    //std::cout<<"ADD 2"<<std::endl;
    si.add(Data(2));
    si.add(Data(1));
    si.add(Data(3));
       
    si.add(Data(3));
    si.add(Data(3));
    si.add(Data(3));
    si.add(Data(3));
    si.add(Data(3));

    si.erase(Data(3));
    
    si.add(Data(8));
    
    si.add(Data(8));
    si.add(Data(8));  

    si.add(Data(9));
    si.add(Data(9));

    si.erase(Data(9));

    si.add(Data(12));
    si.add(Data(10));

    si.add(Data(11));
    
    si.erase(Data(8));
    
    printAllFiles(si);

    //std::cout<<"RANGE SEARCH 3"<<std::endl;
    /* Response response = si.rangeSearch(Data(1), Data(0));

    std::cout<<"RESPONSE"<<std::endl;
    for (auto& record : response.records) {
        std::cout<<record<<std::endl;
    }
    std::cout<<"TIME: "<<response.query_time<<std::endl; */
    return 0;
}