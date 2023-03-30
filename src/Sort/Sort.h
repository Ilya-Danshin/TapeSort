#ifndef TEST_SORT_H
#define TEST_SORT_H

#include <string>
#include <vector>
#include <filesystem>

#include "ISort.h"
#include "../Tape/ITape.h"

#define TMP_PATH "../../src/tmp/"
#define SETTINGS_PATH "../../src/settings.txt"

// Sort implement interface ISort
class Sort: public ISort {
public:
    // Constructor
    Sort(ITape* tape, const std::string& out_file_name, int64_t M);

    // Override start method
    void Start() override;

    ~Sort() override;

private:
    std::string output_file_name;

    // Getter
    std::string GetOutFileName() const;

    // First step of sorting
    void SortToTempFiles();
    std::vector<int32_t>* ReadMValues() const;
    ITape* CreateTempTape(int64_t temp_folder, int64_t number) const;
    ITape* CreateOutputTape() const;
    ITape* CreateTape(std::string file_path, std::string setting) const;
    void WriteVectorToTape(ITape* tape, std::vector<int32_t>* vector) const;

    // Second step of sorting
    void MergeTempFiles();
    ITape* OpenTempTape(std::filesystem::directory_entry& file) const;
    void MergeFiles(ITape* merge_tape, ITape* tape1, ITape* tape2) const;
    bool IsLastMerged(int64_t i) const;
    std::filesystem::directory_iterator* GetTmpDirectoryIterator(int64_t i) const;
    void CopyOddTmpFile(int64_t i, std::filesystem::directory_entry& file) const;
    void CopyResultToOutputTape(int64_t last_tmp_folder) const;

};


#endif //TEST_SORT_H
