BIN=psh

OBJ_DIR=.objects

CC=gcc
CC_FLAGS=-Wall -c

output: $(BIN) objDir

$(BIN): objDir main.o
	@ $(CC) -o $(BIN) $(OBJ_DIR)/main.o

main.o: main.c
	@ $(CC) $(CC_FLAGS) -o $(OBJ_DIR)/main.o main.c

objDir:
	@ mkdir -p $(OBJ_DIR)

clean:
	@ rm -rf $(OBJ_DIR)/* $(BIN)
	@ rm -rf $(OBJ_DIR)
