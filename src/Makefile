BIN=psh

OBJ_DIR=.objects

CC=gcc
CC_FLAGS=-Wall -c

output: $(BIN) objDir

$(BIN): objDir main.o task.o sighand.o builtin.o
	@ $(CC) -o $(BIN) $(OBJ_DIR)/main.o $(OBJ_DIR)/sighand.o $(OBJ_DIR)/builtin.o

main.o: main.c
	@ $(CC) $(CC_FLAGS) -o $(OBJ_DIR)/main.o main.c

task.o: task.c
	@ $(CC) $(CC_FLAGS) -o $(OBJ_DIR)/task.o task.c

sighand.o: sighand.c
	@ $(CC) $(CC_FLAGS) -o $(OBJ_DIR)/sighand.o sighand.c

builtin.o: builtin.c
	@ $(CC) $(CC_FLAGS) -o $(OBJ_DIR)/builtin.o builtin.c

objDir:
	@ mkdir -p $(OBJ_DIR)

clean:
	@ rm -rf $(OBJ_DIR)/* $(BIN)
	@ rm -rf $(OBJ_DIR)
