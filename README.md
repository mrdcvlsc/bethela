# imgcrypt

### COMPILATION
    - dependencies : OpenCV v3+ (above)
    - cpp standard : -std=c++17

## EXAMPLE ENCRYPTION

### Folder Structure
- images (the folder containing all images you want to work on)
    - test1.png
    - test2.jpg
    - testLock.key (a generated key)
    - encrypt (a subfolder that will contain all the encrypted images)

### RUNNING THE PROGRAM

```
./img_crypt.o
image$encryption : ~/Documents/images/ ENCRYPT testLock.key
```

_for decryption just change the 'ENCRYPT' command to 'DECRYPT', and change the subfolder 'encrypt' to 'decrypt'_

### OUTPUT

- images
    - test1.png
    - test2.jpg
    - testLock.key
    - encrypt
        - 0.png
        - 1.png

### COMMAND INPUT FORMAT

- Encryption ```image$encryption : DIRECTORY ENCRYPT KEYFILE```

- Decryption ```image$encryption : DIRECTORY DECRYPT KEYFILE```

- GENERATE KEY ```image$encryption : DIRECTORY GENERATE KEYFILE SIZE```

*a folder named ```encrypt```/```decrypt``` should exist first inside the directory given before **encrypting/decrypting**. the ```keyfile``` should also be inside the given ```directory```. when generating a new keyfile it will be stored in the given directory*

