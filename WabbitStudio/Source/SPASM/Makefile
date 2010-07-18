CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
CFLAGS = --std=c99 -x c -g3 -I. -I./parser  -DUSE_REUSABLES -DUSE_GMP -DUNIXVER
LDFLAGS = -lc -lgmp -lm -lcrypto

# Suffix Rules
.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) -c $<

.c:
	$(CC) $(CFLAGS) $< -o $@
	
SRC = main.c opcodes.c pass_one.c pass_two.c utils.c export.c preop.c directive.c console.c \
parser/expand_buf.c parser/hash.c parser/list.c parser/parser.c parser/storage.c
OBJ = $(addsuffix .o, $(basename $(SRC)))
OBJ_FILES = $(addsuffix .o, $(basename $(notdir $(SRC))))

spasm: $(OBJ)
	gcc $(LDFLAGS) -o spasm $(OBJ_FILES)
	
clean:
	rm $(OBJ)
	
install: spasm /usr/bin/spasm
