CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-variable -ggdb -O0
# CFLAGS = -O3

SRC = main.c args.c  function.c  integrator.c  object.c  plot.c gamma2.c
OBJ = $(patsubst %.c, %.o, $(SRC))

EXE = funcs.exe

ifeq ($(OS), Windows_NT)
	LIB = -L lib/raylib-5.0_win64_mingw-w64/lib -lraylib -lgdi32 -lwinmm
	INC = -I lib/raylib-5.0_win64_mingw-w64/include
else
	LIB = -L lib/raylib-5.0_linux_amd64/lib -l:libraylib.a -lm
	INC = -I lib/raylib-5.0_linux_amd64/include
endif

INC += -I lib/Crossline-1.0/
SRC += lib/Crossline-1.0/crossline.c

all: $(EXE)

$(EXE): $(OBJ) $(OBJ_EXTENDED)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC)

clean:
	rm -rf external-functions-shared-object.* $(OBJ) $(EXE)
