mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

EXECUTABLE=bethela
INSTALLPATH=/usr/local/bin

OS := $(shell uname)

all:
	@echo "compiling executable..."

ifeq ($(OS), Linux)
	@g++ -static-libgcc -static-libstdc++ main.cpp -o ${EXECUTABLE} -O3
else
	@g++ -static-libgcc -static-libstdc++ main.cpp -o ${EXECUTABLE}.exe -O3
endif
	@echo "compilation done."

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