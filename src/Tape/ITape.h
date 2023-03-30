#ifndef TEST_ITAPE_H
#define TEST_ITAPE_H

#include <cstdint>


class ITape {
public:
    virtual int32_t Read() = 0;
    virtual void Write(int32_t n) = 0;
    virtual void ShiftLeft() = 0;
    virtual void ShiftRight() = 0;
    virtual void Rewind() = 0;

    virtual int64_t GetN() const = 0;
    virtual int64_t GetPosition() const = 0;

    virtual ~ITape() {};

protected:
    int64_t N;
    int64_t position;
};


#endif //TEST_ITAPE_H
