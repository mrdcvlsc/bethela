#ifndef BETHELA_CONSTANTS_HPP
#define BETHELA_CONSTANTS_HPP

#include <iostream>
#include <vector>
#include <array>

// bethela constants and type namespace
namespace bconst {
    const static int MIN = 0;
    const static int MAX = 255;
    const static int CHARSETS = 256;

    typedef unsigned char byte;
    typedef std::vector<unsigned char> bytestream;

    const static std::string extension = ".bthl";
    const static std::vector<byte> FILESIGNATURE({'B', 'E', 'T', 'H', 'E', 'L', 'A'});
    constexpr static std::array<unsigned char, 7> FileSignature = {'B', 'E', 'T', 'H', 'E', 'L', 'A'};
} // namespace bconst

#endif