#include "sequential_index.hpp"

template<typename KEY_TYPE>
void printAllFiles(SequentialIndex<KEY_TYPE>& si){
    std::cout<<"-----------"<<std::endl;
    std::cout<<"INDEX FILE"<<std::endl;
    si.printIndexFile();
    std::cout<<"AUX FILE"<<std::endl;
    si.printAuxFile();
    std::cout<<"DUPLICATES FILE"<<std::endl;
    si.printDuplicatesFile();
    std::cout<<"-----------"<<std::endl;
}

void printResponse(Response& response){
    std::cout<<"-------"<<std::endl;
    std::cout<<"RESPONSE"<<std::endl;
    for (auto& record : response.records) {
        std::cout<<record<<std::endl;
    }
    std::cout<<"TIME: "<<response.query_time<<std::endl;
    std::cout<<"-------"<<std::endl;
}

int main(){
    std::string table_name = "t1";
    std::string attribute_name = "attr1";
    bool PK = false;

    using attribute_type = int;

    SequentialIndex<attribute_type> si(table_name, attribute_name, PK);

    std::cout<<"ADD 2"<<std::endl;
    si.add(Data(2), 2);

    si.add(Data(1), 1);
    
    si.add(Data(3), 3);
    
    si.add(Data(3), 3);

    si.add(Data(3), 3);


    si.add(Data(3), 3);
    si.add(Data(3), 3);
    si.add(Data(3), 3);

    si.erase(Data(3));
/*    
    si.add(Data(8), 8);
    
    si.add(Data(8), 8);
    si.add(Data(8), 8);  

    si.add(Data(9), 9);
    si.add(Data(9), 9);

    si.erase(Data(9));

    si.add(Data(12), 12);
    si.add(Data(10), 10);

    si.add(Data(11), 11);
    
    si.erase(Data(8)); */
    
    printAllFiles(si); 

    std::cout<<"RANGE SEARCH 3"<<std::endl;
    Response response = si.rangeSearch(Data(1), Data(3));

    printResponse(response);
    return 0;
}