OUT = Pong
CXX = g++
SDL = -lSDL2 -lSDL2_image -lSDL2_ttf

OBJECTS = obj/main.o

all: $(OUT)
$(OUT): $(OBJECTS)
	$(CXX) -o bin/$@ $^ ${SDL}

