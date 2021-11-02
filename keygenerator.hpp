#ifndef KEYGENERATOR_HPP
#define KEYGENERATOR_HPP

#include <iostream>
#include <vector>
#include <fstream>

namespace keygen
{
    template<typename T>
    void write(const std::string& filename, const std::vector<T>& arr, const std::vector<T>& filesig) {

        std::ofstream write_bin(filename, std::ios::out | std::ios::binary);
        if (!write_bin)
        {
            std::cout << "ERROR: writing key failed!\n";
            exit(1);
        }

        std::vector<T> writeData = filesig;
        writeData.insert(writeData.end(),arr.begin(), arr.end());

        size_t limit = writeData.size();
        write_bin.write(reinterpret_cast<char*>(&limit), sizeof(size_t));

        if (limit != 0)
        {
            write_bin.write(reinterpret_cast<const char*>(&writeData[0]), sizeof(T) * limit);
        }

        write_bin.close();
    }

    template<typename T>
    std::vector<T> read(const std::string& filename, const std::vector<T>& filesig) {

        std::ifstream read_bin(filename, std::ios::out | std::ios::binary);
        if (!read_bin)
        {
            std::cout << "\n\tERROR: key not found or corrupted!\n\n";
            exit(1);
        }

        size_t limit;
        read_bin.read(reinterpret_cast<char*>(&limit), sizeof(size_t));

        limit -= filesig.size();
        std::vector<T> sigRead(filesig.size(),0);
        read_bin.read(reinterpret_cast<char*>(&sigRead[0]), sizeof(T) * filesig.size());

        for(size_t i=0; i<filesig.size(); ++i)
        {
            if(filesig[i]!=sigRead[i])
            {
                std::cerr << "\n\tERROR: bad key!\n\n";
                exit(1);
            }
        }

        std::vector<T> arr(limit, 0);
        if (limit != 0)
        {
            read_bin.read(reinterpret_cast<char*>(&arr[0]), sizeof(T) * limit);
        }

        read_bin.close();

        return arr;
    }
}

#endif