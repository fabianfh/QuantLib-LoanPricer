CC = g++
BUILD_DIR = ./build
Flags = -g

all : createBuildDir $(BUILD_DIR)/Utils.o $(BUILD_DIR)/BootStrapper.o

$(BUILD_DIR)/Utils.o: Utils.cpp Utils.hpp
	$(CC) $(Flags) -c  Utils.cpp  -L/usr/local/lib -lQuantLib -o $(BUILD_DIR)/Utils.o
$(BUILD_DIR)/BootStrapper.o:BootStrapper.cpp BootStrapper.hpp  Utils.hpp  Utils.cpp
	$(CC) $(Flags) -c  BootStrapper.cpp   -L/usr/local/lib -lQuantLib -o $(BUILD_DIR)/BootStrapper.o

clean :
	rm -rf $(BUILD_DIR)

createBuildDir:  
	mkdir -p $(BUILD_DIR)