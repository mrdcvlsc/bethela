# bethela

![ubuntu](https://github.com/mrdcvlsc/bethela/actions/workflows/build-test.yml/badge.svg)

A simple terminal command line tool for symmetric encrpytion & decryption of any files

### install in linux

<ul>

```
wget https://github.com/mrdcvlsc/bethela/archive/refs/tags/v3.4.4.tar.gz
tar -xf v3.4.4..tar.gz
cd bethela-3.4.4
make
make install
```
or
```
git clone https://github.com/mrdcvlsc/bethela.git
cd bethela
make
make install
```

***If you have crypto++ in your system you can use it instead of the portable AES, to do this in the installation replace ```make``` with ```make cryptopp```***

***If your system supports AES-NI(most modern coputer does), use ```make aesni```***
  
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
the **keysize** should be a positive number greater than 0

<br>

### encrypt/decrypt with replace

adding '-replace' command when encrypting/decrypting will
replace the old files with the encrypted/decrypted files.

```
bethela --encrypt-replace keyfile file1 file2 ... fileN
bethela --decrypt-replace keyfile file1 file2 ... fileN
```

<br>

for more information about using AES and other info about the program use the command below to show the help menu

```
bethela --help
```