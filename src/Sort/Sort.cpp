#include <algorithm>

#include "Sort.h"
#include "../Tape/Tape.h"

Sort::Sort(ITape *tape, const std::string& out_file_name, int64_t M) {
    this->tape = tape;
    this->output_file_name = out_file_name;
    this->M = M;
}

void Sort::Start() {
    SortToTempFiles();
    MergeTempFiles();
}

std::vector<int32_t>* Sort::ReadMValues() const {
    auto values = new std::vector<int32_t>;

    // Read M numbers from tape or read while head not in the end of tape
    for (int64_t i = 0; i < this->M && this->tape->GetPosition() < this->tape->GetN(); i++) {
        values->push_back(this->tape->Read());
        this->tape->ShiftLeft();
    }

    return values;
}

// Create tape method
ITape *Sort::CreateTape(std::string file_path, std::string setting) const {
    TapeSettings settings(setting);
    ITape* tempTape = new Tape(file_path, settings);

    return tempTape;
}

// Create temp tape
// temp_folder - number of temp folder (0, 1, 2, ...)
// number - number of file
ITape *Sort::CreateTempTape(int64_t temp_folder, int64_t number) const {
    // Construct path to file
    std::string tmp(TMP_PATH);
    tmp += std::to_string(temp_folder);
    std::filesystem::create_directories(tmp);

    tmp += "/";
    tmp += std::to_string(number);
    tmp += std::string(".txt");

    return this->CreateTape(tmp, SETTINGS_PATH);
}

// Create output tape
ITape *Sort::CreateOutputTape() const {
    return this->CreateTape(this->GetOutFileName(), SETTINGS_PATH);
}

// Write numbers to tape
void Sort::WriteVectorToTape(ITape *tape, std::vector<int32_t>* vector) const {
    for (auto n: *vector) {
        tape->Write(n);
        tape->ShiftLeft();
    }
    tape->Rewind();
}

// First step of sorting
// 1. Take M values from tape
// 2. Sort it
// 3. Write to temporary file in folder /tmp/0/
void Sort::SortToTempFiles() {
    for (int64_t i = 0; i*this->M < this->tape->GetN(); i++) {
        auto values = ReadMValues();
        std::sort(values->begin(), values->end());

        auto tempTape = CreateTempTape(0, i);
        WriteVectorToTape(tempTape, values);

        delete tempTape;
        delete values;
    }
}

// Second step of sorting
// After first step we have folder /tmp/0/ witch store (N/M+1) files that contain sorted sequences
// We can get two sorted files, merge it and save new sorted file at folder /tmp/1/
// If 1 file have no pair, it just copy to folder for next round
// Repeat it while until 1 file remains
// btw it's just adaptation of merge sort
void Sort::MergeTempFiles() {
    // i - number of round
    for (int64_t i = 0; true; i++) {
        // If current temp folder have 1 file this is the end of sorting
        if (this->IsLastMerged(i)) {
            CopyResultToOutputTape(i);
            break;
        }

        // Get iterator for work with files
        auto dir_iter = Sort::GetTmpDirectoryIterator(i);

        int32_t counter = 0;
        // Going through all files in folder
        while(*dir_iter != end(*dir_iter)) {
            // Try to get two files and merge it
            auto f1 = **dir_iter;
            (*dir_iter)++;
            if (*dir_iter != end(*dir_iter)) {
                auto f2 = **dir_iter;
                (*dir_iter)++;

                // Open tapes for merge
                auto tmp1 = OpenTempTape(f1);
                auto tmp2 = OpenTempTape(f2);

                // Create merged tape
                auto merged = CreateTempTape(i+1, counter);

                MergeFiles(merged, tmp1, tmp2);

                delete tmp1;
                delete tmp2;
                delete merged;

                counter++;
            } else {
                // If there was only 1 file, copy it to folder for next round
                CopyOddTmpFile(i, f1);
            }
        }

        delete dir_iter;
    }
}

// Check count of files in folder
bool Sort::IsLastMerged(int64_t i) const {
    auto dir_iter = Sort::GetTmpDirectoryIterator(i);

    int fileCount = std::count_if(
            begin(*dir_iter),
            end(*dir_iter),
            [](auto& entry) { return entry.is_regular_file(); }
    );

    delete dir_iter;

    if (fileCount == 1) {
        return true;
    }

    return false;
}

// Copy result tape to output tape
void Sort::CopyResultToOutputTape(int64_t last_tmp_folder) const{
    auto dir_iter = Sort::GetTmpDirectoryIterator(last_tmp_folder);
    auto merged = **dir_iter;
    auto merged_tape = OpenTempTape(merged);

    auto out = CreateOutputTape();

    merged_tape->Rewind();

    while(merged_tape->GetPosition() < merged_tape->GetN()) {
        int32_t n = merged_tape->Read();
        merged_tape->ShiftLeft();

        out->Write(n);
        out->ShiftLeft();
    }

    delete dir_iter;
    delete merged_tape;
    delete out;
}

// Open existing tape file
ITape *Sort::OpenTempTape(std::filesystem::directory_entry& file) const {
    TapeSettings settings(SETTINGS_PATH);

#ifdef __MINGW64__
    // This work on MinGW
    auto wpath = std::wstring(file.path().c_str());
#elif __GNUC__
    // This work on GCC
    std::wstring wpath( file.path().string().length(), L'#' );
    mbstowcs(&wpath[0], file.path().c_str(), file.path().string().length());
#endif

    std::string path(wpath.begin(), wpath.end());

    return (ITape*) new Tape(path, settings);
}

std::filesystem::directory_iterator* Sort::GetTmpDirectoryIterator(int64_t i) const {
    auto dir = std::string(TMP_PATH);
    dir += std::to_string(i);
    auto dir_iter = new std::filesystem::directory_iterator(dir);

    return dir_iter;
}

void Sort::CopyOddTmpFile(int64_t i, std::filesystem::directory_entry& file) const {
    std::string tmp(TMP_PATH);
    tmp += std::to_string(i+1);
    tmp += "/";
    tmp += file.path().filename().string();

    std::filesystem::copy(file, tmp);
}

// Merge files method
// tape1 and tape2 are sorted
// 1. Get first number from tape1 and tape2
// 2. Add to merged tape lesser number, get next number from that tape witch has lesser number
// If we reach end of one of tape we can just copy other tape to merged tape
void Sort::MergeFiles(ITape *merged_tape, ITape *tape1, ITape *tape2) const {
    // Get first numbers
    int32_t n1 = tape1->Read();
    int32_t n2 = tape2->Read();

    while (true) {
        // While numbers in tape1 lesser than first number of tape2 copy numbers from tape1 to merged tape
        while (n1 <= n2 && tape1->GetPosition() < tape1->GetN()) {
            merged_tape->Write(n1);
            merged_tape->ShiftLeft();

            tape1->ShiftLeft();
            n1 = tape1->Read();
        }
        // If reach end of tape1 - copy tape2 to merged tape
        if (tape1->GetPosition() == tape1->GetN()) {
            while (tape2->GetPosition() < tape2->GetN()) {
                merged_tape->Write(n2);
                merged_tape->ShiftLeft();

                tape2->ShiftLeft();
                n2 = tape2->Read();
            }
            break;
        }

        // While numbers in tape2 lesser than first number of tape1 copy numbers from tape2 to merged tape
        while (n2 <= n1 && tape2->GetPosition() < tape2->GetN()) {
            merged_tape->Write(n2);
            merged_tape->ShiftLeft();

            tape2->ShiftLeft();
            n2 = tape2->Read();
        }
        // If reach end of tape2 - copy tape1 to merged tape
        if (tape2->GetPosition() == tape2->GetN()) {
            while (tape1->GetPosition() < tape1->GetN()) {
                merged_tape->Write(n1);
                merged_tape->ShiftLeft();

                tape1->ShiftLeft();
                n1 = tape1->Read();
            }
            break;
        }
    }
}

std::string Sort::GetOutFileName() const {
    return this->output_file_name;
}

Sort::~Sort() = default;



