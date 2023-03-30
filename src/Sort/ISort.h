#ifndef TEST_ISORT_H
#define TEST_ISORT_H

#include <cstdint>

#include "../Tape/ITape.h"

// Interface for work with sort
class ISort {
public:
    // Should have method that start sorting
    virtual void Start() = 0;

    virtual ~ISort() { };
protected:
    // Tape that we need to sort
    ITape* tape;
    // Max volume of memory
    int64_t M;
};


#endif //TEST_ISORT_H
