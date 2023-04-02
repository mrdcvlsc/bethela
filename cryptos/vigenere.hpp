#ifndef BETHELA_VIGENERE_HPP
#define BETHELA_VIGENERE_HPP

#include <iostream>
#include <vector>
#include <chrono>
#include <random>

#include "../include/constants.hpp"
#include "keygenerator.hpp"

/// Vigenère cipher
namespace vigenere
{
    void encrypt(bconst::bytestream& data, const bconst::bytestream& key)
    {
        size_t ikey = 0;

        for(size_t i=0; i<data.size(); ++i)
        {
            data[i] = ((int)data[i]+(int)key[ikey])%bconst::CHARSETS;
            ikey++;
            if(ikey>=key.size()) {
                ikey = 0;
            }
        }
    }

    void decrypt(bconst::bytestream& data, const bconst::bytestream& key)
    {
        size_t ikey = 0;

        for(size_t i=0; i<data.size(); ++i)
        {
            data[i] = ((int)data[i]-(int)key[ikey])%bconst::CHARSETS;
            ikey++;
            if(ikey==key.size()) {
                ikey = 0;
            }
        }
    }
}

#endif