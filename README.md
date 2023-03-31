# bethela

![Ubuntu](https://github.com/mrdcvlsc/bethela/actions/workflows/build-test.yml/badge.svg)

## **Compiles with C++14**

A simple terminal command line tool for symmetric encryption & decryption of any files

## **install in Linux**

- **Clone** and the repo

    ```
    git clone --recurse-submodules https://github.com/mrdcvlsc/bethela
    git submodule update --init --recursive
    cd bethela
    ```

- **Makefile variables**

    If not specified the first values are the default values.

    | Variable    | Value                               |
    | ----------- | ----------------------------------- | 
    | **CXX**     | `g++`, `clang++`                    |
    | **TYPE**    | `release`, `debug`, `debug_threads` |
    | **VERSION** | `portable`, `aesni`, `neon`         |
    | **LINK**    | `dynamic`, `static`                 |

- Example **compilation** with clang and aesni instructions

    ```
    make compile CXX=clang++ VERSION=aesni TYPE=release
    ```

- Example **compilation** with g++ and portable C++ code

    I don't need to specify other values because here the target
    values are already the default values.

    ```
    make compile
    ```

- **Install** the program to usr/local/bin (for linux)

    ```
    make install
    ```

- **Uninstall** the program to usr/local/bin (for linux)

    ```
    make uninstall
    ```

----------------------------------------------------

- **encrypt command format:**

    ```
    bethela --encrypt keyfile file1 file2 ... fileN
    ```

<br>

- **decrypt command format:**

    ```
    bethela --decrypt keyfile file1 file2 ... fileN
    ```
    As you can see you can pass 1 or more files to the program

<br>

- **generate key format:**

    ```
    bethela --generate keyfilename keysize
    ```
    The **keysize** should be a positive number greater than 0

<br>

- **encrypt/decrypt with replace**

    Adding '-replace' command when encrypting/decrypting will
    replace the old files with the encrypted/decrypted files.

    ```
    bethela --encrypt-replace keyfile file1 file2 ... fileN
    
    bethela --decrypt-replace keyfile file1 file2 ... fileN
    ```

<br>

For more information about using **AES** and other info about the program use the command below to show the help menu

```
bethela --help
```
