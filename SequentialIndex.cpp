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
    /* std::cout<<"REBUILD INDEX FILE"<<std::endl; // TODO: remove this "debug"
    this->printIndexFile();
    std::cout<<"REBUILD AUX FILE"<<std::endl; // TODO: remove this "debug"
    this->printAuxFile();
    std::cout<<"REBUILD DUPLICATES FILE"<<std::endl; // TODO: remove this "debug"
    this->printDuplicatesFile();  */

    std::fstream indexFile(this->indexFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!indexFile.is_open()) throw std::runtime_error("Couldn't open indexFile");

    std::fstream auxFile(this->auxFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!auxFile.is_open()) throw std::runtime_error("Couldn't open auxFile");

    std::fstream duplicatesFile(this->duplicatesFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!duplicatesFile.is_open()) throw std::runtime_error("Couldn't open duplicatesFile");

    std::ofstream newIndexFile("new_" + this->indexFilename, std::ios::app | std::ios::binary);
    if (!newIndexFile.is_open()) throw std::runtime_error("Couldn't create newIndexFile");

    std::ofstream newAuxFile("new_" + this->auxFilename, std::ios::app | std::ios::binary);
    if (!newAuxFile.is_open()) throw std::runtime_error("Couldn't create newAuxFile");

    std::ofstream newDuplicatesFile("new_" + this->duplicatesFilename, std::ios::app | std::ios::binary);
    if (!newDuplicatesFile.is_open()) throw std::runtime_error("Couldn't create newDuplicatesFile");

    SequentialIndexHeader sih;
    SequentialIndexRecord sir;
    SequentialIndexRecord sir_dup;

    try {
        readHeader(indexFile, sih);
        if (sih.next_pos != -1) {
            writeHeader(newIndexFile, sizeof(SequentialIndexHeader), INDEXFILE);
            if (sih.next_file == INDEXFILE) {
                this->moveReadRecord(indexFile, sih.next_pos, sir);
            } else {
                this->moveReadRecord(auxFile, sih.next_pos, sir);
            }

            physical_pos next_move;
            physical_pos next_dup;
            file_pos next_file;

            while(true){
                if (sir.dup_pos != -1) {
                    physical_pos current_dup_pos = newDuplicatesFile.tellp();
                    next_dup = sir.dup_pos;
                    sir.setDupPos(current_dup_pos);
                    this->moveReadRecord(duplicatesFile, next_dup, sir_dup);
                    
                    while (true) {
                        physical_pos current_dup_pos = newDuplicatesFile.tellp();
                        sir_dup.setCurrent(current_dup_pos, DUPFILE);
                        next_dup = sir_dup.dup_pos;
                        if (next_dup == -1) {
                            sir_dup.setDupPos(-1);
                        } else {
                            sir_dup.setDupPos(current_dup_pos + sizeof(SequentialIndexRecord));
                        }
                        this->writeRecord(newDuplicatesFile, sir_dup);
                        if (next_dup == -1) {break;}
                        this->moveReadRecord(duplicatesFile, next_dup, sir_dup);
                    }
                }  
                physical_pos current_pos = newIndexFile.tellp();
                sir.setCurrent(current_pos, INDEXFILE);
                next_move = sir.next_pos;
                next_file = sir.next_file;
                if (next_move == -1) {
                    sir.setNext(-1, INDEXFILE);
                } else {
                    sir.setNext(current_pos + sizeof(SequentialIndexRecord), INDEXFILE);
                }
                this->writeRecord(newIndexFile, sir);
                if (next_move == -1) {break;}
                if (next_file == INDEXFILE) {
                    this->moveReadRecord(indexFile, next_move, sir);
                } else {
                    this->moveReadRecord(auxFile, next_move, sir);
                }
            }
        } else {
            writeHeader(newIndexFile, -1, INDEXFILE);
        }
        
    } catch (...) {
        indexFile.close();
        auxFile.close();
        duplicatesFile.close();
        newIndexFile.close();
        newDuplicatesFile.close();
        throw std::runtime_error("Couldn't rebuild");
    }
    
    indexFile.close();
    auxFile.close();
    duplicatesFile.close();
    newIndexFile.close();
    newAuxFile.close();
    newDuplicatesFile.close();

    // truncate old files and replace new files with old names
    try {
        std::remove(this->indexFilename.c_str());
        std::rename(("new_" + this->indexFilename).c_str(), this->indexFilename.c_str());
        std::remove(this->auxFilename.c_str());
        std::rename(("new_" + this->auxFilename).c_str(), this->auxFilename.c_str());
        std::remove(this->duplicatesFilename.c_str());
        std::rename(("new_" + this->duplicatesFilename).c_str(), this->duplicatesFilename.c_str());
    } catch (...) {
        throw std::runtime_error("Couldn't rename files");
    }

   /*  std::cout<<"AFTER INDEX FILE"<<std::endl; // TODO: remove this "debug"
    this->printIndexFile(); */
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

        

    } catch (std::runtime_error) {
        response.stopTimer();
        indexFile.close();
        throw std::runtime_error("Couldn't add");
    }

    response.records.push_back(sir.raw_pos);
    response.stopTimer();
    indexFile.close();

    if (!this->validNumberRecords())    this->rebuild();
    
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

void SequentialIndex::searchAuxFile(Data data, BinarySearchResponse& bir, std::vector<physical_pos>& records, SequentialIndexRecord& sir){
    std::fstream auxFile(this->auxFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!auxFile.is_open()) throw std::runtime_error("Couldn't open auxFile");
     
    try {
        if (bir.header) {
            sir = bir.sir;
            if (bir.sih.next_file == INDEXFILE) return;            
            auxFile.seekp(bir.sih.next_pos, std::ios::beg);
        } else if (bir.location == REC_PREV){
            sir = bir.sir_prev;
            if (bir.sir_prev.next_file == INDEXFILE) return;
            auxFile.seekp(bir.sir_prev.next_pos, std::ios::beg);
        } else if (bir.location == REC_NEXT) {
            sir = bir.sir;
            if (bir.sir.next_file == INDEXFILE) return;
            auxFile.seekp(bir.sir.next_pos, std::ios::beg);
        }
        this->readRecord(auxFile, sir);
        while(data >= sir.data) {
            if (sir.data == data) { this->getAllRawCurrentRecords(sir, records); break;}
            if (sir.next_file != INDEXFILE) { break; }
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
        SequentialIndexRecord sir;
        if (bsr.location == EMPTY_FILE) { response.stopTimer(); indexFile.close(); return response; 
        } else if (bsr.location == REC_CUR) {
            this->getAllRawCurrentRecords(bsr.sir, response.records);
        } else {
            this->searchAuxFile(data, bsr, response.records, sir);
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

Response SequentialIndex::rangeSearch(Data begin, Data end) {
    Response response;
    

    std::fstream indexFile(this->indexFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!indexFile.is_open()) throw std::runtime_error("Couldn't open indexFile");

    response.startTimer();
    if (begin > end) { response.stopTimer(); indexFile.close(); return response;}
    try {
        BinarySearchResponse bsr = this->binarySearch(indexFile, begin);
        SequentialIndexRecord sir;
        if (bsr.location == EMPTY_FILE) { response.stopTimer(); indexFile.close(); return response; 
        } else if (bsr.location == REC_CUR) {
            sir = bsr.sir;
            this->getAllRawCurrentRecords(sir, response.records);
        } else {
            this->searchAuxFile(begin, bsr, response.records, sir);
        }
        if (sir.data <= begin) {
            // Move to the next record
            if (sir.next_file == INDEXFILE) {
                if (sir.next_pos == -1) { response.stopTimer(); indexFile.close(); return response; }
                moveReadRecord(indexFile, sir.next_pos, sir);
            } else {
                std::fstream auxFile(this->auxFilename, std::ios::in | std::ios::out | std::ios::binary);
                if (!auxFile.is_open()) throw std::runtime_error("Couldn't open auxFile");
                auxFile.seekp(sir.next_pos, std::ios::beg);
                this->readRecord(auxFile, sir);
                auxFile.close();
            }
        } 
        std::fstream auxFile(this->auxFilename, std::ios::in | std::ios::out | std::ios::binary);
        if (!auxFile.is_open()) throw std::runtime_error("Couldn't open auxFile");
        while(sir.data <= end) {
            this->getAllRawCurrentRecords(sir, response.records);
            if (sir.next_pos == -1) { break; }
            if (sir.next_file == INDEXFILE) {
                moveReadRecord(indexFile, sir.next_pos, sir);
            } else {
                moveReadRecord(auxFile, sir.next_pos, sir);
            }
        }
        auxFile.close();
    } catch (std::runtime_error) {
        response.stopTimer();
        indexFile.close();
        throw std::runtime_error("Couldn't search");
    }

    response.stopTimer();
    indexFile.close();
    return response;
}

Response SequentialIndex::erase(Data data, Response& response) {

    std::fstream indexFile(this->indexFilename, std::ios::in | std::ios::out | std::ios::binary);
    if (!indexFile.is_open()) throw std::runtime_error("Couldn't open indexFile");

    response.startTimer();
    try {
        BinarySearchResponse bsr = this->binarySearch(indexFile, data);
        if (bsr.location == EMPTY_FILE) { response.stopTimer(); indexFile.close(); return response; 
        } else if (bsr.location == REC_CUR) {
            if (bsr.sir.current_pos == sizeof(SequentialIndexHeader)) {
                SequentialIndexHeader sih;
                this->readHeader(indexFile, sih);
                if (sih.next_file == INDEXFILE) {
                    sih.next_file = bsr.sir.next_file;
                    sih.next_pos = bsr.sir.next_pos;

                    bsr.sir.next_file = -1;
                    bsr.sir.next_pos = -1;

                    this->writeHeader(indexFile, sih.next_pos, sih.next_file);
                    this->moveWriteRecord(indexFile, bsr.sir.current_pos, bsr.sir);
                }
                response.records.push_back(bsr.sir.raw_pos);
                response.stopTimer(); 
                indexFile.close();
                return response;
            } else if (bsr.sir_prev.next_file == INDEXFILE) {
                bsr.sir_prev.setNext(bsr.sir.next_pos, bsr.sir.next_file);
                bsr.sir.setNext(-1, INDEXFILE);

                this->moveWriteRecord(indexFile, bsr.sir_prev.current_pos, bsr.sir_prev);
                this->moveWriteRecord(indexFile, bsr.sir.current_pos, bsr.sir);

                response.records.push_back(bsr.sir.raw_pos);
                response.stopTimer();
                indexFile.close();
                return response;
            }
        } else {
            std::fstream auxFile(this->auxFilename, std::ios::in | std::ios::out | std::ios::binary);
            SequentialIndexRecord sir_cur;
            SequentialIndexRecord sir_prev;
            // Record to delete is not in files
            if (
                (bsr.header && bsr.sih.next_file == INDEXFILE) || 
                (bsr.location == REC_PREV && bsr.sir_prev.next_file == INDEXFILE) ||
                (bsr.location == REC_NEXT && bsr.sir.next_file == INDEXFILE) ||
                (bsr.header && bsr.sih.next_pos == -1) ||
                (bsr.location == REC_PREV && bsr.sir_prev.next_pos == -1) ||
                (bsr.location == REC_NEXT && bsr.sir.next_pos == -1)
            ) {
                response.stopTimer();
                indexFile.close();
                auxFile.close();
                return response;
            }
            if (bsr.header) {

                this->moveReadRecord(indexFile, bsr.sih.next_pos, sir_cur);
                if (sir_cur.data == data) {
                    bsr.sih.next_pos = sir_cur.next_pos;
                    bsr.sih.next_file = sir_cur.next_file;
                    sir_cur.setNext(-1, INDEXFILE);
                    this->writeHeader(indexFile, bsr.sih.next_pos, bsr.sih.next_file);
                    this->moveWriteRecord(indexFile, sir_cur.current_pos, sir_cur);
                    auxFile.close();
                    response.stopTimer();
                    indexFile.close();
                    return response;
                } else {
                    if (sir_cur.next_file == INDEXFILE || sir_cur.next_pos == -1 || sir_cur.data > data) {
                        response.stopTimer();
                        indexFile.close();
                        auxFile.close();
                        return response;
                    } else {
                        sir_prev = sir_cur;
                        this->moveReadRecord(auxFile, sir_cur.next_pos, sir_cur);
                    }
                }
                
            } else if (bsr.location == REC_PREV) {
                sir_prev = bsr.sir_prev;
                this->moveReadRecord(auxFile, sir_prev.next_pos, sir_cur);
            } else if (bsr.location == REC_NEXT) {
                                                std::cout<<"ACA"<<std::endl;
                sir_prev = bsr.sir;
                this->moveReadRecord(auxFile, sir_cur.next_pos, sir_cur);
            }
            std::cout<<"SIR_PREV: "<<sir_prev<<std::endl;
            std::cout<<"SIR_CUR: "<<sir_cur<<std::endl;
            while(true) {
                if (sir_cur.data == data) {
                    sir_prev.setNext(sir_cur.next_pos, sir_cur.next_file);
                    sir_cur.setNext(-1, INDEXFILE);

                    if (sir_prev.current_file == INDEXFILE) {
                        this->moveWriteRecord(indexFile, sir_prev.current_pos, sir_prev);
                    } else {
                        this->moveWriteRecord(auxFile, sir_prev.current_pos, sir_prev);
                    }
                    this->moveWriteRecord(auxFile, sir_cur.current_pos, sir_cur);
                    break;
                }
                if (sir_cur.next_file == INDEXFILE || sir_cur.next_pos == -1 || sir_cur.data > data) { 
                    response.stopTimer();
                    indexFile.close();
                    auxFile.close();
                    return response;
                }
                sir_prev = sir_cur;
                this->moveReadRecord(auxFile, sir_cur.next_pos, sir_cur);

            }
            response.records.push_back(sir_cur.raw_pos);
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


Response SequentialIndex::erase(Data data) {
    Response response;
    this->erase(data, response);
    if (response.records.size() != 0) {
        this->rebuild();
    }
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

void SequentialIndex::printDuplicatesFile() {
    std::fstream duplicatesFile(this->duplicatesFilename, std::ios::in | std::ios::out | std::ios::binary);
    SequentialIndexRecord sir;
    try {
        while(duplicatesFile.peek() != EOF) {
            this->readRecord(duplicatesFile, sir);
            std::cout<<sir<<std::endl;
        }
    } catch (std::runtime_error) {
        duplicatesFile.close();
        throw std::runtime_error("Couldn't print duplicatesFile");
    }
    duplicatesFile.close();
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
