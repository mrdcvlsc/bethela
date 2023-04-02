#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>

#include "cryptos/Krypt/src/Krypt.hpp"
#include "cryptos/vigenere.hpp"

#include "include/argscheck.hpp"
#include "include/timing.hpp"
#include "include/byteio.hpp"

#if defined(USE_AESNI)
    #define CRYPTOLIB "AES-NI"
#elif defined(USE_ARM_AES)
    #define CRYPTOLIB "neon"
#else
    #define CRYPTOLIB "portable"
#endif

#define BETHELA_VERSION "version 3.5.8"

#define HELP_FLAG "--help"
#define VERSION_FLAG "--version"
#define ENCRYPT_FLAG "--encrypt"
#define DECRYPT_FLAG "--decrypt"
#define GENERATE_FLAG "--generate"
#define REPLACE_FLAG "-replace"

#define AES_ENCRYPT "--enc-AES"
#define AES_DECRYPT "--dec-AES"

using namespace Krypt;

constexpr size_t SIZE_T_32BIT = 4;

constexpr size_t AES_BLOCKSIZE = 16;

constexpr size_t AES128_BYTEKEY = 16;
constexpr size_t AES192_BYTEKEY = 24;
constexpr size_t AES256_BYTEKEY = 32;

constexpr size_t COMMAND = 1;
constexpr size_t KEY = 2;
constexpr size_t STARTING_FILE = 3;

/// 192 MB.
constexpr size_t BUFFER_BYTESIZE = 201326592;

std::mutex global_mtx;

int main(int argc, char *args[]) {
    const size_t processor_count = std::thread::hardware_concurrency();

    std::cout << "\n";

    std::vector<std::string> file_queue;

    for (size_t i = STARTING_FILE; i < (size_t) argc; ++i) {
        file_queue.push_back(args[i]);
    }

    if (argc >= 2) {
        if (match(args[COMMAND], ENCRYPT_FLAG)) {
            std::cout << "Default Encryption : Vigenere cipher \n";
            emptyFileArgs(args[COMMAND], argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);

            auto start = timing_start();
            size_t cnt = 0;
            for (int i = STARTING_FILE; i < argc; ++i) {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if (!filebytestream.empty()) {
                    vigenere::encrypt(filebytestream, loadKey);
                    filebytestream.insert(
                        filebytestream.end(), bconst::FILESIGNATURE.begin(), bconst::FILESIGNATURE.end()
                    );
                    cnt += byteio::file_write(args[i] + bconst::extension, filebytestream);
                    checkif_replace(args[COMMAND], args[i]);
                }
            }
            timing_end("En", start, cnt);
        } else if (match(args[COMMAND], DECRYPT_FLAG)) {
            std::cout << "Default Decryption : Vigenere cipher \n";
            emptyFileArgs(args[COMMAND], argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);

            auto start = timing_start();
            size_t cnt = 0;
            for (int i = STARTING_FILE; i < argc; ++i) {
                bconst::bytestream filebytestream = byteio::file_read(args[i]);
                if (filebytestream.empty())
                    continue;

                bconst::bytestream filesig(filebytestream.end() - bconst::FILESIGNATURE.size(), filebytestream.end());
                if (filesig != bconst::FILESIGNATURE) {
                    std::cerr << "The file '" << args[i] << "' is not encrypted, no need to decrypt it!\n";
                    continue;
                }

                filebytestream.erase(filebytestream.end() - bconst::FILESIGNATURE.size(), filebytestream.end());

                if (!filebytestream.empty()) {
                    vigenere::decrypt(filebytestream, loadKey);
                    std::string output_filename(args[i]);

                    output_filename = output_filename.substr(0, output_filename.size() - bconst::extension.size());
                    cnt += byteio::file_write(output_filename, filebytestream);
                    checkif_replace(args[COMMAND], args[i]);
                }
            }
            timing_end("De", start, cnt);
        } else if (match(args[COMMAND], AES_ENCRYPT)) {
            size_t AES_KEY_SIZE = check_aes_arg(args[COMMAND]);

            emptyFileArgs(args[COMMAND], argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);
            keygen::AES_KEYCHECK(loadKey, AES_KEY_SIZE);

            Mode::CBC<BlockCipher::AES, Padding::PKCS_5_7> aes_scheme(loadKey.data(), loadKey.size());

            auto start = timing_start();
            std::atomic<size_t> cnt(0);

            std::cout << "Encryption : AES block cipher \n";

            auto encrypt_lambda = [&]() {
                global_mtx.lock();
                std::cout << "Encryption threads running : tid = " << std::this_thread::get_id() << "\n";
                global_mtx.unlock();

                bool run_thread = true;

                char *tbuffer = new char[BUFFER_BYTESIZE];
                Krypt::Bytes *encryptedBuffer = new Krypt::Bytes[BUFFER_BYTESIZE];

                while (run_thread) {
                    std::string target_file;

                    global_mtx.lock();
                    run_thread = !file_queue.empty();

                    if (run_thread) {
                        target_file = file_queue.back();
                        file_queue.pop_back();
                    } else {
                        global_mtx.unlock();
                        break;
                    }

                    global_mtx.unlock();

                    std::string outfname(target_file + bconst::extension);
                    std::ifstream curr_file(target_file, std::ios::binary);

                    if (!curr_file.good()) {
                        global_mtx.lock();
                        std::cout << "The file : " << target_file
                                  << "\n"
                                     "was not encrypted...\n"
                                     "it might be read protected, corrupted or non-existent...\n";
                        global_mtx.unlock();
                    } else {
                        global_mtx.lock();
                        std::cout << "encrypting : " << target_file << "...\n";
                        global_mtx.unlock();

                        Krypt::Bytes *iv = keygen::random_bytestream_array(AES_BLOCKSIZE);

                        std::ofstream output_file(outfname, std::ios::binary | std::ios::trunc);
                        output_file.write(
                            reinterpret_cast<const char *>(bconst::FILESIGNATURE.data()), bconst::FILESIGNATURE.size()
                        );
                        output_file.close();
                        output_file.open(outfname, std::ios::binary | std::ios::app);
                        output_file.write(reinterpret_cast<const char *>(iv), AES_BLOCKSIZE);

                        while (!curr_file.eof()) {
                            curr_file.read(tbuffer, BUFFER_BYTESIZE);
                            size_t read_buffer_size = curr_file.gcount();

                            if (!curr_file.eof() && read_buffer_size == BUFFER_BYTESIZE) {
                                for (size_t index = 0; index < read_buffer_size; index += AES_BLOCKSIZE) {
                                    aes_scheme.blockEncrypt(
                                        reinterpret_cast<unsigned char *>(tbuffer + index),
                                        reinterpret_cast<unsigned char *>(encryptedBuffer + index), iv
                                    );
                                }
                                output_file.write(reinterpret_cast<char *>(encryptedBuffer), read_buffer_size);
                            } else if (curr_file.eof()) {
                                if (read_buffer_size) {

                                    size_t remaining_blocks = read_buffer_size / AES_BLOCKSIZE;
                                    size_t remaining_bytes = read_buffer_size % AES_BLOCKSIZE;
                                    size_t index;

                                    for (index = 0; index < remaining_blocks; ++index) {
                                        aes_scheme.blockEncrypt(
                                            reinterpret_cast<unsigned char *>(tbuffer + (index * AES_BLOCKSIZE)),
                                            reinterpret_cast<unsigned char *>(
                                                encryptedBuffer + (index * AES_BLOCKSIZE)
                                            ),
                                            iv
                                        );
                                    }
                                    output_file.write(
                                        reinterpret_cast<char *>(encryptedBuffer), remaining_blocks * AES_BLOCKSIZE
                                    );

                                    Krypt::ByteArray cipher = aes_scheme.encrypt(
                                        reinterpret_cast<unsigned char *>(tbuffer + (index * AES_BLOCKSIZE)),
                                        remaining_bytes, iv
                                    );
                                    output_file.write(reinterpret_cast<char *>(cipher.array), cipher.length);
                                }
                            } else {
                                memset((unsigned char *) iv, 0x00, AES_BLOCKSIZE);
                                memset((char *) tbuffer, 0x00, BUFFER_BYTESIZE);
                                memset((Krypt::Bytes *) encryptedBuffer, 0x00, BUFFER_BYTESIZE);

                                delete[] iv;
                                delete[] tbuffer;
                                delete[] encryptedBuffer;

                                throw std::logic_error("enc: something wrong happend");
                            }
                        }
                        cnt++;

                        memset((unsigned char *) iv, 0x00, AES_BLOCKSIZE);
                        delete[] iv;

                        checkif_replace(args[COMMAND], target_file);
                    }
                }

                memset((char *) tbuffer, 0x00, BUFFER_BYTESIZE);
                memset((Krypt::Bytes *) encryptedBuffer, 0x00, BUFFER_BYTESIZE);

                delete[] tbuffer;
                delete[] encryptedBuffer;
            };

            std::vector<std::thread> threads;

            for (size_t i = 0; i < processor_count - 1; ++i) {
                global_mtx.lock();
                threads.push_back(std::thread(encrypt_lambda));
                global_mtx.unlock();
            }

            encrypt_lambda();

            for (size_t i = 0; i < processor_count - 1; ++i) {
                threads[i].join();
            }

            timing_end("En", start, cnt);
        } else if (match(args[COMMAND], AES_DECRYPT)) {
            size_t AES_KEY_SIZE = check_aes_arg(args[COMMAND]);

            emptyFileArgs(args[COMMAND], argc);
            bconst::bytestream loadKey = keygen::readKey(args[KEY]);
            keygen::AES_KEYCHECK(loadKey, AES_KEY_SIZE);

            Mode::CBC<BlockCipher::AES, Padding::PKCS_5_7> aes_scheme(loadKey.data(), loadKey.size());

            auto start = timing_start();
            std::atomic<size_t> cnt(0);

            std::cout << "Decryption : AES block cipher \n";

            auto decrypt_lambda = [&]() {
                global_mtx.lock();
                std::cout << "Decryption threads running : tid = " << std::this_thread::get_id() << "\n";
                global_mtx.unlock();

                bool run_thread = true;

                char *tbuffer = new char[BUFFER_BYTESIZE];
                char *filesig = new char[bconst::FILESIGNATURE.size()];
                Krypt::Bytes *decryptedBuffer = new Krypt::Bytes[BUFFER_BYTESIZE];

                while (run_thread) {
                    std::string target_file;

                    global_mtx.lock();
                    run_thread = !file_queue.empty();

                    if (run_thread) {
                        target_file = file_queue.back();
                        file_queue.pop_back();
                    } else {
                        global_mtx.unlock();
                        break;
                    }

                    global_mtx.unlock();

                    std::string outfname(target_file);
                    outfname = outfname.substr(0, outfname.size() - bconst::extension.size());

                    std::ifstream curr_file(target_file, std::ios::binary);

                    if (!curr_file.good()) {
                        global_mtx.lock();
                        std::cout << "The file : " << target_file
                                  << "\n"
                                     "was not decrypted...\n"
                                     "it might be read protected, corrupted or non-existent...\n";
                        global_mtx.unlock();
                    } else {
                        global_mtx.lock();
                        std::cout << "decrypting : " << target_file << "...\n";
                        global_mtx.unlock();

                        curr_file.read(filesig, bconst::FILESIGNATURE.size());

                        if (memcmp(filesig, bconst::FILESIGNATURE.data(), bconst::FILESIGNATURE.size())) {
                            std::cerr << "The file '" << target_file << "' is not encrypted, no need to decrypt it!\n";
                        } else {
                            std::ofstream output_file(outfname, std::ios::binary | std::ios::trunc);
                            output_file.close();
                            output_file.open(outfname, std::ios::binary | std::ios::app);

                            unsigned char *iv = new unsigned char[AES_BLOCKSIZE];
                            curr_file.read(reinterpret_cast<char *>(iv), AES_BLOCKSIZE);

                            while (!curr_file.eof()) {
                                curr_file.read(tbuffer, BUFFER_BYTESIZE);
                                size_t read_buffer_size = curr_file.gcount();

                                if (!curr_file.eof() && read_buffer_size == BUFFER_BYTESIZE) {
                                    for (size_t index = 0; index < read_buffer_size; index += AES_BLOCKSIZE) {
                                        aes_scheme.blockDecrypt(
                                            reinterpret_cast<unsigned char *>(tbuffer + index),
                                            reinterpret_cast<unsigned char *>(decryptedBuffer + index), iv
                                        );
                                    }
                                    output_file.write(reinterpret_cast<char *>(decryptedBuffer), read_buffer_size);
                                } else if (curr_file.eof()) {

                                    if (read_buffer_size) {
                                        size_t remaining_blocks = read_buffer_size / AES_BLOCKSIZE;
                                        size_t index;

                                        for (index = 0; index < remaining_blocks - 1; ++index) {
                                            aes_scheme.blockDecrypt(
                                                reinterpret_cast<unsigned char *>(tbuffer + (index * AES_BLOCKSIZE)),
                                                reinterpret_cast<unsigned char *>(
                                                    decryptedBuffer + (index * AES_BLOCKSIZE)
                                                ),
                                                iv
                                            );
                                        }
                                        output_file.write(
                                            reinterpret_cast<char *>(decryptedBuffer),
                                            (remaining_blocks - 1) * AES_BLOCKSIZE
                                        );

                                        Krypt::ByteArray recover = aes_scheme.decrypt(
                                            reinterpret_cast<unsigned char *>(tbuffer + (index * AES_BLOCKSIZE)),
                                            AES_BLOCKSIZE, iv
                                        );

                                        output_file.write(reinterpret_cast<char *>(recover.array), recover.length);
                                    }
                                } else {
                                    memset((unsigned char *) iv, 0x00, AES_BLOCKSIZE);
                                    memset((char *) tbuffer, 0x00, BUFFER_BYTESIZE);
                                    memset((char *) filesig, 0x00, bconst::FILESIGNATURE.size());
                                    memset((Krypt::Bytes *) decryptedBuffer, 0x00, BUFFER_BYTESIZE);

                                    delete[] iv;
                                    delete[] tbuffer;
                                    delete[] filesig;
                                    delete[] decryptedBuffer;

                                    throw std::logic_error("something wrong happen");
                                }
                            }

                            memset((unsigned char *) iv, 0x00, AES_BLOCKSIZE);
                            delete[] iv;

                            cnt++;
                            checkif_replace(args[COMMAND], target_file);
                        }
                    }
                }

                memset((char *) tbuffer, 0x00, BUFFER_BYTESIZE);
                memset((char *) filesig, 0x00, bconst::FILESIGNATURE.size());
                memset((Krypt::Bytes *) decryptedBuffer, 0x00, BUFFER_BYTESIZE);

                delete[] tbuffer;
                delete[] filesig;
                delete[] decryptedBuffer;
            };

            std::vector<std::thread> threads;

            for (size_t i = 0; i < processor_count - 1; ++i) {
                global_mtx.lock();
                threads.push_back(std::thread(decrypt_lambda));
                global_mtx.unlock();
            }

            decrypt_lambda();

            for (size_t i = 0; i < processor_count - 1; ++i) {
                threads[i].join();
            }

            timing_end("De", start, cnt);
        } else if (!strcmp(args[COMMAND], GENERATE_FLAG)) {
            if (argc == 2) {
                std::cerr << "\n\tERROR: --generate can only be followed by the filename\n";
                std::cerr << "\t         of the key that you want to create and the size\n";
                exit(1);
            } else if (argc == 3) {
                std::cerr << "\n\tERROR: --generate key has no size provided\n";
                exit(1);
            } else if (argc == 4) {
                int keysize = std::atoi(args[KEY + 1]);

                if (keysize < 1) {
                    std::cerr << "\n\tERROR: --generate cannot take a negative keysize\n\n";
                    exit(1);
                }

                keygen::generateKey(args[KEY], keysize);
                std::cout << "success - generated keyfile : " << args[KEY] << "\n";
            } else {
                std::cerr << "\n\tERROR: --generate has too many arguments\n";
                exit(1);
            }
        } else if (!strcmp(args[COMMAND], HELP_FLAG)) {
            std::cerr << "\t" << HELP_FLAG
                      << " menu\n\n"
                         "\tdisplay version:\n\n"
                         "\t\tbethela "
                      << VERSION_FLAG
                      << "\n\n\n"
                         "\tencrypt command format:\n\n"
                         "\t\tbethela "
                      << ENCRYPT_FLAG
                      << " keyfile file1 file2 ... fileN\n\n\n"
                         "\tdecrypt command format:\n\n"
                         "\t\tbethela "
                      << DECRYPT_FLAG
                      << " keyfile file1 file2 ... fileN\n\n"
                         "\t\tAs you can see you can pass 1 or more files to the program\n\n\n"
                         "\tgenerate key format:\n\n"
                         "\t\tbethela "
                      << GENERATE_FLAG
                      << " keyfilename keysize\n\n"
                         "\t\tthe key size should be a positive number greater than 0\n\n\n"
                         "\tadding '-replace' command when encrypting/decrypting will\n"
                         "\treplace the old files with the encrypted/decrypted files.\n\n"
                         "\t\tbethela "
                      << ENCRYPT_FLAG << REPLACE_FLAG
                      << " keyfile file1 file2 ... fileN\n"
                         "\t\tbethela "
                      << DECRYPT_FLAG << REPLACE_FLAG
                      << " keyfile file1 file2 ... fileN\n\n\n"
                         "\talgorithms:\n\n"
                         "\t\tVigenère  "
                      << ENCRYPT_FLAG << " & " << DECRYPT_FLAG
                      << "\n"
                         "\t\tAES256    "
                      << AES_ENCRYPT << "XXX & " << AES_DECRYPT << "XXX\n\n\n";
        } else if (!strcmp(args[COMMAND], VERSION_FLAG)) {
            std::string compiler;
#if defined(__clang__)
            compiler = "clang++";
#elif defined(__GNUC__) || defined(__GNUG__)
            compiler = "g++";
#elif defined(_MSC_VER)
            compiler = "MSC";
#endif
            int executable_bit = sizeof(size_t) == SIZE_T_32BIT ? 32 : 64;
            std::cout << "bethela " << executable_bit << "-bit : " << BETHELA_VERSION << " [" << CRYPTOLIB << "]\n"
                      << "      compiler : " << compiler << "\n\n";
        } else {
            std::cerr << "\n\tinvalid command, type the command below for info:\n\n"
                         "\tbethela --help\n\n";
        }
    } else {
        std::cerr << "A simple terminal command line tool for symmetric\n"
                     "encrpytion & decryption of any files \n\n"
                     "for more info about this program input the command:\n\n\tbethela --help\n\n";
    }
}
