#include "SequentialIndexHeader.h"
#include "SequentialIndexRecord.h"

struct BinarySearchResponse {
    int location;
    bool header;
    SequentialIndexHeader sih;
    SequentialIndexRecord sir_prev;
    SequentialIndexRecord sir;
    SequentialIndexRecord sir_next;
};