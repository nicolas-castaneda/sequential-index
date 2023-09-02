#ifndef SEQUENTIALINDEXHEADER_H
#define SEQUENTIALINDEXHEADER_H

#include "utils.h"

class SequentialIndexHeader {
    physical_pos next_pos = -1;
    file_pos next_file = 'F';

public:
    SequentialIndexHeader(){};
    SequentialIndexHeader(physical_pos _next_post, file_pos _next_file){
        this->next_pos = _next_post;
        this->next_file = _next_file;
    }

    friend std::ostream& operator<<(std::ostream& stream, const SequentialIndexHeader& sih);
};

inline std::ostream& operator<<(std::ostream& stream, const SequentialIndexHeader& sih) {
        stream<<" | next_pos: "<<sih.next_pos<<" | next_file: "<<sih.next_file;
        return stream;
}

#endif //SEQUENTIALINDEXHEADER_H