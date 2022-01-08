CC = g++
BUILD_DIR = ./build
COPY_FILES = cp *.xml $(BUILD_DIR)
Flags = -g

all: createBuildDir $(BUILD_DIR)/Main 


$(BUILD_DIR)/Main: Main.cpp MarketData.o MarketConvention.o BootStrapper.o
	$(CC) $(Flags) Main.cpp Market/MarketData.hpp $(BUILD_DIR)/MarketData.o Market/MarketConvention.hpp $(BUILD_DIR)/MarketConvention.o BootStrapper/BootStrapper.hpp $(BUILD_DIR)/BootStrapper.o -L/usr/local/lib -lQuantLib -o $(BUILD_DIR)/Main

MarketData.o:
	cp ./Market/$(BUILD_DIR)/MarketData.o $(BUILD_DIR)

MarketConvention.o:
	cp ./Market/$(BUILD_DIR)/MarketConvention.o $(BUILD_DIR)

BootStrapper.o:
	cp ./BootStrapper/$(BUILD_DIR)/BootStrapper.o $(BUILD_DIR)

createBuildDir:  
	mkdir -p $(BUILD_DIR)
	cp ./Market/build/*.xml $(BUILD_DIR) 

clean :
	rm -rf $(BUILD_DIR)

