#include "../byteio.hpp"
#include "../constants.hpp"
#include <iostream>

#define TOTAL_TEST 10
#define NUMBER_INDEX 4

bool ISEQUAL(const std::string &FileA, const std::string &FileB) {
    bconst::bytestream A = byteio::file_read(FileA);
    bconst::bytestream B = byteio::file_read(FileB);

    if (A.size() != B.size()) {
        std::cout << "\n\t\tSize is not equal\n";
        return false;
    } else if (A.size() <= 0 || B.size() <= 0) {
        return false;
    } else {
        size_t not_equal = 0;
        for (size_t i = 0; i < A.size(); ++i) {
            if (A[i] != B[i]) {
                not_equal++;
            }
        }

        float equality = ((float) A.size() - (float) not_equal) / (float) A.size();
        std::cout << "\n\t\tEqual by : " << equality * 100.0f << " percent\n";

        if (not_equal) {
            return false;
        }
    }

    return true;
}

int main() {
    std::string RandFilenameTemplate = "fileN.subject";
    std::string RandVFilenameTemplate = "fileN.validator";

    size_t tests = 0;

    for (size_t i = 0; i < TOTAL_TEST; ++i) {
        RandFilenameTemplate[NUMBER_INDEX] = RandVFilenameTemplate[NUMBER_INDEX] = ('0' + (char) i);
        bool eq = ISEQUAL(RandFilenameTemplate, RandVFilenameTemplate);
        if (eq)
            std::cout << "\t\ttest " << i << " - success\n";
        else
            std::cout << "\t\ttest " << i << " - failed\n";

        tests += eq;
    }

    std::cout << "\tTotal Test Passed : " << tests << "\n";

    if (tests == TOTAL_TEST)
        std::cout << "[PASSED]\n";
    else {
        std::cout << "[FAILED]\n";
        throw std::logic_error("Some/All test failed");
    }

    return 0;
}