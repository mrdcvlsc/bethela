#ifndef BETHELA_READBINFILE_HPP
#define BETHELA_READBINFILE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>

#include "cryptalgo.hpp"

namespace filemanage
{
    const static std::string extension = ".bethela";

    typedef unsigned char byte;
    typedef std::vector<byte> bhstream;

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

    bool encryptFile(const std::string& filename, const bhstream& keystream)
    {
        // read binary into vector
        std::ifstream inputData(filename, std::ios::binary );
        if(!inputData.is_open()) return false;

        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(inputData), {});

        lock::encrypt(buffer,keystream);

        // write vector into a binary file
        std::ofstream fout(filename+extension, std::ios::out | std::ios::binary);
        fout.write((char*)&buffer[0], buffer.size() * sizeof(byte));
        fout.close();

        return true;
    }

    bool decryptFile(const std::string& filename, const bhstream& keystream)
    {
        // read binary into vector
        std::ifstream inputData(filename, std::ios::binary );
        if(!inputData.is_open()) return false;

        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(inputData), {});

        lock::decrypt(buffer,keystream);
        
        // write vector into a binary file
        std::ofstream fout(
            filename.substr(0,filename.size()-extension.size()),
            std::ios::out | std::ios::binary
        );
        fout.write((char*)&buffer[0], buffer.size() * sizeof(byte));
        fout.close();

        return true;
    }
}

#endif