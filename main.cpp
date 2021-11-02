#include <iostream>
#include <cstring>
#include <chrono>

#include "bethela_key.hpp"
#include "readbinfile.hpp"

#define BETHELA_VERSION "version 1.0"

#define HELP_FLAG "--help"
#define ENCRYPT_FLAG "--encrypt"
#define DECRYPT_FLAG "--decrypt"
#define GENERATE_FLAG "--generate"

#define COMMAND 1
#define KEY 2
#define STARTING_FILE 3

void emptyFileArgs(char cmd[10], int argcnt)
{
    if(argcnt==2)
    {
        std::cerr << "\n\tERROR: command " << cmd << "\n";
        std::cerr <<   "\t       no key provided\n";
        exit(1);
    }
    else if(argcnt==3)
    {
        std::cerr << "\n\tERROR: command " << cmd << "\n";
        std::cerr <<   "\t       no files provided\n";
        exit(1);
    }
}

int main(int argc, char* args[])
{
    if(argc>=2)
    {
        if(!strcmp(args[COMMAND],ENCRYPT_FLAG))
        {
            #ifndef RELEASE
            std::cout << "Encrypt\n";
            #endif

            emptyFileArgs(args[COMMAND],argc);
            bethela::bhstream loadKey = bethela::readKey(args[KEY]);

            auto start = std::chrono::high_resolution_clock::now();
            size_t cnt = 0;
            for(size_t i=STARTING_FILE; i<argc; ++i)
            {
                cnt += filemanage::encryptFile(args[i],loadKey);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
            std::cout << "Encryption took : \n";
            std::cout << "\t" << duration.count() << " milliseconds\n";
            std::cout << "\t" << duration.count()/1000 << " seconds\n";
            std::cout << "\tEncrypted File(s) : " << cnt << "\n";
        }
        else if(!strcmp(args[COMMAND],DECRYPT_FLAG))
        {
            #ifndef RELEASE
            std::cout << "Decrypt\n";
            #endif

            emptyFileArgs(args[COMMAND],argc);
            bethela::bhstream loadKey = bethela::readKey(args[KEY]);

            auto start = std::chrono::high_resolution_clock::now();
            size_t cnt = 0;
            for(size_t i=STARTING_FILE; i<argc; ++i)
            {
                cnt += filemanage::decryptFile(args[i],loadKey);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
            std::cout << "Decryption took : \n";
            std::cout << "\t" << duration.count() << " milliseconds\n";
            std::cout << "\t" << duration.count()/1000 << " seconds\n";
            std::cout << "Decrypted File(s) : " << cnt << "\n";
        }
        else if(!strcmp(args[COMMAND],GENERATE_FLAG))
        {
            #ifndef RELEASE
            std::cout << "Generate\n";
            #endif

            if(argc==2)
            {
                std::cerr << "\n\tERROR: --generate can only be followed by the filename\n";
                std::cerr <<   "\t         of the key that you want to create and the size\n";
                exit(1);
            }
            else if(argc==3)
            {
                std::cerr << "\n\tERROR: --generate key has no size provided\n";
                exit(1);
            }
            else if(argc==4)
            {
                int keysize = std::atoi(args[KEY+1]);

                if(keysize<1)
                {
                    std::cerr << "\n\tERROR: --generate cannot take a negative keysize\n\n";
                    exit(1);
                }

                bethela::generateKey(args[KEY],keysize);
                std::cout<<"success - generated keyfile : "<<args[KEY]<<"\n";
            }
            else
            {
                std::cerr << "\n\tERROR: --generate has too many arguments\n";
                exit(1);
            }
        }
        else if(!strcmp(args[COMMAND],HELP_FLAG))
        {
            #ifndef RELEASE
            std::cout <<
                "\t--help menu\n\n"
                "\tencrypt command format:\n\n"
                "\t\tbethela --encrypt keyfile file1 file2 ... fileN\n\n\n"
                "\tdecrypt command format:\n\n"
                "\t\tbethela --decrypt keyfile file1 file2 ... fileN\n\n"
                "\t\tAs you can see you can pass 1 or more files to the program\n\n\n"
                "\tgenerate key format:\n\n"
                "\t\tbethela --generate keyfilename keysize\n\n"
                "\t\tthe key size should be a positive number greater than 0\n\n\n";
            #endif
        }
        else
        {
            #ifndef RELEASE
            std::cerr <<
                "\n\tinvalid command, type the command below for info:\n\n"
                "\t\tbethela --help\n\n";     
            #endif
        }
    }
    else
    {
        std::cout << "bethela " << BETHELA_VERSION << "\n";
    }
}