CXX = g++
CFLAGS = $(shell sdl2-config --cflags)
LFLAGS = -Wall $(shell sdl2-config --libs) -lSDL2_image

ifeq ($(RELEASE), yes)
	LFLAGS+=-O2
endif
ifeq ($(DEBUG), yes)
	LFLAGS += -ggdb -g3 -pg -O0
endif

target_file  := main
prog_name    := bordertanks
dest_dir     := ./src/
object_files := $(patsubst %.cpp, %.o, $(wildcard $(dest_dir)*.cpp))

all: $(dest_dir)$(target_file)

$(dest_dir)$(target_file): $(object_files)
	$(CXX) $(object_files) -o $(prog_name) $(LFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) $(prog_name) $(object_files)
