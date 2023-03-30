#include "Tape.h"

#include <fstream>
#include <chrono>
#include <thread>
#include <vector>

#include <iostream>

TapeSettings::TapeSettings(std::string& settingsFileName) {
    std::ifstream file;
    file.open(settingsFileName);
    if (file.is_open()) {
        std::string line;

        TapeSettings settings;

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

    *this = TapeSettings();
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

TapeSettings::TapeSettings(int32_t read_delay, int32_t write_delay, int32_t rewind_delay, int32_t shift_delay) {
    this->read_delay = read_delay;
    this->write_delay = write_delay;
    this->rewind_delay = rewind_delay;
    this->shift_delay = shift_delay;
}

TapeSettings::~TapeSettings() = default;

TapeSettings &TapeSettings::operator=(TapeSettings const &other) = default;

TapeSettings::TapeSettings() = default;

Tape::Tape(std::string& inputFileName, TapeSettings& settings) {
    this->position = 0;
    this->file = inputFileName;
    this->settings = settings;

    this->N = CalculateN(inputFileName);
}

void Tape::Write(int32_t n) {
    std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetWriteDelay()));

    if (this->GetPosition() < this->GetN()) {
        ChangeNumber(n);
    } else {
        Append(n);
    }
}

int32_t Tape::Read() {
    std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetReadDelay()));

    std::ifstream tape_file;
    tape_file.open(this->GetFileName());
    if (tape_file.is_open()) {
        int32_t value;
        for (int i = 0; i <= this->GetPosition(); i++, tape_file >> value) {}

        tape_file.close();
        return value;
    }

    return 0;
}

void Tape::ShiftLeft() {
    if (this->GetPosition() <= this->GetN()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetShiftDelay()));
        this->position++;
    }
}

void Tape::ShiftRight() {
    if (this->GetPosition() >= 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetShiftDelay()));
        this->position--;
    }
}

void Tape::Rewind() {
    std::this_thread::sleep_for(std::chrono::milliseconds(this->settings.GetRewindDelay()));

    this->position = 0;
}

int64_t Tape::CalculateN(std::string& inputFileName) const {
    std::ifstream tape_file;
    tape_file.open(inputFileName);
    if (tape_file.is_open()) {
        int32_t value;
        int64_t i = 0;

        do {
            tape_file >> value;
            i++;
        } while(!tape_file.eof());

        tape_file.close();
        return i;
    }

    return 0;
}

int64_t Tape::GetN() const {
    return this->N;
}

int64_t Tape::GetPosition() const {
    return this->position;
}

void Tape::ChangeNumber(int32_t n) {
    std::ifstream tape_file(this->GetFileName());
    if (tape_file.is_open()) {
        std::vector<int32_t> tape;
        int32_t value;
        for (int i = 0; tape_file >> value; i++) {
            if (i == this->GetPosition()) {
                tape.push_back(n);
            } else {
                tape.push_back(value);
            }
        }
        tape_file.close();

        std::ofstream changed_file(this->GetFileName());
        for (auto num: tape) {
            changed_file << " " << num;
        }
        changed_file.close();
    }
}

void Tape::Append(int32_t n) {
    std::ofstream tape_file(this->GetFileName(), std::ios::app);
    if (tape_file.is_open()) {
        if (this->GetPosition() != 0) {
            tape_file << " " << n;
        } else {
            tape_file << n;
        }
    }
    tape_file.close();
    this->N++;
    this->position++;
}

std::string Tape::GetFileName() {
    return this->file;
}

Tape::~Tape() = default;
