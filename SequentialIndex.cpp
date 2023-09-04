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

template<typename FileType = std::fstream>
    void SequentialIndex::moveReadRecord(FileType& file, physical_pos& pos, SequentialIndexRecord& sir){
    try {
        file.seekp(pos, std::ios::beg);
        file.read(reinterpret_cast<char*> (&sir), sizeof(SequentialIndexRecord));
    } catch (...) {
        throw std::runtime_error("Couldn't move read record");
    }
}

template<typename FileType = std::fstream>
void SequentialIndex::moveWriteRecord(FileType& file, physical_pos& pos,SequentialIndexRecord& sir){
    try {
        file.seekp(pos, std::ios::beg);
        file.write(reinterpret_cast<char*> (&sir), sizeof(SequentialIndexRecord));
    } catch (...) {
        throw std::runtime_error("Couldn't move write record");
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
    
    try {
        physical_pos logical_left = 0;

        file.seekp(0, std::ios::end);
        if (file.tellp() == header_offset) { bsr.location = EMPTY_FILE; return bsr;}
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
                bsr.location = REC_CUR;
                break;
            } else if (sir_cur.current_pos + sequentialIndexSize == physical_last && sir_cur.data < data) {
                bsr.location = REC_NEXT;
                break;
            } else if (sir_cur.current_pos == header_offset && data < sir_cur.data) {
                bsr.location = REC_PREV;
                bsr.setHeader(sih);
                break;
            } else if (sir_prev.data < data && data < sir_cur.data) {
                bsr.location = REC_PREV;
                break;
            } else if (sir_cur.data < data && data < sir_next.data) {
                bsr.location = REC_NEXT;
                break;
            } else if (sir_cur.data < data) {
                logical_left = logical_mid + 1;
            } else {
                logical_right = logical_mid - 1;
            }
        }
        bsr.setRecords(sir_prev, sir_cur, sir_next);
    } catch (...) {
        throw std::runtime_error("Couldn't binary search");
    }
    return bsr;
}

/*
    Query functions
*/

void SequentialIndex::insertDuplicateFile(SequentialIndexRecord& sir){
    std::fstream duplicatesFile(this->duplicatesFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!duplicatesFile.is_open()) throw std::runtime_error("Couldn't open duplicatesFile");

    duplicatesFile.seekp(0, std::ios::end);
    physical_pos physical_pos = duplicatesFile.tellp();

    sir.setCurrent(physical_pos, DUPFILE);

    try {
        this->writeRecord(duplicatesFile, sir);
    } catch (...) {
        duplicatesFile.close();
        throw std::runtime_error("Couldn't insert duplicate");
    }
    duplicatesFile.close();
}

void SequentialIndex::insertAuxFile(SequentialIndexRecord& sir){
    std::fstream auxFile(this->auxFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!auxFile.is_open()) throw std::runtime_error("Couldn't open auxFile");

    auxFile.seekp(0, std::ios::end);
    physical_pos physical_pos = auxFile.tellp();

    sir.setCurrent(physical_pos, AUXFILE);

    try {
        this->writeRecord(auxFile, sir);
    } catch (...) {
        auxFile.close();
        throw std::runtime_error("Couldn't insert auxFile");
    }
    
    auxFile.close();
};

template <typename FileType = std::fstream>
void SequentialIndex::insertAfterRecord(FileType& file, SequentialIndexRecord& sir_prev, SequentialIndexRecord& sir, SequentialIndexHeader& sih, bool header){
    try {
        if (header) {
            sir.setNext(sih.next_pos, sih.next_file);
            this->insertAuxFile(sir);
            this->writeHeader(file, sir.current_pos, sir.current_file);
        } else {
            sir.setNext(sir_prev.next_pos, sir_prev.next_file);
            this->insertAuxFile(sir);
            sir_prev.setNext(sir.current_pos, sir.current_file);
            file.seekp(sir_prev.current_pos, std::ios::beg);
            this->writeRecord(file, sir_prev);
        }
    } catch (...) {
        throw std::runtime_error("Couldn't insert between records");
    }
}

template <typename FileType = std::fstream>
void SequentialIndex::insertDuplicate(FileType& file, SequentialIndexRecord& sir, SequentialIndexRecord& sir_dup){
    try {
        sir_dup.setDupPos(sir.dup_pos);
        this->insertDuplicateFile(sir_dup);
        sir.setDupPos(sir_dup.current_pos);
        this->moveWriteRecord(file, sir.current_pos, sir);
    } catch (...) {
        throw std::runtime_error("Couldn't insert duplicate");
    }
}

template <typename FileType = std::fstream>
void SequentialIndex::insertAux(FileType& indexFile, SequentialIndexRecord& sir_init, SequentialIndexRecord& sir, BinarySearchResponse& bsr){
    std::fstream auxFile(this->auxFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!auxFile.is_open()) throw std::runtime_error("Couldn't open auxFile");

    SequentialIndexRecord sir_prev = sir_init;
    SequentialIndexRecord sir_cur;
    
    try {
        if (bsr.header) {
            auxFile.seekp(bsr.sih.next_pos, std::ios::beg);
        } else {
            auxFile.seekp(sir_prev.next_pos, std::ios::beg);
        }
        this->readRecord(auxFile, sir_cur);

        bool inserted = false;

        if (bsr.header) {
            if (sir.data < sir_cur.data) {
                this->insertAfterRecord(auxFile, sir_init, sir, bsr.sih, bsr.header);
                inserted = true;
            } else if (sir.data == sir_cur.data) {
                this->insertDuplicate(auxFile, sir_cur, sir);
                inserted = true;
            } else if (sir_cur.next_file == INDEXFILE) {
                this->insertAfterRecord(auxFile, sir_cur, sir, bsr.sih, !bsr.header);
                inserted = true;
            } else {
                sir_prev = sir_cur;
                this->moveReadRecord(auxFile, sir_cur.next_pos, sir_cur);
                bsr.header = false;
            }
        } else if (sir_prev.data < sir.data && sir.data < sir_cur.data) {
            this->insertAfterRecord(indexFile, sir_prev, sir, bsr.sih, bsr.header);
            inserted = true;
        } 

        if (!inserted) {
            while (sir_cur.next_file != INDEXFILE) {
                if (sir_cur.data == sir.data) { 
                    this->insertDuplicate(auxFile, sir_cur, sir);
                    break;
                }
                else if (sir_prev.data < sir.data && sir.data < sir_cur.data) {
                    this->insertAfterRecord(auxFile, sir_prev, sir, bsr.sih, bsr.header);  
                    break;      
                }
                sir_prev = sir_cur;
                this->moveReadRecord(auxFile, sir_cur.next_pos, sir_cur);
            }

            if (sir_cur.next_file == INDEXFILE) {
                if (sir_cur.data == sir.data) { 
                    this->insertDuplicate(auxFile, sir_cur, sir);
                } else if (sir_cur.data < sir.data) {
                    this->insertAfterRecord(auxFile, sir_cur, sir, bsr.sih, bsr.header);
                } else {
                    this->insertAfterRecord(auxFile, sir_prev, sir, bsr.sih, bsr.header);
                }
            }
        }
    } catch (...) {
        auxFile.close();
        throw std::runtime_error("Couldn't insert auxFile");
    }
    
    auxFile.close();
}

Response SequentialIndex::add(Data data){
    Response response;
    response.startTimer();

    std::fstream indexFile(this->indexFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!indexFile.is_open()) throw std::runtime_error("Couldn't open indexFile");

    SequentialIndexRecord sir;
    sir.setData(data);
    sir.setRawPos(data.numero);
    sir.setDupPos(-1);

    try {
        BinarySearchResponse bsr = this->binarySearch(indexFile, data);
        if (bsr.location == EMPTY_FILE) { 
            // Insert as first record
            sir.setCurrent(sizeof(SequentialIndexHeader), INDEXFILE);
            sir.setNext(-1, INDEXFILE);
            this->writeHeader(indexFile, sizeof(SequentialIndexHeader), INDEXFILE); 
            this->writeRecord(indexFile, sir);
            response.records.push_back(sir.raw_pos);
        } else if (bsr.header && bsr.sih.next_file == INDEXFILE) {
            // Insert between header and first record
            this->insertAfterRecord(indexFile, bsr.sir_prev, sir, bsr.sih, bsr.header);
        } else if (bsr.header ) {
            // Move to aux file from header
            this->insertAux(indexFile, bsr.sir_prev, sir, bsr);
        } else if (bsr.location == -1 && bsr.sir_prev.next_file == INDEXFILE) {
            // Insert between prev and cur
            this->insertAfterRecord(indexFile, bsr.sir_prev, sir, bsr.sih, bsr.header);
        } else if (bsr.location == -1) {
            // Move to aux file from prev
            this->insertAux(indexFile, bsr.sir_prev, sir, bsr);
        } else if (bsr.location == 0) {
            // Insert duplicate
            this->insertDuplicate(indexFile, bsr.sir, sir);
        } else if (bsr.sir.next_file == INDEXFILE) {
            // Insert between cur and next
            this->insertAfterRecord(indexFile, bsr.sir, sir, bsr.sih, bsr.header);
        } else {
            // Move to aux file from cur
            this->insertAux(indexFile, bsr.sir, sir, bsr);
        }

        if (!this->validNumberRecords()) this->rebuild();

    } catch (std::runtime_error) {
        response.stopTimer();
        indexFile.close();
        throw std::runtime_error("Couldn't add");
    }

    response.stopTimer();
    indexFile.close();
    return response;
}

void SequentialIndex::getAllRawCurrentRecords(SequentialIndexRecord sir, std::vector<physical_pos>& records){
    std::fstream duplicatesFile(this->duplicatesFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!duplicatesFile.is_open()) throw std::runtime_error("Couldn't open duplicatesFile");

    try {
        records.push_back(sir.raw_pos);
        while(sir.dup_pos != -1) {
            duplicatesFile.seekp(sir.dup_pos, std::ios::beg);
            this->readRecord(duplicatesFile, sir);
            records.push_back(sir.raw_pos);
        }
    } catch (...) {
        duplicatesFile.close();
        throw std::runtime_error("Couldn't get all raw current records");
    }
    duplicatesFile.close();
}

void SequentialIndex::searchAuxFile(Data data, BinarySearchResponse& bir, std::vector<physical_pos>& records){
    std::fstream auxFile(this->auxFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!auxFile.is_open()) throw std::runtime_error("Couldn't open auxFile");
    
    SequentialIndexRecord sir;
    try {
        if (bir.header) {
            if (bir.sih.next_file == INDEXFILE) return;            
            auxFile.seekp(bir.sih.next_pos, std::ios::beg);
        } else if (bir.location == REC_PREV){
            if (bir.sir_prev.next_file == INDEXFILE) return;
            auxFile.seekp(bir.sir_prev.next_pos, std::ios::beg);
        } else if (bir.location == REC_NEXT) {
            if (bir.sir.next_file == INDEXFILE) return;
            auxFile.seekp(bir.sir.next_pos, std::ios::beg);
        }
        this->readRecord(auxFile, sir);
        while(sir.next_file != INDEXFILE) {
            if (sir.data == data) { this->getAllRawCurrentRecords(sir, records); break;}
            auxFile.seekp(sir.next_pos, std::ios::beg);
            this->readRecord(auxFile, sir);
        }
    } catch (...) {
        auxFile.close();
        throw std::runtime_error("Couldn't search auxFile");
    }
    auxFile.close();
}

Response SequentialIndex::search(Data data){
    Response response;

    std::fstream indexFile(this->indexFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!indexFile.is_open()) throw std::runtime_error("Couldn't open indexFile");

    response.startTimer();
    try {
        BinarySearchResponse bsr = this->binarySearch(indexFile, data);
        if (bsr.location == EMPTY_FILE) { response.stopTimer(); indexFile.close(); return response; 
        } else if (bsr.location == REC_CUR) {
            this->getAllRawCurrentRecords(bsr.sir, response.records);
        } else {
            this->searchAuxFile(data, bsr, response.records);
        }
    } catch (std::runtime_error) {
        response.stopTimer();
        indexFile.close();
        throw std::runtime_error("Couldn't search");
    }

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
        throw std::runtime_error("Couldn't print indexFile");
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
        throw std::runtime_error("Couldn't print auxFile");
    }
    auxFile.close();
}

size_t SequentialIndex::numberIndexRecords(){
    std::ifstream indexFile(this->indexFilename, std::ios::in | std::ios::binary);
    if (!indexFile.is_open()) throw std::runtime_error("Couldn't open indexFile");
    indexFile.seekg(0, std::ios::end);
    size_t size = indexFile.tellg();
    indexFile.close();
    return (size - sizeof(SequentialIndexHeader)) / sizeof(SequentialIndexRecord);
}

size_t SequentialIndex::numberAuxRecords(){
    std::ifstream auxFile(this->auxFilename, std::ios::in | std::ios::binary);
    if (!auxFile.is_open()) throw std::runtime_error("Couldn't open auxFile");
    auxFile.seekg(0, std::ios::end);
    size_t size = auxFile.tellg();
    auxFile.close();
    return size / sizeof(SequentialIndexRecord);
}

bool SequentialIndex::validNumberRecords(){
    size_t indexRecords = this->numberIndexRecords();
    size_t auxRecords = this->numberAuxRecords();
    return log2(indexRecords) >= auxRecords;
}

#endif //SEQUENTIALINDEX_CPP
