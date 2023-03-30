#include "googletest/googletest/include/gtest/gtest.h"

#include "../src/Tape/Tape.h"
#include "../src/Sort/Sort.h"

#include <fstream>

#define TEST_SETTINGS "../../test/test_settings"
#define TEST_INPUT_FILE "../../test/test_input"
#define TEST_OUTPUT_FILE "../../test/test_output"
#define TMP_FOLDER "../../src/tmp"

using namespace std;

TEST(TapeSettingsTest, empty_init) {
    auto settings = new TapeSettings();

    ASSERT_EQ(settings->GetReadDelay(), 0);
    ASSERT_EQ(settings->GetWriteDelay(), 0);
    ASSERT_EQ(settings->GetRewindDelay(), 0);
    ASSERT_EQ(settings->GetShiftDelay(), 0);

    delete settings;
}

TEST(TapeSettingsTest, file_init) {
    std::string file(TEST_SETTINGS);
    auto settings = new TapeSettings(file);

    ASSERT_EQ(settings->GetReadDelay(), 1);
    ASSERT_EQ(settings->GetWriteDelay(), 2);
    ASSERT_EQ(settings->GetRewindDelay(), 3);
    ASSERT_EQ(settings->GetShiftDelay(), 4);

    delete settings;
}

TEST(TapeSettingsTest, param_init) {
    auto settings = new TapeSettings(1, 2, 3, 4);

    ASSERT_EQ(settings->GetReadDelay(), 1);
    ASSERT_EQ(settings->GetWriteDelay(), 2);
    ASSERT_EQ(settings->GetRewindDelay(), 3);
    ASSERT_EQ(settings->GetShiftDelay(), 4);

    delete settings;
}

TEST(TapeInitTest, init_test) {
    auto settings = new TapeSettings();
    std::string input(TEST_INPUT_FILE);
    auto test_tape = new Tape(input, *settings);

    ASSERT_STREQ(test_tape->GetFileName().c_str(), TEST_INPUT_FILE);
    ASSERT_EQ(test_tape->GetN(), 10);
    ASSERT_EQ(test_tape->GetPosition(), 0);

    delete settings;
    delete test_tape;
}

struct TapeTest : public testing::Test {
    Tape *tape;

    void SetUp() {
        auto settings = new TapeSettings();
        std::string input(TEST_INPUT_FILE);
        tape = new Tape(input, *settings);
    }

    void TearDown() {
        delete tape;
    }
};

TEST_F(TapeTest, shift_test) {
    tape->ShiftLeft();

    ASSERT_EQ(tape->GetPosition(), 1);

    tape->ShiftLeft();
    tape->ShiftLeft();

    tape->ShiftRight();

    ASSERT_EQ(tape->GetPosition(), 2);
}

TEST_F(TapeTest, shift_border_test) {
    // We cant shift out of tape, do at borders should be 0 and N
    for (int i = 0; i < 10; i++) {
        tape->ShiftLeft();
    }

    tape->ShiftLeft();

    ASSERT_EQ(tape->GetPosition(), 10);

    for (int i = 0; i < 20; i++) {
        tape->ShiftRight();
    }

    ASSERT_EQ(tape->GetPosition(), 0);
}

TEST_F(TapeTest, read_test) {
    // Just check that test_input file was read correct
    for (int i = 10; i > 0; i--) {
        ASSERT_EQ(tape->Read(), i);
        tape->ShiftLeft();
    }
}

TEST_F(TapeTest, read_border_test) {
    tape->ShiftRight();
    tape->ShiftRight();
    tape->ShiftRight();
    ASSERT_EQ(tape->Read(), 10);

    for (int i = 10; i > 0; i--) {
        ASSERT_EQ(tape->Read(), i);
        tape->ShiftLeft();
    }

    tape->ShiftLeft();
    tape->ShiftLeft();
    tape->ShiftLeft();
    ASSERT_EQ(tape->Read(), 1);
}

TEST_F(TapeTest, rewind_test) {
    tape->Rewind();
    ASSERT_EQ(tape->GetPosition(), 0);

    for(int i = 0; i < 3; i++) {
        tape->ShiftLeft();
    }
    tape->Rewind();
    ASSERT_EQ(tape->GetPosition(), 0);

    for(int i = 0; i < 30; i++) {
        tape->ShiftLeft();
    }
    tape->Rewind();
    ASSERT_EQ(tape->GetPosition(), 0);

    for(int i = 0; i < 5; i++) {
        tape->ShiftLeft();
    }
    tape->Rewind();
    ASSERT_EQ(tape->GetPosition(), 0);
}

void RefreshTestInput() {
    // test_input file content should be: 10 9 8 7 6 5 4 3 2 1

    ofstream test_file(TEST_INPUT_FILE);

    test_file << "10 9 8 7 6 5 4 3 2 1";

    test_file.close();
}

void ClearTestInput() {
    ofstream test_file(TEST_INPUT_FILE, std::ofstream::out | std::ofstream::trunc);
    test_file.close();
}

TEST_F(TapeTest, write_test) {
    // Try to rewrite everything
    for (int i = 1; i <= 10; i++) {
        tape->Write(i);
        EXPECT_EQ(tape->Read(), i);
        tape->ShiftLeft();
    }

    RefreshTestInput();
}

TEST_F(TapeTest, write_append_test) {
    // Shift to the enf of tape
    for (int i = 0; i < 10; i++) {
        tape->ShiftLeft();
    }

    //Add 5 more numbers
    for (int i = 0; i < 5; i++) {
        tape->Write(i);
        EXPECT_EQ(tape->Read(), i);
        tape->ShiftLeft();
    }

    RefreshTestInput();
}

TEST(TapeEmptyTest, empty_tape) {
    // Get empty input file
    ClearTestInput();

    auto settings = new TapeSettings();
    std::string input(TEST_INPUT_FILE);
    auto tape = new Tape(input, *settings);

    // Check that all read correctly
    EXPECT_EQ(tape->GetN(), 0);
    EXPECT_EQ(tape->GetPosition(), 0);

    // Try to write to empty file
    tape->Write(1);
    EXPECT_EQ(tape->Read(), 1);

    delete settings;
    delete tape;

    // Refresh input file for another tests
   RefreshTestInput();
}

void DeleteDirectoryContents(const std::string &dir_path) {
    for (const auto& entry : std::filesystem::directory_iterator(dir_path))
        std::filesystem::remove_all(entry.path());
}

struct SortTest : public testing::Test {
    Sort* sort;

    void SetUp() {
        // Clear tmp folder
        DeleteDirectoryContents(TMP_FOLDER);
        // Create tape
        auto settings = new TapeSettings();
        std::string input(TEST_INPUT_FILE);
        auto tape = new Tape(input, *settings);

        // Create sort
        sort = new Sort(tape, TEST_OUTPUT_FILE, 5);
    }

    void TearDown() {
        delete sort;
    }
};

TEST_F(SortTest, sort_test) {
    sort->Start();

    // Create tape for output check
    auto settings = new TapeSettings();
    std::string output(TEST_OUTPUT_FILE);
    auto out_tape = Tape(output, *settings);

    // Check that all numbers sorted
    int32_t prev = out_tape.Read();
    for (int i = 0; i < out_tape.GetN(); i++, prev = out_tape.Read(), out_tape.ShiftLeft()) {
        ASSERT_LE(prev, out_tape.Read());
    }
}

TEST(SortMTest, sort_odd_test) {
    DeleteDirectoryContents(TMP_FOLDER);
    auto settings = new TapeSettings();
    std::string input(TEST_INPUT_FILE);
    auto tape = new Tape(input, *settings);

    // Default test_input have N=10
    auto sort = new Sort(tape, TEST_OUTPUT_FILE, 4);

    sort->Start();

    // Create tape for output check
    std::string output(TEST_OUTPUT_FILE);
    auto out_tape = Tape(output, *settings);

    // Check that all numbers sorted
    int32_t prev = out_tape.Read();
    for (int i = 0; i < out_tape.GetN(); i++, prev = out_tape.Read(), out_tape.ShiftLeft()) {
        ASSERT_LE(prev, out_tape.Read());
    }
}

TEST(SortMTest, sort_big_M_test) {
    DeleteDirectoryContents(TMP_FOLDER);
    auto settings = new TapeSettings();
    std::string input(TEST_INPUT_FILE);
    auto tape = new Tape(input, *settings);

    // Default test_input have N=10
    auto sort = new Sort(tape, TEST_OUTPUT_FILE, 40);

    sort->Start();

    // Create tape for output check
    std::string output(TEST_OUTPUT_FILE);
    auto out_tape = Tape(output, *settings);

    // Check that all numbers sorted
    int32_t prev = out_tape.Read();
    for (int i = 0; i < out_tape.GetN(); i++, prev = out_tape.Read(), out_tape.ShiftLeft()) {
        ASSERT_LE(prev, out_tape.Read());
    }
}

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
