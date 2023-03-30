#ifndef TEST_TAPE_H
#define TEST_TAPE_H

#include <cstdint>
#include <string>

#include "ITape.h"

#define READ_DELAY_STR "READ_DELAY"
#define WRITE_DELAY_STR "WRITE_DELAY"
#define SHIFT_DELAY_STR "SHIFT_DELAY"
#define REWIND_DELAY_STR "REWIND_DELAY"

class TapeSettings {
public:
    TapeSettings(std::string& settingsFileName);
    TapeSettings(int32_t read_delay, int32_t write_delay, int32_t rewind_delay, int32_t shift_delay);
    TapeSettings();

    TapeSettings& operator=(TapeSettings const& other);

    int32_t GetReadDelay() const;
    int32_t GetWriteDelay() const;
    int32_t GetRewindDelay() const;
    int32_t GetShiftDelay() const;

    ~TapeSettings();
private:
    int32_t read_delay;
    int32_t write_delay;
    int32_t rewind_delay;
    int32_t shift_delay;
};

class Tape: public ITape {
public:
    Tape(std::string& inputFileName, TapeSettings& settings);

    int32_t Read() override;
    void Write(int32_t n) override;
    void ShiftLeft() override;
    void ShiftRight() override;
    void Rewind() override;
    
    std::string GetFileName();
    int64_t GetN() const override;
    int64_t GetPosition() const override;

    ~Tape() override;
private:

    TapeSettings settings;
    std::string file;

    int64_t CalculateN(std::string& inputFileName) const;
    void ChangeNumber(int32_t n);
    void Append(int32_t n);

    Tape() = default;
};


#endif //TEST_TAPE_H
