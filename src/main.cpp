#include "Tape/ITape.h"
#include "Tape/Tape.h"

#include "Sort/ISort.h"
#include "Sort/Sort.h"

void DeleteDirectoryContents(const std::string &dir_path) {
    for (const auto& entry : std::filesystem::directory_iterator(dir_path))
        std::filesystem::remove_all(entry.path());
}

int main(int argc, char** argv) {
    std::string setting("../../src/settings.txt");
    TapeSettings settings(setting);
    std::string inpFile;
    std::string outFile;

    DeleteDirectoryContents("../../src/tmp");

    if (argc > 1) {
        inpFile = std::string(argv[1]);
        outFile = std::string(argv[2]);
    } else {
        inpFile = std::string("../../src/input.txt");
        outFile = std::string("../../src/output.txt");
    }

    auto tape = (ITape*) new Tape(inpFile, settings);

    auto sort = (ISort*) new Sort(tape, outFile, 9);
    sort->Start();

    return 0;
}
