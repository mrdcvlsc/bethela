#ifndef BETHELA_KEYGEN_HPP
#define BETHELA_KEYGEN_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>

#include "keygenerator.hpp"
#include "constants.hpp"

namespace bethela
{
    typedef unsigned char byte;
    typedef std::vector<byte> bhstream;
    const static std::vector<byte> FILESIGNATURE({ 'B','E','T','H','E','L','A' });

    void generateKey(const std::string& keyname, size_t keysize)
    {
        std::vector<byte> new_key;
        new_key.reserve(keysize);

        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937_64 rand_engine(seed);
        std::uniform_int_distribution<int> random_number(bthconst::MIN,bthconst::MAX);

        for(size_t i=0; i<keysize; ++i)
            new_key.push_back(random_number(rand_engine));

        keygen::write(keyname,new_key,FILESIGNATURE);
    }

    std::vector<byte> readKey(const std::string& keyfile)
    {
        return keygen::read<byte>(keyfile,FILESIGNATURE);
    }
}

#endif