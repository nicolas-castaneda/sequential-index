#include <string>
#include <fstream>

class SequentialIndex {
    std::string filename;

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

};