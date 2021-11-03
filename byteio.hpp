#ifndef BETHELA_BYTEIO_HPP
#define BETHELA_BYTEIO_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>

#include "constants.hpp"

namespace byteio
{
    /// First = path, Second = filename
    std::pair<std::string,std::string> getfilepath(const std::string& fullFilePath)
    {
        size_t filenameLen = 0, fullLen = fullFilePath.size();

        for(size_t i=0; i<fullLen; ++i)
        {
            if(fullFilePath[fullLen-1-i]=='/' || fullFilePath[fullLen-1-i]=='\\') break;
            filenameLen++;
        }

        std::string filepath = fullFilePath.substr(0,fullLen-filenameLen);
        std::string filename = fullFilePath.substr(fullLen-filenameLen,filenameLen);

        return {filepath,filename};
    }

    /// returns an empty bytestream if the file cannot be opened, or non existing
    beth_const::bytestream file_read(const std::string& filename)
    {
        std::ifstream fileData(filename, std::ios::binary );
        if(!fileData.is_open())
        {
            beth_const::bytestream empty_bytestream = {};
            return empty_bytestream;
        }
        beth_const::bytestream filebytestream(std::istreambuf_iterator<char>(fileData), {});
        return filebytestream;
    }

    bool file_write(const std::string& outputfilename, const beth_const::bytestream& filebytestream)
    {
        std::ofstream fileData(outputfilename, std::ios::out | std::ios::binary);
        if(!fileData.is_open()) return false;
        fileData.write((char*)&filebytestream[0], filebytestream.size() * sizeof(beth_const::byte));
        fileData.close();
        return true;
    }
}

#endif