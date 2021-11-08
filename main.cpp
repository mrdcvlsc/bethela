#include <iostream>
#include <cstring>
#include <chrono>

#include "byteio.hpp"
#include "cryptos/vigenere.hpp"
#include "cryptos/AES_SergeyBel.hpp"

#define BETHELA_VERSION "version 1.7"
#define SIZE_T_32BIT 4

#define HELP_FLAG "--help"
#define VERSION_FLAG "--version"
#define ENCRYPT_FLAG "--encrypt"
#define DECRYPT_FLAG "--decrypt"
#define GENERATE_FLAG "--generate"
#define REPLACE_FLAG "-replace"

#define AES256_ENCRYPT "--aes256en"
#define AES256_DECRYPT "--aes256de"
#define AES256_BYTEKEY 32

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

bool MATCH(const std::string& INPUT, const std::string& COMMAND_FLAG)
{
    for(size_t i=0; i<COMMAND_FLAG.size() && i<INPUT.size(); ++i)
    {
        if(INPUT[i]!=COMMAND_FLAG[i]) return false;
    }
    return true;
}

void CHECKIF_REPLACE(const std::string& INPUT_COMMAND, const std::string& FILE_TO_REMOVE)
{
    size_t found = INPUT_COMMAND.find(REPLACE_FLAG);
    if(found!=std::string::npos)
    {
        int file_not_removed = std::remove(FILE_TO_REMOVE.data());
        if(file_not_removed)
        {
            std::cout
                << "notice: The program cannot remove the file '" << FILE_TO_REMOVE << "'\n"
                << "        file location might be protected for delete actions.\n";    
        }
    }
}

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
        if(MATCH(args[COMMAND],ENCRYPT_FLAG))
        {
            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);

            TIMING_START;
            size_t cnt = 0;
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if(!filebytestream.empty())
                {
                    vigenere::encrypt(filebytestream,loadKey);
                    cnt += byteio::file_write(args[i]+bconst::extension,filebytestream);
                    CHECKIF_REPLACE(args[COMMAND],args[i]);
                }
            }
            TIMING_END("En");
        }
        else if(MATCH(args[COMMAND],DECRYPT_FLAG))
        {
            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);

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
                    CHECKIF_REPLACE(args[COMMAND],args[i]);
                }
            }
            TIMING_END("De");
        }
        else if(MATCH(args[COMMAND],AES256_ENCRYPT))
        {
            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);
            keygen::AES_KEYCHECK(loadKey);

            TIMING_START;
            size_t cnt = 0;
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if(!filebytestream.empty())
                {
                    bconst::bytestream iv = keygen::random_bytestream(AES256_BYTEKEY);
                    SergeyBel::AES crypt;
                    unsigned int output_len = 0;

                    bconst::byte* encrypt_raw = crypt.EncryptCBC(filebytestream.data(),filebytestream.size(),loadKey.data(),iv.data(),output_len);
                    bconst::bytestream encrypted(encrypt_raw,encrypt_raw+output_len);
                    encrypted.insert(encrypted.end(),iv.begin(),iv.end());

                    cnt += byteio::file_write(args[i]+bconst::extension,encrypted);

                    CHECKIF_REPLACE(args[COMMAND],args[i]);
                    delete [] encrypt_raw;
                }
            }
            TIMING_END("En");
        }
        else if(MATCH(args[COMMAND],AES256_DECRYPT))
        {
            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);
            keygen::AES_KEYCHECK(loadKey);
            
            TIMING_START;
            size_t cnt = 0;
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if(!filebytestream.empty())
                {
                    bconst::bytestream iv(filebytestream.end()-AES256_BYTEKEY,filebytestream.end());
                    filebytestream.erase(filebytestream.end()-AES256_BYTEKEY,filebytestream.end());
                    SergeyBel::AES crypt;
                    unsigned int output_len = filebytestream.size();

                    bconst::byte* decrypt_raw = crypt.DecryptCBC(filebytestream.data(),output_len,loadKey.data(),iv.data());
                    
                    std::string output_filename(args[i]);
                    output_filename = output_filename.substr(0,output_filename.size()-bconst::extension.size());
                    cnt += byteio::file_write(output_filename,decrypt_raw,output_len);
                    CHECKIF_REPLACE(args[COMMAND],args[i]);
                    delete [] decrypt_raw;
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

                keygen::generateKey(args[KEY],keysize);
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
                "\t" << HELP_FLAG << " menu\n\n"
                "\tdisplay version:\n\n"
                "\t\tbethela " << VERSION_FLAG << "\n\n\n"
                "\tencrypt command format:\n\n"
                "\t\tbethela " << ENCRYPT_FLAG << " keyfile file1 file2 ... fileN\n\n\n"
                "\tdecrypt command format:\n\n"
                "\t\tbethela " << DECRYPT_FLAG << " keyfile file1 file2 ... fileN\n\n"
                "\t\tAs you can see you can pass 1 or more files to the program\n\n\n"
                "\tgenerate key format:\n\n"
                "\t\tbethela " << GENERATE_FLAG << " keyfilename keysize\n\n"
                "\t\tthe key size should be a positive number greater than 0\n\n\n"
                "\tadding '-replace' command when encrypting/decrypting will\n"
                "\treplace the old files with the encrypted/decrypted files.\n\n"
                "\t\tbethela " << ENCRYPT_FLAG << REPLACE_FLAG << " keyfile file1 file2 ... fileN\n"
                "\t\tbethela "<< DECRYPT_FLAG << REPLACE_FLAG << " keyfile file1 file2 ... fileN\n\n\n"
                "\talgorithms:\n\n"
                "\t\tVigenère  " << ENCRYPT_FLAG << " & " << DECRYPT_FLAG << "\n"
                "\t\tAES256    " << AES256_ENCRYPT << " & " << AES256_DECRYPT << "\n\n\n";
        }
        else if(!strcmp(args[COMMAND],VERSION_FLAG))
        {
            int executable_bit = sizeof(size_t)==SIZE_T_32BIT ? 32 : 64;
            std::cout << "bethela " << executable_bit << "-bit : " << BETHELA_VERSION << "\n";
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
