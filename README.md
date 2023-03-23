# bethela

![Ubuntu](https://github.com/mrdcvlsc/bethela/actions/workflows/build-test.yml/badge.svg)

A simple terminal command line tool for symmetric encryption & decryption of any files

## **install in Linux**

<ul>

**Clone and open the repo**

```
git clone --recurse-submodules https://github.com/mrdcvlsc/bethela
git submodule update --init --recursive
cd bethela
```

**Compile with clang and aesni instructions**

```
make compile CXX=clang++ VERSION=aesni TYPE=release
```

**Compile with clang and pure C++ implementation**

```
make compile CXX=clang++ VERSION=portable TYPE=release
```

**Compile with gcc and aesni instructions**

```
make compile CXX=g++ VERSION=aesni TYPE=release
```

**Compile with gcc and pure C++ implementation**

```
make compile CXX=g++ VERSION=portable TYPE=release
```

**Install the program to usr/local/bin (for linux)**

```
make install
```

**Uninstall the program to usr/local/bin (for linux)**

```
make uninstall
```

***If your system supports AES-NI(most modern computers does), use ```make aesni``` instead of just ```make```, this will significantly increase the performance by a very large amount***
  
</ul>

----------------------------------------------------

### encrypt command format:

```
bethela --encrypt keyfile file1 file2 ... fileN
```

<br>

### decrypt command format:

```
bethela --decrypt keyfile file1 file2 ... fileN
```
As you can see you can pass 1 or more files to the program

<br>

### generate key format:

```
bethela --generate keyfilename keysize
```
The **keysize** should be a positive number greater than 0

<br>

### encrypt/decrypt with replace

Adding '-replace' command when encrypting/decrypting will
replace the old files with the encrypted/decrypted files.

```
bethela --encrypt-replace keyfile file1 file2 ... fileN
bethela --decrypt-replace keyfile file1 file2 ... fileN
```

<br>

For more information about using AES and other info about the program use the command below to show the help menu

```
bethela --help
```