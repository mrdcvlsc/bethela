#include <iostream>
#include "../constants.hpp"
#include "../byteio.hpp"

#define TOTAL_TEST 9
#define NUMBER_INDEX 4

bool ISEQUAL(const std::string& FileA, const std::string& FileB)
{
    bconst::bytestream A = byteio::file_read(FileA);
    bconst::bytestream B = byteio::file_read(FileB);

    return A == B;
}

int main()
{
    std::string RandFilenameTemplate = "fileN.subject";
    std::string RandVFilenameTemplate = "fileN.validator";

    size_t tests = 0;

    for(size_t i=0; i<TOTAL_TEST; ++i)
    {
        RandFilenameTemplate[NUMBER_INDEX] = RandVFilenameTemplate[NUMBER_INDEX] = ('0'+(char)i);
        bool eq = ISEQUAL(RandFilenameTemplate,RandVFilenameTemplate);
        if(eq)
            std::cout << "\t\ttest " << i << " - success\n";
        else
            std::cout << "\t\ttest " << i << " - failed\n";

        tests += eq;
    }

    std::cout << "\tTotal Test Passed : " << tests << "\n";
    
    if(tests==TOTAL_TEST)
        std::cout << "[PASSED]\n";
    else
    {
        std::cout << "[FAILED]\n";
        throw std::logic_error("Some/All test failed");
    }

    return 0;
}