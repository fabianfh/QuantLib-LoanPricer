CC = g++
BUILD_DIR = ./build
Flags = -g

all : createBuildDir  $(BUILD_DIR)/OISwapTest.o $(BUILD_DIR)/LiborSwapTest.o


$(BUILD_DIR)/OISwapTest.o:OISwapTest.cpp OISwapTest.hpp  
	$(CC) $(Flags) -c  OISwapTest.cpp   -L/usr/local/lib -lQuantLib -o $(BUILD_DIR)/OISwapTest.o

$(BUILD_DIR)/LiborSwapTest.o:LiborSwapTest.cpp LiborSwapTest.hpp  
	$(CC) $(Flags) -c  LiborSwapTest.cpp   -L/usr/local/lib -lQuantLib -o $(BUILD_DIR)/LiborSwapTest.o

clean :
	rm -rf $(BUILD_DIR)

createBuildDir:  
	mkdir -p $(BUILD_DIR)