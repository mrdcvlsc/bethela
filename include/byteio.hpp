#ifndef BETHELA_BYTEIO_HPP
#define BETHELA_BYTEIO_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>

#include "constants.hpp"

namespace byteio {
    /// First = path, Second = filename
    std::pair<std::string, std::string> getfilepath(const std::string &fullFilePath) {
        size_t filenameLen = 0, fullLen = fullFilePath.size();

        for (size_t i = 0; i < fullLen; ++i) {
            if (fullFilePath[fullLen - 1 - i] == '/' || fullFilePath[fullLen - 1 - i] == '\\') {
                break;
            }
            filenameLen++;
        }

        std::string filepath = fullFilePath.substr(0, fullLen - filenameLen);
        std::string filename = fullFilePath.substr(fullLen - filenameLen, filenameLen);

        return {filepath, filename};
    }

    /// returns an empty bytestream if the file cannot be opened, or non existing
    bconst::bytestream file_read(const std::string &filename) {
        std::ifstream fileData(filename, std::ios::binary);
        if (!fileData.is_open()) {
            bconst::bytestream empty_bytestream = {};
            return empty_bytestream;
        }
        bconst::bytestream filebytestream(std::istreambuf_iterator<char>(fileData), {});
        return filebytestream;
    }

    bool file_write(const std::string &outputfilename, const bconst::bytestream &filebytestream) {
        std::ofstream fileData(outputfilename, std::ios::out | std::ios::binary);
        if (!fileData.is_open()) {
            return false;
        }
        fileData.write((char *) &filebytestream[0], filebytestream.size() * sizeof(bconst::byte));
        fileData.close();
        return true;
    }

    bool file_write(const std::string &outputfilename, bconst::byte *filebytestream, size_t length) {
        std::ofstream fileData(outputfilename, std::ios::out | std::ios::binary);
        if (!fileData.is_open())
            return false;
        fileData.write((char *) &filebytestream[0], length * sizeof(bconst::byte));
        fileData.close();
        return true;
    }
} // namespace byteio

#endif