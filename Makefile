mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

EXECUTABLE=bethela
INSTALLPATH=/usr/local/bin

OS := $(shell uname)

CXX:=g++
CXX_STANDARD=-std=c++14

ifeq ($(CXX), clang++)
THREADING=
else
THREADING=
endif

TYPE=release
VERSION=portable
LINK=dynamic

ifeq ($(LINK), dynamic)
LINKER=
else ifeq ($(LINK), static)
LINKER=-static
endif

ifeq ($(CXX), clang++)
ADDRESS_SANITIZER=-fsanitize=address
THREADS_SANITIZER=-fsanitize=thread
else
ADDRESS_SANITIZER=
THREADS_SANITIZER=
endif

ifeq ($(TYPE), release)
CXX_FLAGS=-O3
else ifeq ($(TYPE), debug)
CXX_FLAGS=-g -Wall -Wextra $(ADDRESS_SANITIZER)
else ifeq ($(TYPE), debug_threads)
CXX_FLAGS=-g -Wall -Wextra $(THREADS_SANITIZER)
endif

ifeq ($(VERSION), portable)
COMPILATION_MSG="compiling portable version"
VERSION_FLAGS=
else ifeq ($(VERSION), aesni)
COMPILATION_MSG="compiling AES-NI version"
VERSION_FLAGS=-DUSE_AESNI -maes
endif

.PHONY: default environment compile install uninstall encrypt_decrypt randfile checkfile genkeys vig_encrypt_decrypt clean

default:
	@echo "Makefile variables and possible values"
	@echo "The the first element are always the default value"
	@echo "CXX     : g++, clang++"
	@echo "TYPE    : release, debug, debug_threads"
	@echo "VERSION : portable, aesni"
	@echo "LINK    : dynamic, static"
	@echo ""
	@echo "Makefile recipes"

environment:
	@echo "OS : $(OS)"

compile:
	@echo $(COMPILATION_MSG)
	$(CXX) $(LINKER) $(CXX_STANDARD) main.cpp -o $(EXECUTABLE) $(VERSION_FLAGS) $(THREADING) $(CXX_FLAGS)

install:
ifeq ($(OS), Linux)
	@echo "created a symlink in $(INSTALLPATH)"
	@ln -s $(dir $(abspath $(lastword $(MAKEFILE_LIST))))$(EXECUTABLE) $(INSTALLPATH)
else
	@echo "Set It Manually for now"
endif

uninstall:
ifeq ($(OS), Linux)
	@rm $(INSTALLPATH)/$(EXECUTABLE)
else
	@rm ./$(EXECUTABLE)
endif

encrypt_decrypt:
	@echo "Encrypting/Decrypting test files with:"
	@./bethela --version
	@./bethela --enc-AES256-replace AES256.key file0.subject file1.subject file2.subject file3.subject file4.subject file5.subject file6.subject file7.subject file8.subject file9.subject
	@./bethela --dec-AES256-replace AES256.key file0.subject.bthl file1.subject.bthl file2.subject.bthl file3.subject.bthl file4.subject.bthl file5.subject.bthl file6.subject.bthl file7.subject.bthl file8.subject.bthl file9.subject.bthl

randfile:
	@echo "compiling random file generator"
	$(CXX) $(CXX_STANDARD) tests/RandFile.cpp -o RandFile -O3
	@echo "generating random files..."
	@./RandFile

checkfile:
	@echo "compiling equality file checker"
	$(CXX) $(CXX_STANDARD) tests/FileCompare.cpp -o FileCompare -O3
	@echo "checking random files equality..."
	@./FileCompare

genkeys:
	@echo "generate keys"
	@./bethela --generate testkeyVignere1000.key 1000
	@./bethela --generate AES128.key 16
	@./bethela --generate AES192.key 24
	@./bethela --generate AES256.key 32

vig_encrypt_decrypt:
	@echo "encrypting test files..."
	@./bethela --encrypt-replace testkeyVignere1000.key *.subject
	@./bethela --decrypt-replace testkeyVignere1000.key *.bthl

clean:
	@rm *.key
	@rm *.subject
	@rm *.validator
	@rm *.bthl
	@rm FileCompare
	@rm RandFile

simple-test:
	@$(MAKE) compile CXX=clang++ TYPE=release VERSION=aesni LINK=dynamic
	@$(MAKE) genkeys CXX=clang++
	@$(MAKE) randfile CXX=clang++
	@$(MAKE) encrypt_decrypt
	@$(MAKE) checkfile CXX=clang++

temp-linux-test-flow:

	@$(MAKE) compile CXX=clang++ TYPE=debug VERSION=portable
	@$(MAKE) genkeys CXX=clang++
	@$(MAKE) randfile CXX=clang++
	@$(MAKE) encrypt_decrypt
	@$(MAKE) checkfile CXX=clang++
	@$(MAKE) vig_encrypt_decrypt
	@./FileCompare
	@$(MAKE) compile CXX=clang++ TYPE=debug_threads VERSION=portable
	@$(MAKE) encrypt_decrypt
	@./FileCompare
	@$(MAKE) vig_encrypt_decrypt
	@./FileCompare
	@$(MAKE) compile CXX=clang++ TYPE=debug VERSION=aesni
	@$(MAKE) encrypt_decrypt
	@./FileCompare
	@$(MAKE) vig_encrypt_decrypt
	@./FileCompare
	@$(MAKE) compile CXX=clang++ TYPE=debug_threads VERSION=aesni
	@$(MAKE) encrypt_decrypt
	@./FileCompare
	@$(MAKE) vig_encrypt_decrypt
	@./FileCompare
