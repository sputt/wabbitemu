CC = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
CXXFLAGS+=  -I.  -DUSE_REUSABLES -DUSE_GMP -DUNIXVER -DUSE_BUILTIN_FCREATE
LDFLAGS+= -lc -lgmp -lm -lcrypto
	 
# Suffix Rules
.SUFFIXES: .cpp
	 
.cpp.o:
		$(CC) $(CXXFLAGS) -c $<
	 
.cpp:
		$(CC) $(CXXFLAGS) $< -o $@

SRC = main.cpp opcodes.cpp pass_one.cpp pass_two.cpp utils.cpp export.cpp preop.cpp directive.cpp console.cpp \
expand_buf.cpp hash.cpp list.cpp parser.cpp storage.cpp errors.cpp bitmap.cpp
OBJ = $(addsuffix .o, $(basename $(SRC)))
OBJ_FILES = $(addsuffix .o, $(basename $(notdir $(SRC))))
	 
spasm: $(OBJ) Makefile
		$(CC) $(LDFLAGS) -o spasm $(OBJ_FILES)
	 
clean:
		rm $(OBJ)
	 
install: spasm /usr/bin/spasm
