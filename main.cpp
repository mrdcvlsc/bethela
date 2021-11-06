#include <iostream>
#include <cstring>
#include <chrono>

#include "byteio.hpp"
#include "cryptos/vigenere.hpp"

#define BETHELA_VERSION "version 1.2"

#define HELP_FLAG "--help"
#define VERSION_FLAG "--version"
#define ENCRYPT_FLAG "--encrypt"
#define DECRYPT_FLAG "--decrypt"
#define ENCRYPT_REPLACE_FLAG "--encrypt-replace"
#define DECRYPT_REPLACE_FLAG "--decrypt-replace"
#define GENERATE_FLAG "--generate"

#define COMMAND 1
#define KEY 2
#define STARTING_FILE 3

#define TIMING_START auto start = std::chrono::high_resolution_clock::now()

#define TIMING_END(NAME) \
auto end = std::chrono::high_resolution_clock::now(); \
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start); \
std::cout << NAME << "cryption took : \n"; \
std::cout << "\t" << duration.count() << " milliseconds\n"; \
std::cout << "\t" << duration.count()/1000 << " seconds\n"; \
std::cout << "\t" << NAME << "crypted File(s) : " << cnt << "\n"

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
            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = vigenere::readKey(args[KEY]);

            TIMING_START;
            size_t cnt = 0;
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if(!filebytestream.empty())
                {
                    vigenere::encrypt(filebytestream,loadKey);
                    cnt += byteio::file_write(args[i]+bconst::extension,filebytestream);
                }
            }
            TIMING_END("En");
        }
        else if(!strcmp(args[COMMAND],DECRYPT_FLAG))
        {
            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = vigenere::readKey(args[KEY]);

            TIMING_START;
            size_t cnt = 0;
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if(!filebytestream.empty())
                {
                    vigenere::decrypt(filebytestream,loadKey);
                    std::string output_filename(args[i]);
                    output_filename = output_filename.substr(0,output_filename.size()-bconst::extension.size());
                    cnt += byteio::file_write(output_filename,filebytestream);
                }
            }
            TIMING_END("De");
        }
        else if(!strcmp(args[COMMAND],ENCRYPT_REPLACE_FLAG))
        {
            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = vigenere::readKey(args[KEY]);

            TIMING_START;
            size_t cnt = 0;
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if(!filebytestream.empty())
                {
                    vigenere::encrypt(filebytestream,loadKey);
                    cnt += byteio::file_write(args[i]+bconst::extension,filebytestream);
                    int file_not_removed = std::remove(args[i]);
                    if(file_not_removed)
                    {
                        std::cout
                            << "notice: The program cannot remove the file '" << args[i] << "'\n"
                            << "        file location might be protected for delete actions.\n";    
                    }
                }
            }
            TIMING_END("En");
        }
        else if(!strcmp(args[COMMAND],DECRYPT_REPLACE_FLAG))
        {
            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = vigenere::readKey(args[KEY]);

            TIMING_START;
            size_t cnt = 0;
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if(!filebytestream.empty())
                {
                    vigenere::decrypt(filebytestream,loadKey);
                    std::string output_filename(args[i]);
                    output_filename = output_filename.substr(0,output_filename.size()-bconst::extension.size());
                    cnt += byteio::file_write(output_filename,filebytestream);
                    int file_not_removed = std::remove(args[i]);
                    if(file_not_removed)
                    {
                        std::cout
                            << "notice: The program cannot remove the file '" << args[i] << "'\n"
                            << "        file location might be protected for delete actions.\n";    
                    }
                }
            }
            TIMING_END("De");
        }
        else if(!strcmp(args[COMMAND],GENERATE_FLAG))
        {
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

                vigenere::generateKey(args[KEY],keysize);
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
            std::cerr <<
                "\t--help menu\n\n"
                "\tdisplay version:\n\n"
                "\t\tbethela --version\n\n\n"
                "\tencrypt command format:\n\n"
                "\t\tbethela --encrypt keyfile file1 file2 ... fileN\n\n\n"
                "\tdecrypt command format:\n\n"
                "\t\tbethela --decrypt keyfile file1 file2 ... fileN\n\n"
                "\t\tAs you can see you can pass 1 or more files to the program\n\n\n"
                "\tgenerate key format:\n\n"
                "\t\tbethela --generate keyfilename keysize\n\n"
                "\t\tthe key size should be a positive number greater than 0\n\n\n"
                "\tadding '-replace' command when encrypting/decrypting will\n"
                "\treplace the old files with the encrypted/decrypted files.\n\n"
                "\t\tbethela --encrypt-replace keyfile file1 file2 ... fileN\n"
                "\t\tbethela --decrypt-replace keyfile file1 file2 ... fileN\n\n\n";
        }
        else if(!strcmp(args[COMMAND],VERSION_FLAG))
        {
            std::cout << "bethela " << BETHELA_VERSION << "\n";
        }
        else
        {
            std::cerr <<
                "\n\tinvalid command, type the command below for info:\n\n"
                "\tbethela --help\n\n";
        }
    }
    else
    {
        std::cerr <<
            "A simple terminal command line tool for symmetric\n"
            "encrpytion & decryption of any files \n\n"
            "for more info about this program input the command:\n\n\tbethela --help\n\n";
    }
}