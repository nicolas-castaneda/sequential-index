#include "libraries.h"

struct Data {
    int numero;
};

struct Response {
    std::vector<Data> records;
    time_t query_time;
};