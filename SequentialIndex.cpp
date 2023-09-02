#ifndef SEQUENTIALINDEX_CPP
#define SEQUENTIALINDEX_CPP

#include "SequentialIndex.h"

void SequentialIndex::printIndexFile() {
    std::fstream indexFile(this->indexFilename, std::ios::in | std::ios::out | std::ios::binary);
    SequentialIndexHeader sih;
    SequentialIndexRecord sir;
    try {
        while(indexFile.peek() != EOF) {
            if (indexFile.tellg() == 0) {
                this->readHeader(indexFile, sih);
                std::cout<<sih<<std::endl;
            } else {
                this->readRecord(indexFile, sir);
                std::cout<<sir<<std::endl;
            }
        }
    } catch (std::runtime_error) {
        indexFile.close();
    }
    
    indexFile.close();
}

void SequentialIndex::printAuxFile() {
    std::fstream auxFile(this->auxFilename, std::ios::in | std::ios::out | std::ios::binary);
    SequentialIndexRecord sir;
    try {
        while(auxFile.peek() != EOF) {
            this->readRecord(auxFile, sir);
            std::cout<<sir<<std::endl;
        }
    } catch (std::runtime_error) {
        auxFile.close();
    }
    auxFile.close();
}

#endif //SEQUENTIALINDEX_CPP
