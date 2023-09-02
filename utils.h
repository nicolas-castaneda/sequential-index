#ifndef UTILS_H
#define UTILS_H

#include "libraries.h"

const char INDEXFILE = 'f';
const char AUXFILE = 'a';

using physical_pos = long;
using file_pos = char;

struct Data {
    int numero;

    Data(){}

    Data(int _numero) {
        this->numero = _numero;
    }
    friend std::ostream& operator<<(std::ostream& stream, const Data& sir);
};

inline std::ostream& operator<<(std::ostream& stream, const Data& data){
    stream<<" | numero: "<<data.numero;
    return stream;
}

struct Response {
    std::vector<Data> records;
    time_t query_time;
};

#endif // UTILS_H