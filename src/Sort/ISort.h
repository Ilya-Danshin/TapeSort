#ifndef TEST_ISORT_H
#define TEST_ISORT_H

#include <cstdint>

#include "../Tape/ITape.h"

class ISort {
public:
    virtual void Start() = 0;
protected:
    ITape* tape;
    int64_t M;
};


#endif //TEST_ISORT_H
