#include "SequentialIndexHeader.h"
#include "SequentialIndexRecord.h"

enum status {
    EMPTY_FILE = -2,
    REC_PREV = -1,
    REC_CUR = 0,
    REC_NEXT = 1
};

struct BinarySearchResponse {
    status location;
    bool header;

    SequentialIndexHeader sih;

    SequentialIndexRecord sir_prev;
    SequentialIndexRecord sir;
    SequentialIndexRecord sir_next;

    BinarySearchResponse() {
        this->location = EMPTY_FILE;
        this->header = false;
    }

    void setHeader(SequentialIndexHeader sih) {
        this->sih = sih;
        this->header = true;
    }

    void setRecords(SequentialIndexRecord sir_prev, SequentialIndexRecord sir, SequentialIndexRecord sir_next) {
        this->sir_prev = sir_prev;
        this->sir = sir;
        this->sir_next = sir_next;
    }

    friend std::ostream& operator<<(std::ostream& stream, const BinarySearchResponse& bsr);
};

inline std::ostream& operator<<(std::ostream& stream, const BinarySearchResponse& bsr){
    stream<<"BinarySearchResponse: \n";
    stream<<"location: "<<bsr.location<<"\n";
    stream<<"header_bool: "<<bsr.header<<"\n";
    stream<<"header_rec: "<<bsr.sih<<"\n";
    stream<<"sir_prev: "<<bsr.sir_prev<<"\n";
    stream<<"sir: "<<bsr.sir<<"\n";
    stream<<"sir_next: "<<bsr.sir_next<<"\n";
    
    return stream;
}