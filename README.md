# bethela - CLI file cryptography

![Ubuntu](https://github.com/mrdcvlsc/bethela/actions/workflows/test.yml/badge.svg)

A simple terminal command line tool for encryption & decryption of any files, currently supports `Vigenere`, `AES128-CBC`, `AES192-CBC` and `AES256-CBC` ciphers.

The program can also use hardware acceleration technology for AES ciphers using `AES-NI` (Intel and AMD CPUs) and `ARM-neon` (armv8, aarch64 CPUs) intrinsics.

## **Install in Linux**

- Requirement:
    - **make**
    - **compiler that can compile with or above C++17**

1. **Clone** the repo

    ```
    git clone --recurse-submodules https://github.com/mrdcvlsc/bethela
    git submodule update --init --recursive
    cd bethela
    ```

2. **Compile** the program

    Example:
    ```
    make compile CXX=clang++ VERSION=aesni TYPE=release
    ```

    This table shows what values you can pass to a dflag
    during compilation to alter the compilation behaviour.

    If a variable flag is not specified, the first value in 
    the table below are taken as the default values.

    | Variable    | Value                               |
    | ----------- | ----------------------------------- | 
    | **CXX**     | `g++`, `clang++`                    |
    | **TYPE**    | `release`, `debug`, `debug_threads` |
    | **VERSION** | `portable`, `aesni`, `neon`         |
    | **LINK**    | `dynamic`, `static`                 |

    **aesni** - will enable hardware acceleration technology
    `AES-NI` when compiling for x86-64 Intel and/or AMD CPUs.

    **neon** - will enable hardware acceleration technology
    in ARM when compiling for ARM armv8/aarch64 CPUs.

3. **Install** the program to usr/local/bin (**for linux**)

    ```
    make install
    ```

    To **Uninstall** the program in usr/local/bin (**for linux**), use
    the command below

    ```
    make uninstall
    ```

-----

### **Generate a key file:**

```cmd
bethela --generate key_filename key_size_in_bytes
```

The **key size** should be a positive number greater than 0

-----

### **Encrypt/Decrypt with replace**

Adding `-replace` command when encrypting/decrypting will
replace the old files with the encrypted/decrypted files.

-----

### **Vigenere Cipher**
Requires any key file sizes.
```
bethela --encrypt-replace keyfile file1 file2 ... fileN
bethela --decrypt-replace keyfile file1.bthl file2.bthl ... fileN.bthl
```

-----

### **AES-128**
Requires a 128-bit key, or 16 byte key file size.
```
bethela --enc-AES128-replace keyfile file1 file2 ... fileN
bethela --dec-AES128-replace keyfile file1.bthl file2.bthl ... fileN.bthl
```

-----

### **AES-192**
Requires a 192-bit key, or 24 byte key file size.
```
bethela --enc-AES192-replace keyfile file1 file2 ... fileN
bethela --dec-AES192-replace keyfile file1.bthl file2.bthl ... fileN.bthl
```

-----

### **AES-256**
Requires a 256-bit key, or 32 byte key file size.
```
bethela --enc-AES256-replace keyfile file1 file2 ... fileN
bethela --dec-AES256-replace keyfile file1.bthl file2.bthl ... fileN.bthl
```

-----

###  **Help flag**
```
bethela --help
```
