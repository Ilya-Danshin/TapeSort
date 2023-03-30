#include <algorithm>

#include "Sort.h"
#include "../Tape/Tape.h"

Sort::Sort(ITape *tape, std::string& out_file_name, int64_t M) {
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

    for (int64_t i = 0; i < this->M && this->tape->GetPosition() < this->tape->GetN(); i++) {
        values->push_back(this->tape->Read());
        this->tape->ShiftLeft();
    }

    return values;
}

ITape *Sort::CreateTape(std::string file_path, std::string setting) const {
    TapeSettings settings(setting);
    ITape* tempTape = new Tape(file_path, settings);

    return tempTape;
}

ITape *Sort::CreateTempTape(int64_t temp_folder, int64_t number) const {
    std::string setting(SETTINGS_PATH);

    std::string tmp(TMP_PATH);
    tmp += std::to_string(temp_folder);
    std::filesystem::create_directories(tmp);

    tmp += "/";
    tmp += std::to_string(number);
    tmp += std::string(".txt");

    return this->CreateTape(tmp, setting);
}

ITape *Sort::CreateOutputTape() const {
    std::string setting(SETTINGS_PATH);

    return this->CreateTape(this->GetOutFileName(), setting);
}

void Sort::WriteVectorToTape(ITape *tape, std::vector<int32_t>* vector) const {
    for (auto n: *vector) {
        tape->Write(n);
        tape->ShiftLeft();
    }
    tape->Rewind();
}

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

void Sort::MergeTempFiles() {
    for (int64_t i = 0; true; i++) {
        if (this->IsLastMerged(i)) {
            CopyResultToOutputTape(i);
            break;
        }

        auto dir_iter = Sort::GetTmpDirectoryIterator(i);

        int32_t counter = 0;
        while(*dir_iter != end(*dir_iter)) {
            auto f1 = **dir_iter;
            (*dir_iter)++;
            if (*dir_iter != end(*dir_iter)) {
                auto f2 = **dir_iter;
                (*dir_iter)++;

                auto tmp1 = OpenTempTape(f1);
                auto tmp2 = OpenTempTape(f2);

                auto merged = CreateTempTape(i+1, counter);

                MergeFiles(merged, tmp1, tmp2);

                delete tmp1;
                delete tmp2;
                delete merged;

                counter++;
            } else {
                CopyOddTmpFile(i, f1);
            }
        }

        delete dir_iter;
    }
}

ITape *Sort::OpenTempTape(std::filesystem::directory_entry& file) const {
    std::string setting(SETTINGS_PATH);
    TapeSettings settings(setting);

    auto wpath = std::wstring(file.path().c_str());
    std::string path(wpath.begin(), wpath.end());

    return (ITape*) new Tape(path, settings);
}

void Sort::MergeFiles(ITape *merged_tape, ITape *tape1, ITape *tape2) const {
    int32_t n1 = tape1->Read();
    int32_t n2 = tape2->Read();

    while (true) {
        while (n1 <= n2 && tape1->GetPosition() < tape1->GetN()) {
            merged_tape->Write(n1);
            merged_tape->ShiftLeft();

            tape1->ShiftLeft();
            n1 = tape1->Read();
        }
        if (tape1->GetPosition() == tape1->GetN()) {
            while (tape2->GetPosition() < tape2->GetN()) {
                merged_tape->Write(n2);
                merged_tape->ShiftLeft();

                tape2->ShiftLeft();
                n2 = tape2->Read();
            }
            break;
        }

        while (n2 <= n1 && tape2->GetPosition() < tape2->GetN()) {
            merged_tape->Write(n2);
            merged_tape->ShiftLeft();

            tape2->ShiftLeft();
            n2 = tape2->Read();
        }
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

void Sort::CopyResultToOutputTape(int64_t last_tmp_folder) const{
    auto dir_iter = Sort::GetTmpDirectoryIterator(last_tmp_folder);
    auto merged = **dir_iter;
    auto merged_tape = OpenTempTape(merged);

    auto out = CreateOutputTape();

    merged_tape->Rewind();
    //out->Rewind();

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

std::string Sort::GetOutFileName() const {
    return this->output_file_name;
}



