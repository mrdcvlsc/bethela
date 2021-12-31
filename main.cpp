#include <iostream>
#include <cstring>
#include <chrono>
#include <omp.h>

#if defined(USE_AESNI)
#define CRYPTOLIB "AES-NI"
#else
#define CRYPTOLIB "portable"
#endif

#include "byteio.hpp"
#include "cryptos/vigenere.hpp"
#include "cryptos/Krypt/src/Krypt.hpp"

#define BETHELA_VERSION "version 3.5.4"
#define SIZE_T_32BIT 4

#define HELP_FLAG "--help"
#define VERSION_FLAG "--version"
#define ENCRYPT_FLAG "--encrypt"
#define DECRYPT_FLAG "--decrypt"
#define GENERATE_FLAG "--generate"
#define REPLACE_FLAG "-replace"

#define AES_ENCRYPT "--enc-AES"
#define AES_DECRYPT "--dec-AES"

#define AES_BLOCKSIZE 16

#define AES128_BYTEKEY 16
#define AES192_BYTEKEY 24
#define AES256_BYTEKEY 32

#define COMMAND 1
#define KEY 2
#define STARTING_FILE 3

#define BUFFER_BYTESIZE 134217728

#define TIMING_START std::cout << "program running, please wait...\n"; \
auto start = std::chrono::high_resolution_clock::now()

#define TIMING_END(NAME) \
auto end = std::chrono::high_resolution_clock::now(); \
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start); \
std::cout << NAME << "cryption took : \n"; \
std::cout << "\t" << duration.count() << " milliseconds\n"; \
std::cout << "\t" << duration.count()/1000 << " seconds\n"; \
std::cout << "\t" << NAME << "crypted File(s) : " << cnt << "\n"

void NOTV_AES_KEY(size_t ARGS_AES_KEY_SIZE)
{
    switch(ARGS_AES_KEY_SIZE)
    {
        case AES128_BYTEKEY*8: return;
        case AES192_BYTEKEY*8: return;
        case AES256_BYTEKEY*8: return;
    }
    std::cerr << "\n\tERROR: invalid AES command given\n";
    std::cout << "\t\t : "<< ARGS_AES_KEY_SIZE << "\n";
    exit(1);
}

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

#define CHECK_AES_ARG(AES_ARG_INPUT_COMMAND) \
std::string AES_ARGUMENT(AES_ARG_INPUT_COMMAND); \
if(!(AES_ARGUMENT.size()!=12 ^ AES_ARGUMENT.size()!=20)) \
{ \
    std::cerr << "\n\tERROR: AES key length invalid\n"; \
    std::cerr <<   "\t       from arg " << AES_ARG_INPUT_COMMAND << "\n"; \
    exit(1); \
} \
std::string AES_KEY_SIZE_STR; \
AES_KEY_SIZE_STR.push_back(AES_ARG_INPUT_COMMAND[9]); \
AES_KEY_SIZE_STR.push_back(AES_ARG_INPUT_COMMAND[10]); \
AES_KEY_SIZE_STR.push_back(AES_ARG_INPUT_COMMAND[11]); \
size_t AES_KEY_SIZE = std::atol(AES_KEY_SIZE_STR.data()); \
NOTV_AES_KEY(AES_KEY_SIZE); \
std::cerr << "Key length : " << AES_KEY_SIZE << "\n";

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

using namespace Krypt;

int main(int argc, char* args[])
{
    std::cout << "\n";

    if(argc>=2)
    {
        if(MATCH(args[COMMAND],ENCRYPT_FLAG))
        {
            std::cout << "Default Encryption : Vigenere cipher \n";
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
                    filebytestream.insert(filebytestream.end(),bconst::FILESIGNATURE.begin(),bconst::FILESIGNATURE.end());
                    cnt += byteio::file_write(args[i]+bconst::extension,filebytestream);
                    CHECKIF_REPLACE(args[COMMAND],args[i]);
                }
            }
            TIMING_END("En");
        }
        else if(MATCH(args[COMMAND],DECRYPT_FLAG))
        {
            std::cout << "Default Decryption : Vigenere cipher \n";
            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);

            TIMING_START;
            size_t cnt = 0;
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if(filebytestream.empty()) continue;

                bconst::bytestream filesig(filebytestream.end()-bconst::FILESIGNATURE.size(),filebytestream.end());
                if(filesig!=bconst::FILESIGNATURE)
                {
                    std::cerr << "The file '" << args[i] << "' is not encrypted, no need to decrypt it!\n";
                    continue;
                }
                
                filebytestream.erase(filebytestream.end()-bconst::FILESIGNATURE.size(),filebytestream.end());
               
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
        else if(MATCH(args[COMMAND],AES_ENCRYPT))
        {
            CHECK_AES_ARG(args[COMMAND]);

            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);
            keygen::AES_KEYCHECK(loadKey,AES_KEY_SIZE);

            Mode::CBC<BlockCipher::AES,Padding::NoPadding> blocksNoPadding(loadKey.data(),loadKey.size());
            Mode::CBC<BlockCipher::AES,Padding::PKCS_5_7> lastBlockKrypt(loadKey.data(),loadKey.size());

            TIMING_START;
            size_t cnt = 0;

            std::cout << "Encryption : AES block cipher \n";
            #pragma omp parallel for num_threads(omp_get_max_threads())
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                std::cout << "file : " << args[i] << "...\n";
                char* tbuffer = new char[BUFFER_BYTESIZE];
                std::string infname(args[i]), outfname(args[i]+bconst::extension);
                std::ifstream curr_file(infname,std::ios::binary);
                
                Krypt::Bytes* iv = keygen::random_bytestream_array(AES_BLOCKSIZE);

                std::ofstream output_file(outfname,std::ios::binary | std::ios::trunc);
                output_file.write(reinterpret_cast<const char*>(bconst::FILESIGNATURE.data()),bconst::FILESIGNATURE.size());
                output_file.close();
                output_file.open(outfname,std::ios::binary | std::ios::app);
                output_file.write(reinterpret_cast<const char*>(iv),AES_BLOCKSIZE);

                while(!curr_file.eof())
                {
                    curr_file.read(tbuffer,BUFFER_BYTESIZE);
                    size_t read_buffer_size = curr_file.gcount();

                    if(!curr_file.eof() && read_buffer_size==BUFFER_BYTESIZE)
                    {
                        Krypt::ByteArray cipher = blocksNoPadding.encrypt(reinterpret_cast<unsigned char*>(tbuffer),BUFFER_BYTESIZE,iv);
                        output_file.write(reinterpret_cast<char*>(cipher.array),cipher.length);
                        memcpy(iv,cipher.array+(BUFFER_BYTESIZE-AES_BLOCKSIZE),AES_BLOCKSIZE);
                    }
                    else if(curr_file.eof())
                    {
                        Krypt::ByteArray cipher = lastBlockKrypt.encrypt(reinterpret_cast<unsigned char*>(tbuffer),read_buffer_size,iv);
                        output_file.write(reinterpret_cast<char*>(cipher.array),cipher.length);
                    }
                    else
                    {
                        throw std::logic_error("enc: something wrong happend");
                    }
                }
                cnt++;
                delete [] iv;
                delete [] tbuffer;
                CHECKIF_REPLACE(args[COMMAND],args[i]);
            }
            TIMING_END("En");
        }
        else if(MATCH(args[COMMAND],AES_DECRYPT))
        {
            CHECK_AES_ARG(args[COMMAND]);

            emptyFileArgs(args[COMMAND],argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);
            keygen::AES_KEYCHECK(loadKey,AES_KEY_SIZE);

            Mode::CBC<BlockCipher::AES,Padding::NoPadding> blocksNoPadding(loadKey.data(),loadKey.size());
            Mode::CBC<BlockCipher::AES,Padding::PKCS_5_7> lastBlockKrypt(loadKey.data(),loadKey.size());

            TIMING_START;
            size_t cnt = 0;

            std::cout << "Decryption : AES block cipher \n";
            #pragma omp parallel for num_threads(omp_get_max_threads())
            for(int i=STARTING_FILE; i<argc; ++i)
            {
                std::cout << "file : " << args[i] << "...\n";
                char* tbuffer = new char[BUFFER_BYTESIZE];
                char* filesig = new char[bconst::FILESIGNATURE.size()];
                std::string infname(args[i]), outfname(args[i]);
                outfname = outfname.substr(0,outfname.size()-bconst::extension.size());

                std::ifstream curr_file(infname,std::ios::binary);

                curr_file.read(filesig,bconst::FILESIGNATURE.size());

                if(memcmp(filesig,bconst::FILESIGNATURE.data(),bconst::FILESIGNATURE.size()))
                {
                    std::cerr << "The file '" << args[i] << "' is not encrypted, no need to decrypt it!\n";
                }
                else
                {
                    std::ofstream output_file(outfname, std::ios::binary | std::ios::trunc);
                    output_file.close();
                    output_file.open(outfname, std::ios::binary | std::ios::app);

                    char* iv = new char[AES_BLOCKSIZE];
                    curr_file.read(iv,AES_BLOCKSIZE);

                    while(!curr_file.eof())
                    {
                        curr_file.read(tbuffer,BUFFER_BYTESIZE);
                        size_t read_buffer_size = curr_file.gcount();

                        if(!curr_file.eof() && read_buffer_size==BUFFER_BYTESIZE)
                        {
                            Krypt::ByteArray recover = blocksNoPadding.decrypt(reinterpret_cast<unsigned char*>(tbuffer),read_buffer_size,reinterpret_cast<unsigned char*>(iv));
                            output_file.write(reinterpret_cast<char*>(recover.array),recover.length);
                            memcpy(iv,reinterpret_cast<char*>(recover.array+(BUFFER_BYTESIZE-AES_BLOCKSIZE)),AES_BLOCKSIZE);
                        }
                        else if(curr_file.eof())
                        {
                            Krypt::ByteArray recover = lastBlockKrypt.decrypt(reinterpret_cast<unsigned char*>(tbuffer),read_buffer_size,reinterpret_cast<unsigned char*>(iv));
                            output_file.write(reinterpret_cast<char*>(recover.array),recover.length);  
                        }
                        else
                        {
                            throw std::logic_error("something wrong happen");
                        }
                    }
                    delete [] iv;
                    cnt++;
                    CHECKIF_REPLACE(args[COMMAND],args[i]);
                }

                delete [] tbuffer;
                delete [] filesig;
                ///////////////////
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
                "\t\tAES256    " << AES_ENCRYPT << "XXX & " << AES_DECRYPT << "XXX\n\n\n";
        }
        else if(!strcmp(args[COMMAND],VERSION_FLAG))
        {
            int executable_bit = sizeof(size_t)==SIZE_T_32BIT ? 32 : 64;
            std::cout << "bethela " << executable_bit << "-bit : " << BETHELA_VERSION << " [" << CRYPTOLIB << "]\n";
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
