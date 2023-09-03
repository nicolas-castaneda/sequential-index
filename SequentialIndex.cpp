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
    size_t sequentialIndexSize = sizeof(SequentialIndexRecord);

    physical_pos logical_left = 0;

    file.seekp(0, std::ios::end);
    if (file.tellp() == header_offset) { bsr.location = -2; return bsr;}
    physical_pos physical_last = file.tellp();
    physical_pos logical_right = (physical_last - header_offset) / sequentialIndexSize;
    file.seekp(0, std::ios::beg);

    SequentialIndexHeader sih;
    SequentialIndexRecord sir_prev, sir_cur, sir_next;

    while (logical_left <= logical_right) {
        physical_pos logical_mid = (logical_left + logical_right) / 2;
        physical_pos physical_mid = header_offset + ( logical_mid * sequentialIndexSize );
        
        file.seekp( physical_mid, std::ios::beg);
        this->readRecord(file, sir_cur);
        if ( sir_cur.current_pos != header_offset) { 
            file.seekp(-2*sequentialIndexSize, std::ios::cur); this->readRecord(file, sir_prev); } 
        else { this->readHeader(file, sih); }
        if (sir_cur.current_pos + sequentialIndexSize != physical_last) { 
            file.seekp(sequentialIndexSize, std::ios::cur); this->readRecord(file, sir_next); }

        if (sir_cur.data == data) {
            bsr.location = 0;
            break;
        } else if (sir_cur.current_pos + sequentialIndexSize == physical_last && sir_cur.data < data) {
            bsr.location = 1;
            break;
        } else if (sir_cur.current_pos == header_offset && data < sir_cur.data) {
            bsr.location = -1;
            bsr.setHeader(sih);
            break;
        } else if (sir_prev.data < data && data < sir_cur.data) {
            bsr.location = -1;
            break;
        } else if (sir_cur.data < data && data < sir_next.data) {
            bsr.location = 1;
            break;
        } else if (sir_cur.data < data) {
            logical_left = logical_mid + 1;
        } else {
            logical_right = logical_mid - 1;
        }
    }
    bsr.setRecords(sir_prev, sir_cur, sir_next);

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
    SequentialIndexRecord sir(data, indexFile.tellp(), -1, indexFile.tellp(), INDEXFILE, -1, INDEXFILE);
    this->writeRecord(indexFile, sir);

    response.stopTimer();
    indexFile.close();
    return response;
}

Response SequentialIndex::search(Data data){
    Response response;
    response.startTimer();

    std::fstream indexFile(this->indexFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!indexFile.is_open()) throw std::runtime_error("Couldn't open indexFile");

    BinarySearchResponse bsr = this->binarySearch(indexFile, data);
    std::cout<<bsr<<std::endl;

    response.stopTimer();
    indexFile.close();
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
