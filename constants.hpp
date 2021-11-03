#ifndef BETHELA_CONSTANTS_HPP
#define BETHELA_CONSTANTS_HPP

#include <iostream>
#include <vector>

// bethela constants and type namespace
namespace beth_const
{
    const static int MIN = 0;
    const static int MAX = 255;

    typedef unsigned char byte;
    typedef std::vector<unsigned char> bytestream;

    const static std::string extension = ".bethela";
}

#endif