#ifndef CFB_MODE_CPP
#define CFB_MODE_CPP

#include "../mode.hpp"

namespace Krypt::Mode
{
    template<typename CIPHER_TYPE, typename PADDING_TYPE>
    CFB<CIPHER_TYPE,PADDING_TYPE>::CFB(const Bytes* key, size_t keyLen)
        : MODE<CIPHER_TYPE,PADDING_TYPE>()
    {
        this->Encryption = new CIPHER_TYPE(key,keyLen,NULL);
        this->PaddingScheme = new PADDING_TYPE();
    }

    template<typename CIPHER_TYPE, typename PADDING_TYPE>
    CFB<CIPHER_TYPE,PADDING_TYPE>::CFB(const Bytes* key, size_t keyLen, const Bytes* IV)
        : MODE<CIPHER_TYPE,PADDING_TYPE>()
    {
        this->Encryption = new CIPHER_TYPE(key,keyLen,IV);
        this->PaddingScheme = new PADDING_TYPE();
    }

    template<typename CIPHER_TYPE, typename PADDING_TYPE>
    std::pair<Bytes*,size_t> CFB<CIPHER_TYPE,PADDING_TYPE>::encrypt(Bytes* plain, size_t plainLen)
    {
        std::pair<Bytes*,size_t> padded = this->PaddingScheme->AddPadding(plain,plainLen,this->Encryption->BLOCK_SIZE);
        
        Bytes* tempIV = new Bytes[this->Encryption->BLOCK_SIZE];
        Bytes* encIV = new Bytes[this->Encryption->BLOCK_SIZE];
        Bytes* cipher = new Bytes[padded.second];

        memcpy(tempIV,this->Encryption->IV,this->Encryption->BLOCK_SIZE);

        for(size_t i=0; i<padded.second; i+=this->Encryption->BLOCK_SIZE)
        {
            this->Encryption->EncryptBlock(tempIV,encIV);
            XorBlocks(padded.first+i,encIV,cipher+i,this->Encryption->BLOCK_SIZE);
            memcpy(tempIV, cipher+i,this->Encryption->BLOCK_SIZE);
        }

        delete [] padded.first;
        delete [] tempIV;
        delete [] encIV;

        return {cipher,padded.second};
    }

    template<typename CIPHER_TYPE, typename PADDING_TYPE>
    std::pair<Bytes*,size_t> CFB<CIPHER_TYPE,PADDING_TYPE>::decrypt(Bytes* cipher, size_t cipherLen)
    {
        Bytes* recover = new Bytes[cipherLen];
        Bytes* tempIV = new Bytes[this->Encryption->BLOCK_SIZE];
        Bytes* encIV = new Bytes[this->Encryption->BLOCK_SIZE];

        memcpy(tempIV,this->Encryption->IV,this->Encryption->BLOCK_SIZE);

        for(size_t i=0; i<cipherLen; i+=this->Encryption->BLOCK_SIZE)
        {
            this->Encryption->EncryptBlock(tempIV,encIV);
            XorBlocks(cipher+i,encIV,recover+i,this->Encryption->BLOCK_SIZE);
            memcpy(tempIV, cipher+i,this->Encryption->BLOCK_SIZE);
        }

        std::pair<Bytes*,size_t> recoverNoPadding = this->PaddingScheme->RemovePadding(recover,cipherLen,this->Encryption->BLOCK_SIZE);


        delete [] recover;
        delete [] tempIV;
        delete [] encIV;

        return recoverNoPadding;
    }

    template<typename CIPHER_TYPE, typename PADDING_TYPE>
    void CFB<CIPHER_TYPE,PADDING_TYPE>::setIV(Bytes* inputIV)
    {
        this->Encryption->IV = inputIV;
    }
}

#endif