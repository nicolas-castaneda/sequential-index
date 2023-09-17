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
    std::cout<<"TIME: "<<response.query_time.count()<<" ms"<<std::endl;
    std::cout<<"-------"<<std::endl;
}

template<typename KEY_TYPE>
void addNRandomRecord(SequentialIndex<KEY_TYPE>& si, int N, KEY_TYPE min = 0, KEY_TYPE max = 99){
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

    Response response;
    std::vector<std::pair<Data<attribute_type>, physical_pos>> records;

    for(double i = 0; i < 12; i++){
        records.push_back(std::make_pair(Data<attribute_type>(2.08), 0));
        
    }
    
    // PRIMERO BULK LOAD
    si.bulkLoad(records);


    // DESPUES AÑADIR NORMAL
    addNRandomRecord(si, 10);

    printAllFiles(si);

    response = si.rangeSearch(Data(0.0), Data(2.08));
    printResponse(response); 

    response = si.search(Data(0.0));

    printResponse(response);
    

    return 0;
}