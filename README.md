# bethela

A simple terminal command line tool for symmetric encrpytion & decryption of any files

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
