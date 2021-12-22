#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#include "../byteio.hpp"
#include "../constants.hpp"

#define BYTE(N) N
#define MB(N) N*1024*1024


int main()
{
    try{
        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937_64 rand_engine(seed);
        std::uniform_int_distribution<int> random_number(0,255);

        //FILE0 - 10 bytes
        bconst::bytestream RandStream0;
        for(size_t i=0; i<BYTE(10); ++i) RandStream0.push_back(random_number(rand_engine));
        byteio::file_write("file0.subject",RandStream0);
        byteio::file_write("file0.validator",RandStream0);

        //FILE1 - 16 bytes
        bconst::bytestream RandStream1;
        for(size_t i=0; i<BYTE(16); ++i) RandStream1.push_back(random_number(rand_engine));
        byteio::file_write("file1.subject",RandStream1);
        byteio::file_write("file1.validator",RandStream1);

        //FILE2 - 20 bytes
        bconst::bytestream RandStream2;
        for(size_t i=0; i<BYTE(20); ++i) RandStream2.push_back(random_number(rand_engine));
        byteio::file_write("file2.subject",RandStream2);
        byteio::file_write("file2.validator",RandStream2);

        //FILE3 - 32 bytes
        bconst::bytestream RandStream3;
        for(size_t i=0; i<BYTE(32); ++i) RandStream3.push_back(random_number(rand_engine));
        byteio::file_write("file3.subject",RandStream3);
        byteio::file_write("file3.validator",RandStream3);

        //FILE4 - 50 bytes
        bconst::bytestream RandStream4;
        for(size_t i=0; i<BYTE(50); ++i) RandStream4.push_back(random_number(rand_engine));
        byteio::file_write("file4.subject",RandStream4);
        byteio::file_write("file4.validator",RandStream4);

        //FILE5 - 64 bytes
        bconst::bytestream RandStream5;
        for(size_t i=0; i<BYTE(64); ++i) RandStream5.push_back(random_number(rand_engine));
        byteio::file_write("file5.subject",RandStream5);
        byteio::file_write("file5.validator",RandStream5);

        //FILE6 - 10 mb
        bconst::bytestream RandStream6;
        for(size_t i=0; i<MB(10); ++i) RandStream6.push_back(random_number(rand_engine));
        byteio::file_write("file6.subject",RandStream6);
        byteio::file_write("file6.validator",RandStream6);

        //FILE7 - 50 mb
        bconst::bytestream RandStream7;
        for(size_t i=0; i<MB(50); ++i) RandStream7.push_back(random_number(rand_engine));
        byteio::file_write("file7.subject",RandStream7);
        byteio::file_write("file7.validator",RandStream7);

        //FILE8 - 100 mb
        bconst::bytestream RandStream8;
        for(size_t i=0; i<MB(100); ++i) RandStream8.push_back(random_number(rand_engine));
        byteio::file_write("file8.subject",RandStream8);
        byteio::file_write("file8.validator",RandStream8);
    } catch(const char* err) {
        throw err;
    }

    return 0;
}