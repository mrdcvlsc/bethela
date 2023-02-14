mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

EXECUTABLE=bethela
INSTALLPATH=/usr/local/bin

OS := $(shell uname)

CXX:=g++

ifeq ($(CXX), clang++)
THREADING=-fopenmp
else
THREADING=-fopenmp
endif

all:
	@echo "compiling portable version..."
ifeq ($(OS), Linux)
	$(CXX) -static-libgcc -static-libstdc++ $(THREADING) -DRELEASE main.cpp -O3 -march=native -o ${EXECUTABLE}
else
	@echo "for windows the build is not multi-threaded, you need to set it up on your own for now"
	@$(CXX) -static-libgcc -static-libstdc++ -DRELEASE main.cpp -o ${EXECUTABLE}.exe -O3
endif
	@echo "compilation done."

aesni:
	@echo "compiling with AES-NI support"
ifeq ($(OS), Linux)
	$(CXX) -static-libgcc -static-libstdc++ $(THREADING) -DRELEASE main.cpp -DUSE_AESNI -maes -O3 -march=native -o ${EXECUTABLE}
else
	@echo "for windows the build is not multi-threaded, you need to set it up on your own for now"
	$(CXX) -static-libgcc -static-libstdc++ main.cpp -DRELEASE -DUSE_AESNI -maes -o ${EXECUTABLE}.exe -O3
endif
	@echo "compilation done."

aesni_debug:
	@echo "compiling with AES-NI-debug support"
ifeq ($(OS), Linux)
	$(CXX) -g -DRELEASE main.cpp -DUSE_AESNI -maes -Og -march=native -o ${EXECUTABLE} -fsanitize=address
else
	@echo "for windows the build is not multi-threaded, you need to set it up on your own for now"
	$(CXX) -g main.cpp -DRELEASE -DUSE_AESNI -maes -o ${EXECUTABLE}.exe -O0 -fsanitize=address
endif
	@echo "compilation done."

# cryptopp:
# 	@echo "compiliing executable with cryptopp"
# 	@$(CXX) -static-libgcc -static-libstdc++ -DUSE_CRYPTOPP -DRELEASE main.cpp -o ${EXECUTABLE} -lcryptopp $(THREADING) -O3 -march=native

# cryptopp_debug:
# 	@echo "compiliing executable with cryptopp"
# 	@$(CXX) -static-libgcc -static-libstdc++ -DUSE_CRYPTOPP -DRELEASE main.cpp -o ${EXECUTABLE} -g -lcryptopp -fsanitize=address $(THREADING) -O0 -Wall -Wextra

debug_linux:
	@echo "compiling with warnings and fsanitize"
	$(CXX) -g main.cpp -o ${EXECUTABLE} -Wall -Wextra -Og -fsanitize=address
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
	$(shell mkdir -p cryptos/Krypt/bin)
	@echo "-----------------------------------------------------------"
	@echo "==============  Krypt : compiling TEST  ==================="
	@echo "-----------------------------------------------------------"
	@$(MAKE) -C cryptos/Krypt compile_test
	@echo "-----------------------------------------------------------"
	@echo "===============  Krypt : running TEST  ===================="
	@echo "-----------------------------------------------------------"
	@$(MAKE) -C cryptos/Krypt run_test
	@echo "Krypt tests done!!!"
	@echo "..."
	@echo "..."
	@echo "entered bethela test..."
	@make
	@echo "-----------------------------------------------------------"
	@echo "====================   GENERATE KEYS   ===================="
	@echo "-----------------------------------------------------------"
	@make genkeys
	@echo "-----------------------------------------------------------"
	@echo "=================   GENERATE TEST FILES   ================="
	@echo "-----------------------------------------------------------"
	@make randfile

	@echo "-----------------------------------------------------------"
	@echo "===============   COMPILING AES-NI AES   ================="
	@echo "-----------------------------------------------------------"
	@make aesni_debug
	@echo "-----------------------------------------------------------"
	@echo "===============   TESTING AES-NI AES   ===================="
	@echo "-----------------------------------------------------------"
	@./bethela --version
	@make aestest	
	
	@echo "-----------------------------------------------------------"
	@echo "===============   COMPLING PORTABLE AES  =================="
	@echo "-----------------------------------------------------------"
	@make debug_linux
	@echo "-----------------------------------------------------------"
	@echo "===============   TESTING PORTABLE AES  ==================="
	@echo "-----------------------------------------------------------"
	@./bethela --version
	@make aestest
# @echo "-----------------------------------------------------------"
# @echo "===============   COMPILING CRYPTOPP AES  ================="
# @echo "-----------------------------------------------------------"
# @make cryptopp
# @echo "-----------------------------------------------------------"
# @echo "===============   TESTING CRYPTOPP AES  ==================="
# @echo "-----------------------------------------------------------"
# @./bethela --version
# @make aestest

	@echo "-----------------------------------------------------------"
	@echo "=================   TESTING VIGENERE   ===================="
	@echo "-----------------------------------------------------------"
	@make vigtest
	@echo "-----------------------------------------------------------"
	@echo "=================   CLEANING TEST FILES   ================="
	@echo "-----------------------------------------------------------"
	@make clean
	@echo "-----------------------------------------------------------"
	@echo "=================   ALL TEST SUCXXESS!!!   ================="
	@echo "-----------------------------------------------------------"

install_cryptoppp:
	@echo "-----------------------------------------------------------"
	@echo "=================   INSTALL CRYPTOPP   ===================="
	@echo "-----------------------------------------------------------"
	@sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils

aestest:
	@echo "Validating generated test files first..."
	@make checkfile
	@./bethela --enc-AES256-replace tests/AES256.key tests/*.subject
	@./bethela --dec-AES256-replace tests/AES256.key tests/*.bthl
	@echo "Checking Output Files For Real..."
	@make checkfile

randfile:
	@echo "compiling random file generator"
	@$(CXX) tests/RandFile.cpp -o tests/RandFile -O3
	@echo "generating random files..."
	@cd tests && ./RandFile

checkfile:
	@echo "compiling equality file checker"
	@$(CXX) tests/FileCompare.cpp -o tests/FileCompare -O3
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
