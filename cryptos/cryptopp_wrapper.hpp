#ifndef CRYPTOPP_WRAPPER_CPP
#define CRYPTOPP_WRAPPER_CPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <assert.h>

#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <cryptopp/aes.h>
#include <cryptopp/ccm.h>

void printStringHex(const std::string& name, const std::string& str)
{
	std::string encoded;
	encoded.clear();
	CryptoPP::StringSource(str, true,
		new CryptoPP::HexEncoder(
			new CryptoPP::StringSink(encoded)
		)
	);
	std::cout << name << ": " << encoded << std::endl;
}

void printArrayHex(const std::string& name, unsigned char *array, size_t length)
{
	std::string encoded;
	encoded.clear();
	CryptoPP::StringSource(array, length, true,
		new CryptoPP::HexEncoder(
			new CryptoPP::StringSink(encoded)
		)
	);
	std::cout << name << ": " << encoded << std::endl;
}

unsigned char* CryptoPP_AES_encrypt_CBC(unsigned char in[], unsigned int inLen, unsigned  char key[], int keyByteLength, unsigned char * iv, unsigned int &outLen)
{
    std::string plain(reinterpret_cast<const char*>(in),inLen);
    std::string cipher;
    
    CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption crypt;
    crypt.SetKeyWithIV(key,keyByteLength,iv);
    CryptoPP::StringSource s(plain, true, 
        new CryptoPP::StreamTransformationFilter(crypt,
            new CryptoPP::StringSink(cipher)
        )
    );
    
    unsigned char* cipher_array = new unsigned char[cipher.size()];
    memcpy(cipher_array,cipher.data(),sizeof(const char)*cipher.size());
    outLen = cipher.size();
    return cipher_array;
}

unsigned char * CryptoPP_AES_decrypt_CBC(unsigned char in[], unsigned int inLen, unsigned  char key[], int keyByteLength, unsigned char * iv)
{
    std::string cipher(reinterpret_cast<const char*>(in),inLen);
    std::string recovered;

    CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption crypt;
    crypt.SetKeyWithIV(key,keyByteLength,iv);

    CryptoPP::StringSource s(cipher, true, 
        new CryptoPP::StreamTransformationFilter(crypt,
            new CryptoPP::StringSink(recovered)
        )
    );

    // because of padding, recovered.size() can be smaller than or equal with inLen
    // hence we use inLen when allocating recovered_array to avoid heap-buffer overflow
    // when we write the bytes later in the file
    unsigned char* recovered_array = new unsigned char[inLen];
    memcpy(recovered_array,recovered.data(),sizeof(const char)*recovered.size());
    return recovered_array;
}

#endif