CC = g++
BUILD_DIR = ./build
Flags = -g

all : createBuildDir $(BUILD_DIR)/MarketConvention.o $(BUILD_DIR)/MarketData.o

$(BUILD_DIR)/MarketConvention.o: MarketConvention.cpp MarketConvention.hpp
	$(CC) $(Flags) -c MarketConvention.cpp  -L/usr/local/lib -lQuantLib -o $(BUILD_DIR)/MarketConvention.o

$(BUILD_DIR)/MarketData.o: MarketData.cpp MarketData.hpp
	$(CC) $(Flags) -c MarketData.cpp  -L/usr/local/lib -lQuantLib -o $(BUILD_DIR)/MarketData.o

clean :
	rm -rf $(BUILD_DIR)

createBuildDir:  
	mkdir -p $(BUILD_DIR)
	cp ./data/*.xml $(BUILD_DIR) 





