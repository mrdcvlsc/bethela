#ifndef BETHELA_VIGENERE_HPP
#define BETHELA_VIGENERE_HPP

#include <iostream>
#include <vector>
#include <chrono>
#include <random>

#include "../constants.hpp"
#include "keygenerator.hpp"

/// Vigenère cipher
namespace vigenere
{
    int ring(int add_or_sub_values)
    {
        int return_;
        if(add_or_sub_values < bconst::MIN)
        {
            return return_ = add_or_sub_values + (bconst::MAX+1);
        }
        
        if(add_or_sub_values > bconst::MAX+1)
        {
            return return_ = add_or_sub_values - (bconst::MAX+1);
        }
        
        return add_or_sub_values;
    }

    void encrypt(bconst::bytestream& data, const bconst::bytestream& key)
    {
        size_t ikey = 0;

        for(size_t i=0; i<data.size(); ++i)
        {
            data[i] = ring((int)data[i]+(int)key[ikey]);
            ikey++;
            if(ikey>=key.size()) ikey = 0;
        }
    }

    void decrypt(bconst::bytestream& data, const bconst::bytestream& key)
    {
        size_t ikey = 0;

        for(size_t i=0; i<data.size(); ++i)
        {
            data[i] = ring((int)data[i]-(int)key[ikey]);
            ikey++;
            if(ikey==key.size()) ikey = 0;
        }
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
}

#endif