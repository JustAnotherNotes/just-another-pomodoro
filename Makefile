CC = gcc
CFLAGS = -g -Wall $(shell pkg-config --cflags libnotify)
LDFLAGS = $(shell pkg-config --libs libnotify)

SRC_D = src
OBJ_D = obj
BIN_D = bin

SRCS = $(wildcard $(SRC_D)/*.c)
OBJS = $(patsubst $(SRC_D)/%.c, $(OBJ_D)/%.o, $(SRCS))

BIN = $(BIN_D)/jap

all: $(BIN)

release: CFLAGS = -O2 -DNDEBUG
release: clean
release: $(BIN)

$(BIN): $(OBJS) | $(BIN_D)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_D)/%.o: $(SRC_D)/%.c $(SRC_D)/%.h | $(OBJ_D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_D)/%.o: $(SRC_D)/%.c | $(OBJ_D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_D):
	mkdir -p $@

$(OBJ_D):
	mkdir -p $@

clean:
	rm -rf $(BIN_D) $(OBJ_D)
