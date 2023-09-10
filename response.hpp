#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "utils.hpp"

struct Response {
    std::vector<physical_pos> records;
    time_t query_time;

    Response(){}

    void startTimer() {
        this->query_time = time(NULL);
    }

    void stopTimer() {
        this->query_time = time(NULL) - this->query_time;
    }
};

#endif // RESPONSE_HPP