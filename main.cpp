#include "sequential_index.hpp"
#include "random"

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

template<typename KEY_TYPE>
void addNRandomRecord(SequentialIndex<KEY_TYPE>& si, int N, KEY_TYPE min = 0, KEY_TYPE max = 100){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);

    for(double i = 0; i < N; i++){
        si.add(Data(i), 0);
    }
}

int main(){
    std::string table_name = "t1";
    std::string attribute_name = "attr1";
    bool PK = false;

    using attribute_type = double;

    SequentialIndex<attribute_type> si(table_name, attribute_name, PK);


    addNRandomRecord(si, 199000);
    
    /* printAllFiles(si); */

    /*  Response response = si.rangeSearch(Data(1.0), Data(10.5));

    printResponse(response); */
    return 0;
}