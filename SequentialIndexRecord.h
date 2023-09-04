#ifndef SEQUENTIALINDEXRECORD_H
#define SEQUENTIALINDEXRECORD_H

#include "utils.h"

class SequentialIndexRecord {
    
public:
    Data data;

    physical_pos raw_pos;
    physical_pos dup_pos;

    physical_pos current_pos;
    file_pos current_file;

    physical_pos next_pos;
    file_pos next_file;
    
    SequentialIndexRecord(){};    
    SequentialIndexRecord(Data data, physical_pos _raw_pos, physical_pos _dup_pos, physical_pos _current_pos, file_pos _current_file, physical_pos _next_post, file_pos _next_file){
        this->data = data;
        this->raw_pos = _raw_pos, 
        this->dup_pos = _dup_pos;

        this->current_pos = _current_pos;
        this->current_file = _current_file;
        
        this->next_pos = _next_post;
        this->next_file = _next_file;
    };

    void setData(Data data) {
        this->data = data;
    }

    void setRawPos(physical_pos _raw_pos) {
        this->raw_pos = _raw_pos;
    }

    void setDupPos(physical_pos _dup_pos) {
        this->dup_pos = _dup_pos;
    }

    void setCurrent(physical_pos _current_pos, file_pos _current_file) {
        this->current_pos = _current_pos;
        this->current_file = _current_file;
    }

    void setNext(physical_pos _next_pos, file_pos _next_file) {
        this->next_pos = _next_pos;
        this->next_file = _next_file;
    }

    friend std::ostream& operator<<(std::ostream& stream, const SequentialIndexRecord& sir);
};

inline std::ostream& operator<<(std::ostream& stream, const SequentialIndexRecord& sir) {
        stream<<sir.data<<" | raw_pos: "<<sir.raw_pos<<" | dup_pos: "<<sir.dup_pos<<" | current_pos: "<<sir.current_pos<<" | current_file: "<<sir.current_file<<" | next_pos: "<<sir.next_pos<<" | next_file: "<<sir.next_file;
        return stream;
}

#endif //SEQUENTIALINDEXRECORD_H