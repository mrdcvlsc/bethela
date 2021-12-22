mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

EXECUTABLE=bethela
INSTALLPATH=/usr/local/bin

OS := $(shell uname)

CC=g++

all:
	@echo "compiling portable version..."

ifeq ($(OS), Linux)
	@$(CC) -static-libgcc -static-libstdc++ -DRELEASE main.cpp -o ${EXECUTABLE} -fopenmp -O3 -march=native
else
	@echo "for windows the build is not multi-threaded, you need to set it up on your own for now"
	@$(CC) -static-libgcc -static-libstdc++ -DRELEASE main.cpp -o ${EXECUTABLE}.exe -O3
endif
	@echo "compilation done."

cryptopp:
	@echo "compiliing executable with cryptopp"
	@echo "WARNNING: The portable version of bethela is not compatible with the cryptopp version"
	@$(CC) -static-libgcc -static-libstdc++ -DUSE_CRYPTOPP -DRELEASE main.cpp -o ${EXECUTABLE} -lcryptopp -fopenmp -O3 -march=native

cryptopp_debug:
	@echo "compiliing executable with cryptopp"
	@$(CC) -static-libgcc -static-libstdc++ -DUSE_CRYPTOPP -DRELEASE main.cpp -o ${EXECUTABLE} -g -lcryptopp -fsanitize=address -fopenmp -O0 -Wall -Wextra

debug_linux:
	@echo "compiling with warnings and fsanitize"
	$(CC) main.cpp -o ${EXECUTABLE} -Wall -Wextra -O0 -fopenmp -g -fsanitize=address
	@echo "compiling done"

install:
ifeq ($(OS), Linux)
	@echo "created a symlink in ${INSTALLPATH}"
	@ln -s $(dir $(abspath $(lastword $(MAKEFILE_LIST))))${EXECUTABLE} ${INSTALLPATH}
else
	@echo "Set It Manually for now"
endif

uninstall:
ifeq ($(OS), Linux)
	@rm ${INSTALLPATH}/${EXECUTABLE}
else
	@rm ./${EXECUTABLE}.exe
endif

# TEST SCRIPTS

test:
	@echo "-----------------------------------------------------------"
	@echo "====================   GENERATE KEYS   ===================="
	@echo "-----------------------------------------------------------"
	@make genkeys
	@echo "-----------------------------------------------------------"
	@echo "=================   GENERATE TEST FILES   ================="
	@echo "-----------------------------------------------------------"
	@make randfile
	@echo "-----------------------------------------------------------"
	@echo "===============   COMPLING PORTABLE AES  =================="
	@echo "-----------------------------------------------------------"
	@make
	@echo "-----------------------------------------------------------"
	@echo "===============   TESTING PORTABLE AES  ==================="
	@echo "-----------------------------------------------------------"
	@./bethela --version
	@make aestest
	@echo "-----------------------------------------------------------"
	@echo "===============   COMPILING CRYPTOPP AES  ================="
	@echo "-----------------------------------------------------------"
	@make cryptopp
	@echo "-----------------------------------------------------------"
	@echo "===============   TESTING CRYPTOPP AES  ==================="
	@echo "-----------------------------------------------------------"
	@./bethela --version
	@make aestest
	@echo "-----------------------------------------------------------"
	@echo "=================   TESTING VIGENERE   ===================="
	@echo "-----------------------------------------------------------"
	@make vigtest
	@echo "-----------------------------------------------------------"
	@echo "=================   CLEANING TEST FILES   ================="
	@echo "-----------------------------------------------------------"
	@make clean
	@echo "-----------------------------------------------------------"
	@echo "=================   ALL TEST SUCCESS!!!   ================="
	@echo "-----------------------------------------------------------"

install_cryptoppp:
	@echo "-----------------------------------------------------------"
	@echo "=================   INSTALL CRYPTOPP   ===================="
	@echo "-----------------------------------------------------------"
	@sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils

aestest:
	@./bethela --enc-AES256-replace tests/AES256.key tests/*.subject
	@./bethela --dec-AES256-replace tests/AES256.key tests/*.bthl
	@make checkfile

randfile:
	@echo "compiling random file generator"
	@$(CC) tests/RandFile.cpp -o tests/RandFile -fsanitize=address
	@echo "generating random files..."
	@cd tests && ./RandFile

checkfile:
	@echo "compiling equality file checker"
	@$(CC) tests/FileCompare.cpp -o tests/FileCompare -fsanitize=address
	@echo "checking random files equality..."
	@cd tests && ./FileCompare

genkeys:
	@echo "generate keys"
	@./bethela --generate tests/testkeyVignere1000.key 1000
	@./bethela --generate tests/AES128.key 16
	@./bethela --generate tests/AES192.key 24
	@./bethela --generate tests/AES256.key 32

vigtest:
	@echo "encrypting test files..."
	@./bethela --encrypt-replace tests/testkeyVignere1000.key tests/*.subject
	@./bethela --decrypt-replace tests/testkeyVignere1000.key tests/*.bthl
	@make checkfile

clean:
	@rm tests/*.key
	@rm tests/*.subject
	@rm tests/*.validator