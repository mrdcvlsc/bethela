#ifndef BETHELA_CRYPTALGO_HPP
#define BETHELA_CRYPTALGO_HPP

#include <iostream>
#include <vector>

#include "constants.hpp"

namespace lock
{
    typedef unsigned char byte;
    typedef std::vector<byte> bhstream;

    int ring(int add_or_sub_values)
    {
        int return_;
        if(add_or_sub_values < bthconst::MIN)
        {
            return return_ = add_or_sub_values + (bthconst::MAX+1);
        }
        
        if(add_or_sub_values > bthconst::MAX+1)
        {
            return return_ = add_or_sub_values - (bthconst::MAX+1);
        }
        
        return add_or_sub_values;
    }

    void encrypt(bhstream& data, const bhstream& key)
    {
        size_t ikey = 0;

        for(size_t i=0; i<data.size(); ++i)
        {
            data[i] = ring((int)data[i]+(int)key[ikey]);
            ikey++;
            if(ikey>=key.size()) ikey = 0;
        }
    }

    void decrypt(bhstream& data, const bhstream& key)
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