SRC = src
OUTPUT = chipper
PARAMS = -g -O2

# ADD SDL PATHS HERE
SDL_DEVELOPMENT_INC :=
SDL_DEVELOPMENT_DIR :=

INC = -I$(SDL_DEVELOPMENT_INC)
LIB = -L$(SDL_DEVELOPMENT_DIR)

all:
	g++ $(PARAMS) $(INC) $(wildcard $(SRC)/*.cpp) -o $(OUTPUT) $(LIB) -lSDL2 -lSDL2main
	
clean:
	rm chipper.exe