#ifndef KEYGENERATOR_HPP
#define KEYGENERATOR_HPP

#include <iostream>
#include <vector>
#include <fstream>

#include "../include/constants.hpp"

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

        int keylen_offset = writeData.size();
        write_bin.write(reinterpret_cast<char*>(&keylen_offset), sizeof(int));

        if (keylen_offset != 0)
        {
            write_bin.write(reinterpret_cast<const char*>(&writeData[0]), sizeof(T) * keylen_offset);
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

        int keylen_offset;
        read_bin.read(reinterpret_cast<char*>(&keylen_offset), sizeof(int));

        keylen_offset -= filesig.size();
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

        std::vector<T> arr(keylen_offset, 0);
        if (keylen_offset != 0)
        {
            read_bin.read(reinterpret_cast<char*>(&arr[0]), sizeof(T) * keylen_offset);
        }

        read_bin.close();

        return arr;
    }

    void generateKey(const std::string& keyname, size_t keysize)
    {
        bconst::bytestream new_key;
        new_key.reserve(keysize);

        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937_64 rand_engine(seed);
        std::uniform_int_distribution<int> random_number(bconst::MIN,bconst::MAX);

        for(size_t i=0; i<keysize; ++i)
            new_key.push_back(random_number(rand_engine));

        keygen::write(keyname,new_key,bconst::FILESIGNATURE);
    }

    bconst::bytestream readKey(const std::string& keyfile)
    {
        return keygen::read<bconst::byte>(keyfile,bconst::FILESIGNATURE);
    }

    bconst::bytestream random_bytestream(size_t byte_count)
    {
        bconst::bytestream random_bytes;
        random_bytes.reserve(byte_count);

        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937_64 rand_engine(seed);
        std::uniform_int_distribution<int> random_number(bconst::MIN,bconst::MAX);

        for(size_t i=0; i<byte_count; ++i)
            random_bytes.push_back(random_number(rand_engine));

        return random_bytes;
    }

    bconst::byte* random_bytestream_array(size_t byte_count)
    {
        bconst::byte* random_bytes = new bconst::byte[byte_count];

        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937_64 rand_engine(seed);
        std::uniform_int_distribution<int> random_number(bconst::MIN,bconst::MAX);


        for(size_t i=0; i<byte_count; ++i)
            random_bytes[i] = random_number(rand_engine);

        return random_bytes;
    }

    void AES_KEYCHECK(const bconst::bytestream& AES_KEY, size_t GIVEN_AES_KEY_SIZE)
    {
        if(AES_KEY.size()==32 || AES_KEY.size()==24 || AES_KEY.size()==16)
        {
            if(AES_KEY.size()!=(GIVEN_AES_KEY_SIZE/8))
            {
                std::cerr << "\n\tERROR: bad key!\n\n";
                exit(1);
            }
            return;
        }
        std::cerr << "\n\tERROR: bad key!\n\n";
        exit(1);
    }
}

#endif