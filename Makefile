CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-variable -ggdb -O0

SRC = args.c  function.c  integrator.c  main.c  object.c
OBJ = $(patsubst %.c, %.o, $(SRC))
EXE = funcs.exe

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf external-functions-shared-object.* $(OBJ) $(EXE)
