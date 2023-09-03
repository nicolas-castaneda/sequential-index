#ifndef SEQUENTIALINDEX_CPP
#define SEQUENTIALINDEX_CPP

#include "SequentialIndex.h"

/*
    Read and write a record from a sequential File
*/
template<typename FileType = std::fstream>
void SequentialIndex::writeHeader(FileType& indexFile, physical_pos next_pos, file_pos next_file){
    try {
        indexFile.seekp(0, std::ios::beg);
        SequentialIndexHeader sih(next_pos, next_file);
        indexFile.write(reinterpret_cast<char*> (&sih), sizeof(SequentialIndexHeader));
    } catch (...) {
        throw std::runtime_error("Couldn't write header");
    }
}

template<typename FileType = std::fstream>
void SequentialIndex::readHeader(FileType& indexFile, SequentialIndexHeader& sih){
    try {
        indexFile.seekp(0, std::ios::beg);
        indexFile.read(reinterpret_cast<char*> (&sih), sizeof(SequentialIndexHeader));
    } catch (...) {
        throw std::runtime_error("Couldn't read header");
    }
    
}

template<typename FileType = std::fstream>
void SequentialIndex::readRecord(FileType& file, SequentialIndexRecord& sir){
    try {
        file.read(reinterpret_cast<char*> (&sir), sizeof(SequentialIndexRecord));
    } catch (...) {
        throw std::runtime_error("Couldn't read record");
    }
}

template<typename FileType = std::fstream>
void SequentialIndex::writeRecord(FileType& file, SequentialIndexRecord& sir){
    try {
        file.write(reinterpret_cast<char*> (&sir), sizeof(SequentialIndexRecord));
    } catch (...) {
        throw std::runtime_error("Couldn't write record");
    }
}

void SequentialIndex::rebuild(){

}

/*
    Helper functions 
*/
void SequentialIndex::createFile(){
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

bool SequentialIndex::fileExists(){
        std::ifstream file(this->indexFilename);
        bool exists = file.good();
        file.close();
        return exists;
}

/*
    Binary search in files
*/
template<typename FileType = std::fstream>
BinarySearchResponse SequentialIndex::binarySearch(FileType& file, Data data){
    BinarySearchResponse bsr;

    size_t header_offset = sizeof(SequentialIndexHeader);
    size_t sequentialIndexSize = sizeof(SequentialIndex);

    physical_pos logical_left = 0;

    file.seekp(0, std::ios::end);
    physical_pos logical_right = (file.tellp() - header_offset) / sequentialIndexSize;
    file.seekp(0, std::ios::beg);

    SequentialIndexRecord sir_cur;
    SequentialIndexRecord sir_prev;
    SequentialIndexRecord sir_next;
    SequentialIndexHeader sih;

    physical_pos physical_current;

    while (logical_left <= logical_right) {
        physical_pos logical_mid = (logical_left + logical_right) / 2;
        file.seekp( header_offset + ( logical_mid * sequentialIndexSize ), std::ios::beg);
        
        if ( file.tellp() != header_offset) { file.seekp(-sequentialIndexSize, std::ios::cur); this->readRecord(file, sir_prev); } 
        else { this->readHeader(file, sih); }
        this->readRecord(file, sir_cur);
        if (file.peek() != EOF) { this->readRecord(file, sir_next); }

        if (sir_cur.data == data) {

            break;
        }


    }

    return bsr;
}

/*
    Query functions
*/
Response SequentialIndex::add(Data data){
    Response response;
    response.startTimer();

    std::fstream indexFile(this->indexFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!indexFile.is_open()) throw std::runtime_error("Couldn't open indexFile");

    indexFile.seekp(0, std::ios::end);
    SequentialIndexRecord sir(data, indexFile.tellp(), -1, -1, INDEXFILE, -1, INDEXFILE);
    this->writeRecord(indexFile, sir);

    response.stopTimer();
    return response;
}


/*
    Print files sequentially
*/
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
