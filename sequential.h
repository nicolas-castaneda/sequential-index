#include "utils.h"

using physical_pos = long;

class SequentialIndexRecord {
    Data data;
    physical_pos initial_position;
    physical_pos next;    
};


class SequentialIndex {
    std::string filename;
    std::string auxfilename;

    void createFile(){
        std::ofstream file(this->filename, std::ios::app | std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("No se pudo crear el archivo");
        file.close();
    }
        
    bool fileExists(){
        std::ifstream file(this->filename);
        bool exists = file.good();
        file.close();
        return exists;
    }

public:
    SequentialIndex(std::string _fileName){
        this->filename = _fileName;
        if (!fileExists()) { createFile(); }
    }
};