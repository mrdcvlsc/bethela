# bethela

A simple terminal command line tool for symmetric encrpytion & decryption of any files, this program uses [**Vigenère cipher**](https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher) on the target file's bytestream 

----------------------------------------------------

### encrypt command format:

```
bethela --encrypt keyfile file1 file2 ... fileN
```

### decrypt command format:

```
bethela --decrypt keyfile file1 file2 ... fileN
```
As you can see you can pass 1 or more files to the program


### generate key format:

```
bethela --generate keyfilename keysize
```
the **keysize** should be a positive number greater than 0
