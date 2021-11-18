mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

EXECUTABLE=bethela
INSTALLPATH=/usr/local/bin

OS := $(shell uname)

all:
	@echo "compiling executable..."

ifeq ($(OS), Linux)
	@g++ -static-libgcc -static-libstdc++ -DRELEASE main.cpp -o ${EXECUTABLE} -fopenmp -O3 -march=native
else
	@g++ -static-libgcc -static-libstdc++ -DRELEASE main.cpp -o ${EXECUTABLE}.exe -O3
endif
	@echo "compilation done."

debug_linux:
	@echo "compiling with warnings and fsanitize"
	g++ main.cpp -o ${EXECUTABLE} -Wall -Wextra -O3 -g -fsanitize=address
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