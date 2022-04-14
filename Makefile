HPP_FILES=$(wildcard src/common/*.cpp)
OBJ_FILES=$(patsubst src/common/%.cpp,build/obj/%.o,$(wildcard src/common/*.cpp))

CC=g++ -flto -O3 -std=c++17 -Wall -Wextra

.SECONDARY:

build/obj/%.o: src/common/%.cpp $(HPP_FILES)
	@mkdir -p $(@D)
	$(CC) -c $< -o $@

build/obj/bin/%.o: src/bin/%.cpp $(HPP_FILES)
	@mkdir -p $(@D)
	$(CC) -c $< -o $@

build/bin/%: build/obj/bin/%.o $(OBJ_FILES)
	@mkdir -p $(@D)
	$(CC) $^ -lglfw -lGL -o $@
