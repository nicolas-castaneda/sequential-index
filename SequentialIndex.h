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

    template<typename FileType = std::fstream>
    void writeRecord(FileType& file, SequentialIndexRecord& sir);

    template<typename FileType = std::fstream>
    void moveReadRecord(FileType& file, physical_pos& pos, SequentialIndexRecord& sir);

    template<typename FileType = std::fstream>
    void moveWriteRecord(FileType& file, physical_pos& pos,SequentialIndexRecord& sir);

    /*
        Helper functions 
    */
    void createFile();
    bool fileExists();
    void rebuild();

    size_t numberIndexRecords();
    size_t numberAuxRecords();
    bool validNumberRecords();

    void getAllRawCurrentRecords(SequentialIndexRecord sir, std::vector<physical_pos>& records);
    void searchAuxFile(Data data, BinarySearchResponse& bir, std::vector<physical_pos>& records, SequentialIndexRecord& sir);

    template <typename FileType = std::fstream>
    void insertDuplicate(FileType& file, SequentialIndexRecord& sir, SequentialIndexRecord& sir_dup);

    template <typename FileType = std::fstream>
    void insertAux(FileType& indexFile, SequentialIndexRecord& sir_init, SequentialIndexRecord& sir, BinarySearchResponse& bsr);

    void insertDuplicateFile(SequentialIndexRecord& sir);
    void insertAuxFile(SequentialIndexRecord& sir);

    template <typename FileType = std::fstream>
    void insertAfterRecord(FileType& file, SequentialIndexRecord& sir_prev, SequentialIndexRecord& sir, SequentialIndexHeader& sih, bool header);

    /*
        Binary search in files
    */
    template<typename FileType = std::fstream>
    BinarySearchResponse binarySearch(FileType& file, Data data);

    Response erase(Data data, Response& response);

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
        Query functions
    */

   // Sequential deben pasar el indexFile name, el auxFile name, el duplicateFile name
   // Pueden pasar ustedes el nombre de la tabla y el nombre del campo y ya lo concateno
   // Para que los archivos respectivos a una tabla sean tipo nombreTabla_nombreCampo_nombreIndice_extras.bin
   // extras por ejemplo yo uso tres files indexFile, auxFile, duplicateFile
   // entonces los nombres de los archivos serian tipo 
   // nombreTabla_nombreCampo_SequentialIndex_indexFile.bin
    // nombreTabla_nombreCampo_SequentialIndex_auxFile.bin
    // nombreTabla_nombreCampo_SequentialIndex_duplicateFile.bin
   // pos debe ser raw position del inicio del record a insertar
   // Response add(Data<key_type> data, physical_pos pos); 
    Response add(Data data);
    Response search(Data data);
    Response rangeSearch(Data begin, Data end);
    Response erase(Data data);

    /*
        Print files sequentially
    */
    void printIndexFile();
    void printAuxFile();
    void printDuplicatesFile();
};

#endif //SEQUENTIALINDEX_H