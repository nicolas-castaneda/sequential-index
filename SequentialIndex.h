#ifndef SEQUENTIALINDEX_H
#define SEQUENTIALINDEX_H

#include "SequentialIndexHeader.h"
#include "SequentialIndexRecord.h"

class SequentialIndex {
    std::string indexFilename;
    std::string auxFilename;
    std::string duplicatesFilename;

    template<typename FileType = std::fstream>
    void writeHeader(FileType& indexFile, physical_pos next_pos, file_pos next_file){
        try {
            indexFile.seekp(0, std::ios::beg);
            SequentialIndexHeader sih(next_pos, next_file);
            indexFile.write(reinterpret_cast<char*> (&sih), sizeof(SequentialIndexHeader));
        } catch (...) {
            throw std::runtime_error("Couldn't write header");
        }
    }

    template<typename FileType = std::fstream>
    void readHeader(FileType& indexFile, SequentialIndexHeader& sih){
        try {
            indexFile.seekp(0, std::ios::beg);
            indexFile.read(reinterpret_cast<char*> (&sih), sizeof(SequentialIndexHeader));
        } catch (...) {
            throw std::runtime_error("Couldn't read header");
        }
        
    }

    template<typename FileType = std::fstream>
    void readRecord(FileType& file, SequentialIndexRecord& sir){
        try {
            file.read(reinterpret_cast<char*> (&sir), sizeof(SequentialIndexRecord));
        } catch (...) {
            throw std::runtime_error("Couldn't read record");
        }
    }

    void createFile(){
        std::ofstream indexFile(this->indexFilename, std::ios::app | std::ios::binary);
        if(!indexFile.is_open()) throw std::runtime_error("Couldn't create indexFile");
        writeHeader<decltype(indexFile)> (indexFile, -1, INDEXFILE);
        indexFile.close();

        std::ofstream auxFile(this->auxFilename, std::ios::app | std::ios::binary);
        if(!auxFile.is_open())  throw std::runtime_error("Couldn't create auxFile"); 
        auxFile.close();

        std::ofstream duplicatesFile(this->duplicatesFilename, std::ios::app | std::ios::binary);
        if(!duplicatesFile.is_open()) throw std::runtime_error("Couldn't create duplicatesFile");
        duplicatesFile.close();
    }
        
    bool fileExists(){
        std::ifstream file(this->indexFilename);
        bool exists = file.good();
        file.close();
        return exists;
    }

public:
    SequentialIndex(
        std::string _indexFilename, 
        std::string _auxFilename,
        std::string _duplicatesFilename
    ) {
        this->indexFilename = _indexFilename;
        this->auxFilename = _auxFilename;
        this->duplicatesFilename = _duplicatesFilename;
        if (!fileExists()) { createFile(); }
    }

    void printIndexFile();
    void printAuxFile();
};

#endif //SEQUENTIALINDEX_H