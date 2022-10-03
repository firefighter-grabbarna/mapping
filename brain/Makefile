HPP_FILES=$(wildcard common/*.hpp)
OBJ_FILES=$(patsubst %.cpp,build/%.o,$(wildcard common/*.cpp))

CC=g++ -flto -O3 -std=c++17 -Wall -Wextra

.SECONDARY:
.PHONY: all clean

build/obj/%.o: src/common/%.cpp $(HPP_FILES)
	@echo "cc $<"
	@mkdir -p $(@D)
	@$(CC) -c $< -o $@

build/obj/bin/%.o: src/bin/%.cpp $(HPP_FILES)
	@echo "cc $<"
	@mkdir -p $(@D)
	@$(CC) -c $< -o $@

build/bin/%: build/obj/bin/%.o $(OBJ_FILES)
	@echo "ld $@"
	@mkdir -p $(@D)
	@$(CC) $^ -lglfw -lGL -o $@

all: build/bin/display build/bin/robot build/bin/simulation build/bin/stop

clean:
	@[ -d build ] && rm -r build || :
