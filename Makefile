NAME=bordertanks
SOURCE=main.cpp font.cpp
OS=$(shell uname -s)

ifneq ($(OS),Linux)
	CFLAGS+=-lmingw32 -Dmain=SDL_main -lSDL2main
	PROG=$(NAME).exe
else
	PROG=$(NAME)
endif
CFLAGS+=-Wall -lSDL2 -lSDL2_image

all:
	$(CXX) $(SOURCE) -o $(PROG) $(CFLAGS)
clean:
	$(RM) $(PROG)
