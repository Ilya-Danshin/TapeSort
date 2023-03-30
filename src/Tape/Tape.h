#ifndef TEST_TAPE_H
#define TEST_TAPE_H

#include <cstdint>
#include <string>

#include "ITape.h"

// Define strings of settings.txt file that used for tape settings
#define READ_DELAY_STR "READ_DELAY"
#define WRITE_DELAY_STR "WRITE_DELAY"
#define SHIFT_DELAY_STR "SHIFT_DELAY"
#define REWIND_DELAY_STR "REWIND_DELAY"

// TapeSettings define tape characteristics as is read, write, shift and rewind delays
class TapeSettings {
public:
    // Constructors
    TapeSettings(const std::string& settingsFileName);
    TapeSettings(int32_t read_delay, int32_t write_delay, int32_t rewind_delay, int32_t shift_delay);
    TapeSettings();

    // Copy operator
    TapeSettings& operator=(TapeSettings const& other);

    // Getters
    int32_t GetReadDelay() const;
    int32_t GetWriteDelay() const;
    int32_t GetRewindDelay() const;
    int32_t GetShiftDelay() const;

    // Destructor
    ~TapeSettings();
private:
    int32_t read_delay;
    int32_t write_delay;
    int32_t rewind_delay;
    int32_t shift_delay;
};

// Tape is a class that implement ITape and emulate work with tape
class Tape: public ITape {
public:
    // Constructor
    Tape(const std::string& inputFileName, TapeSettings& settings);

    // Methods to work with tape
    int32_t Read() override;
    void Write(int32_t n) override;
    void ShiftLeft() override;
    void ShiftRight() override;
    void Rewind() override;

    // Some getters
    std::string GetFileName() const;
    int64_t GetN() const override;
    int64_t GetPosition() const override;

    // Override destructor
    ~Tape() override;
private:

    TapeSettings settings;
    std::string file;

    // Calculate N when calling constructor
    int64_t CalculateN(const std::string& inputFileName) const;

    // Methods for writing
    void ChangeNumber(int32_t n) const;
    void Append(int32_t n);

    // Copying prohibited
    Tape() = default;
};


#endif //TEST_TAPE_H
