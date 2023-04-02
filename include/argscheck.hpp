#ifndef BETHELA_ARGS_CHECK_HPP
#define BETHELA_ARGS_CHECK_HPP

#include <iostream>

bool match(std::string const& input_arg, std::string const& command_flag) {
    for (size_t i = 0; i < command_flag.size() && i < input_arg.size(); ++i) {
        if (input_arg[i] != command_flag[i]) {
            return false;
        }
    }
    
    return true;
}

void checkif_replace(std::string const& input_arg, std::string const& file_to_replace) {
    size_t found = input_arg.find("-replace");
    
    if (found != std::string::npos) {
        int file_not_removed = std::remove(file_to_replace.data());

        if (file_not_removed) {
            std::cout << "notice: The program cannot remove the file '" << file_to_replace << "'\n"
                      << "        file location might be protected for delete actions.\n";
        }
    }
}

void emptyFileArgs(char cmd[10], int argcnt) {
    if (argcnt == 2) {
        std::cerr << "\n\tERROR: command " << cmd << "\n";
        std::cerr << "\t       no key provided\n";
        exit(1);
    } else if (argcnt == 3) {
        std::cerr << "\n\tERROR: command " << cmd << "\n";
        std::cerr << "\t       no files provided\n";
        exit(1);
    }
}

size_t check_aes_arg(std::string const& aes_input_arg) {
    std::string aes_arg(aes_input_arg);

    if (!((aes_arg.size() != 12) ^ (aes_arg.size() != 20))) {
        std::cerr << "\n\tERROR: AES key length invalid\n";
        std::cerr << "\t       from arg " << aes_input_arg << "\n";
        exit(1);
    }

    std::string str_key_size;

    str_key_size.push_back(aes_input_arg[9]);
    str_key_size.push_back(aes_input_arg[10]);
    str_key_size.push_back(aes_input_arg[11]);

    size_t key_size = std::atol(str_key_size.data());

    if (!(key_size == 128 || key_size == 192 || key_size == 256)) {
        std::cerr << "\n\tERROR: invalid AES command given\n";
        std::cout << "\t\t : " << key_size << "\n";
        exit(1);
    }

    std::cerr << "Key length : " << key_size << "\n";

    return key_size;
}

#endif