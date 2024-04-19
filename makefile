CXX = clang++

# compiler flags for reference
# -c        = Compile and assemble, but do not link
# -D        = Define a preprocessor macro
# -E        = Stop after the preprocessing stage; do not compile or link
# -fpic     = Generate position independent code, typically used for creating shared libraries
# -fPIC     = Same as -fpic but generates code addressing larger offsets
# -fopenmp  = Enable parallel computing via OpenMP
# -g        = Generate debug information, useful for debugging with tools like gdb
# -I        = Include directory, tells compiler where to look for header files
# -l        = Link against a library (followed by library name, eg. -lpthread for POSIX threads library)
# -lm       = Link against the math library
# -o        = Name the binary output file produced by the compiler
# -O2       = Enables optimizations that increase execution speed without code size trade-off
# -O3       = Enables more optimizations that focus on increasing execution speed, even at the cost of code size
# -S        = Stop after compiling; do not assemble or link
# -shared   = Create a shared library
# -std      = Specify the C++ standard version (eg. -std=c++11, -std=c++14, -std=c++17, etc.)
# -v        = Verbose mode, to see each action performed by the compiler during the build
# -Wall     = Enable most compiler warnings, for better code practices
# -Werror   = Turn all warnings into errors, enforcing clean code practices
# -pedantic = Emit warnings dictated by strict ISO compliance (useful in portable codebases)
CXXFLAGS = -O3 -std=c++17 -Wno-deprecated-declarations

# This recursive wildcard function can help you grab files from subdirectories
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# Find all .cpp files in src directory recursively
CPP_FILES = $(call rwildcard, src/, *.cpp)

# output file name
TARGET = game_engine_linux

# included headers path
HEADERS = -I ./dependencies/ -I ./dependencies/glm -I ./dependencies/rapidjson -I ./dependencies/SDL2 -I ./dependencies/SDL2_image -I ./dependencies/SDL2_ttf -I ./dependencies/SDL2_mixer/ -I ./lua -I ./dependencies/LuaBridge

# included binary libraries
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.4

main:
	$(CXX) $(CXXFLAGS) $(HEADERS) -o $(TARGET) $(CPP_FILES) $(LDFLAGS)

pedant:
	$(CXX) $(CXXFLAGS) -Wall -Werror -pedantic $(HEADERS) -o $(TARGET) $(CPP_FILES) $(LDFLAGS)