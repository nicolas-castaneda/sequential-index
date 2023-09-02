#ifndef SEQUENTIALINDEX_H
#define SEQUENTIALINDEX_H

#include "SequentialIndexHeader.h"
#include "SequentialIndexRecord.h"
#include "BinarySearchResponse.h"

class SequentialIndex {
    std::string indexFilename;
    std::string auxFilename;
    std::string duplicatesFilename;

    /*
        Read and write a record from a sequential File
    */
    template<typename FileType = std::fstream>
    void writeHeader(FileType& indexFile, physical_pos next_pos, file_pos next_file);

    template<typename FileType = std::fstream>
    void readHeader(FileType& indexFile, SequentialIndexHeader& sih);

    template<typename FileType = std::fstream>
    void readRecord(FileType& file, SequentialIndexRecord& sir);

    /*
        Helper functions 
    */
    void createFile();
    bool fileExists();

    /*
        Navigate function
    */
    template<typename FileType = std::fstream>
    BinarySearchResponse binarySearch(FileType& file, Data data);

    /*
        Query functions
    */
    Response add(Data data);
    Response search(Data data);
    Response rangeSearch(Data begin, Data end);
    Response erase(Data data);

public:
    /*
        Constructor
    */
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

    /*
        Print files sequentially
    */
    void printIndexFile();
    void printAuxFile();
};

#endif //SEQUENTIALINDEX_H