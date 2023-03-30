#include "Tape/ITape.h"
#include "Tape/Tape.h"

#include "Sort/ISort.h"
#include "Sort/Sort.h"

void DeleteDirectoryContents(const std::string &dir_path) {
    for (const auto& entry : std::filesystem::directory_iterator(dir_path))
        std::filesystem::remove_all(entry.path());
}

int main(int argc, char** argv) {
    TapeSettings settings("../../src/settings.txt");
    const char* inpFile;
    const char* outFile;

    DeleteDirectoryContents("../../src/tmp");

    if (argc > 1) {
        inpFile = argv[1];
        outFile = argv[2];
    } else {
        inpFile = "../../src/input.txt";
        outFile = "../../src/output.txt";
    }

    auto tape = (ITape*) new Tape(inpFile, settings);

    auto sort = (ISort*) new Sort(tape, outFile, 9);
    sort->Start();

    delete tape;
    delete sort;

    return 0;
}
