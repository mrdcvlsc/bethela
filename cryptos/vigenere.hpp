#ifndef BETHELA_VIGENERE_HPP
#define BETHELA_VIGENERE_HPP

#include <iostream>
#include <vector>

#include "../constants.hpp"

/// Vigenère cipher
namespace vigenere
{
    int ring(int add_or_sub_values)
    {
        int return_;
        if(add_or_sub_values < beth_const::MIN)
        {
            return return_ = add_or_sub_values + (beth_const::MAX+1);
        }
        
        if(add_or_sub_values > beth_const::MAX+1)
        {
            return return_ = add_or_sub_values - (beth_const::MAX+1);
        }
        
        return add_or_sub_values;
    }

    void encrypt(beth_const::bytestream& data, const beth_const::bytestream& key)
    {
        size_t ikey = 0;

        for(size_t i=0; i<data.size(); ++i)
        {
            data[i] = ring((int)data[i]+(int)key[ikey]);
            ikey++;
            if(ikey>=key.size()) ikey = 0;
        }
    }

    void decrypt(beth_const::bytestream& data, const beth_const::bytestream& key)
    {
        size_t ikey = 0;

        for(size_t i=0; i<data.size(); ++i)
        {
            data[i] = ring((int)data[i]-(int)key[ikey]);
            ikey++;
            if(ikey==key.size()) ikey = 0;
        }
    }
}

#endif