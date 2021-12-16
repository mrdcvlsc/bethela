mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

EXECUTABLE=bethela
INSTALLPATH=/usr/local/bin

OS := $(shell uname)

all:
	@echo "compiling portable version..."

ifeq ($(OS), Linux)
	@g++ -static-libgcc -static-libstdc++ -DRELEASE main.cpp -o ${EXECUTABLE} -fopenmp -O3 -march=native
else
	@echo "for windows the build is not multi-threaded, you need to set it up on your own for now"
	@g++ -static-libgcc -static-libstdc++ -DRELEASE main.cpp -o ${EXECUTABLE}.exe -O3
endif
	@echo "compilation done."

cryptopp:
	@echo "compiliing executable with cryptopp"
	@echo "WARNNING: The portable version of bethela is not compatible with the cryptopp version"
	@g++ -static-libgcc -static-libstdc++ -DUSE_CRYPTOPP -DRELEASE main.cpp -o ${EXECUTABLE} -lcryptopp -fopenmp -O3 -march=native

cryptopp_debug:
	@echo "compiliing executable with cryptopp"
	@g++ -static-libgcc -static-libstdc++ -DUSE_CRYPTOPP -DRELEASE main.cpp -o ${EXECUTABLE} -g -lcryptopp -fsanitize=address -fopenmp -O0 -Wall -Wextra


debug_linux:
	@echo "compiling with warnings and fsanitize"
	g++ main.cpp -o ${EXECUTABLE} -Wall -Wextra -Og -fopenmp -g -fsanitize=address
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