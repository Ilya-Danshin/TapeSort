#include "Tape.h"

#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

// TapeSettings constructor with settings from file
TapeSettings::TapeSettings(const std::string& settingsFileName) {
    // Try to open file
    std::ifstream file;
    file.open(settingsFileName);
    if (file.is_open()) {
        std::string line;

        TapeSettings settings;

        // Read all lines in file and save it to settings
        while (std::getline(file, line)) {
            if (!line.compare(0, 10, READ_DELAY_STR)) {
                settings.read_delay = stoi(line.substr(11));
            } else if(!line.compare(0, 11, WRITE_DELAY_STR)) {
                settings.write_delay = stoi(line.substr(12));
            } else if(!line.compare(0, 12, REWIND_DELAY_STR)) {
                settings.rewind_delay = stoi(line.substr(13));
            } else if(!line.compare(0, 11, SHIFT_DELAY_STR)) {
                settings.shift_delay = stoi(line.substr(12));
            }
        }

        *this = settings;
        file.close();
        return;
    }

    // If we can't open file then use empty constructor
    *this = TapeSettings();
}

TapeSettings::TapeSettings(int32_t read_delay, int32_t write_delay, int32_t rewind_delay, int32_t shift_delay) {
    this->read_delay = read_delay;
    this->write_delay = write_delay;
    this->rewind_delay = rewind_delay;
    this->shift_delay = shift_delay;
}

int32_t TapeSettings::GetReadDelay() const {
    return this->read_delay;
}

int32_t TapeSettings::GetWriteDelay() const {
    return this->write_delay;
}

int32_t TapeSettings::GetRewindDelay() const {
    return this->rewind_delay;
}

int32_t TapeSettings::GetShiftDelay() const {
    return this->shift_delay;
}

TapeSettings &TapeSettings::operator=(TapeSettings const &other) = default;

TapeSettings::TapeSettings()
{
    this->read_delay = 0;
    this->write_delay = 0;
    this->shift_delay = 0;
    this->rewind_delay = 0;
};

TapeSettings::~TapeSettings() = default;

// Tape constructor
Tape::Tape(const std::string& inputFileName, TapeSettings& settings) {
    // Copy fields
    this->position = 0;
    this->file = inputFileName;
    this->settings = settings;

    // Calculate N
    this->N = CalculateN(inputFileName);
}

int64_t Tape::CalculateN(const std::string& inputFileName) const {
    std::ifstream tape_file;
    tape_file.open(inputFileName);
    if (tape_file.is_open()) {
        int32_t value;
        int64_t i = 0;

        // If file is empty N=0
        if(tape_file.peek() == std::ifstream::traits_type::eof()) {
            tape_file.close();
            return 0;
        }

        // Just count all numbers in file
        do {
            tape_file >> value;
            i++;
        } while(!tape_file.eof());

        tape_file.close();
        return i;
    }

    return 0;
}

void Tape::Write(int32_t n) {
    // Wait delay
    std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetWriteDelay()));

    // If current position == N then we in the end of the tape, so we should use append method
    if (this->GetPosition() < this->GetN()) {
        ChangeNumber(n);
    } else {
        Append(n);
    }
}

// Method that change number in file
void Tape::ChangeNumber(int32_t n) const {
    // Open tape file
    std::ifstream tape_file(this->GetFileName());
    if (tape_file.is_open()) {
        std::vector<int32_t> tape;
        int32_t value;
        // Read all numbers in file and push it to vector.
        // Number that is under head changed to n
        for (int i = 0; tape_file >> value; i++) {
            if (i == this->GetPosition()) {
                tape.push_back(n);
            } else {
                tape.push_back(value);
            }
        }
        tape_file.close();

        // Open file for write and write vector
        std::ofstream changed_file(this->GetFileName());
        for (auto num: tape) {
            changed_file << " " << num;
        }
        changed_file.close();
    }
}

// Method that append number to the end of the tape include case of empty tape
void Tape::Append(int32_t n) {
    std::ofstream tape_file(this->GetFileName(), std::ios::app);
    if (tape_file.is_open()) {
        // Can't start file of tape with space
        if (this->GetPosition() != 0) {
            tape_file << " " << n;
        } else {
            tape_file << n;
        }
    }
    tape_file.close();
    // Add new number, so n increment
    this->N++;
}

int32_t Tape::Read() {
    // Wait delay
    std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetReadDelay()));

    // Open tape file
    std::ifstream tape_file;
    tape_file.open(this->GetFileName());
    if (tape_file.is_open()) {
        // Skip numbers to the position of the head and return number that stay under head
        int32_t value;
        for (int i = 0; i <= this->GetPosition(); i++, tape_file >> value) {}

        tape_file.close();
        return value;
    }

    return 0;
}

void Tape::ShiftLeft() {
    // Cant shift to left if position==N
    if (this->GetPosition() < this->GetN()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetShiftDelay()));
        this->position++;
    }
}

void Tape::ShiftRight() {
    // Cant shift to right if position==0
    if (this->GetPosition() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetShiftDelay()));
        this->position--;
    }
}

void Tape::Rewind() {
    std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetRewindDelay()));

    this->position = 0;
}

int64_t Tape::GetN() const {
    return this->N;
}

int64_t Tape::GetPosition() const {
    return this->position;
}

std::string Tape::GetFileName() const {
    return this->file;
}

Tape::~Tape() = default;