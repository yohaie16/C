CC					=gcc
CFLAGES				=-ansi -pedantic -Wall
ifndef DEBUG
CFLAGES				+= -fsanitize=address -g
endif
PROG_NAME			= assembler 
TEST_DIR			=tests
TEST_DIR_IN			=$(TEST_DIR)/in
TEST_DIR_IN_ERR		=$(TEST_DIR_IN)/error
TEST_DIR_IN_WARN	=$(TEST_DIR_IN)/warn
TEST_DIR_IN_OK		=$(TEST_DIR_IN)/ok

TEST_DIR_OUT		=$(TEST_DIR)/out
TEST_DIR_OUT_ERR	=$(TEST_DIR_OUT)/error
TEST_DIR_OUT_WARN	=$(TEST_DIR_OUT)/warn
TEST_DIR_OUT_OK		=$(TEST_DIR_OUT)/ok

BUILD_DIR			= build
OBJ_DIR				=$(BUILD_DIR)/obj
BIN_DIR				=$(BUILD_DIR)/bin
OBJECTS				= assembler.o analyze.o trie.o vector.o	output_unit.o preprocessor.o share.o main.o
.PHONEY: clean build_env

$(PROG_NAME): build_env  $(OBJECTS)
	$(CC) $(CFLAGES) $(OBJ_DIR)/*.o -o $(BIN_DIR)/$@
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TEST_DIR_OUT_ERR)/* $(TEST_DIR_OUT_WARN)/* $(TEST_DIR_OUT_OK)/*
	

run_tests: $(PROG_NAME)
	$(BIN_DIR)/$(PROG_NAME) $(TEST_DIR_OUT)/o2 -o $(TEST_DIR_OUT_OK)
build_env:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

assembler.o: assembler/assembler.c assembler/assembler.h \
 assembler/../analyze/analyze.h assembler/../preprocessor/preprocessor.h \
 assembler/../share/share.h \
 assembler/../trie/trie.h \
 assembler/../vector/vector.h \
 assembler/../output_unit/output_unit.h \
 assembler/../output_unit/../trie/trie.h \
 assembler/../output_unit/../vector/vector.h \
 assembler/../output_unit/../share/share.h assembler/../trie/trie.h \
 assembler/../vector/vector.h

analyze.o:	analyze/analyze.c analyze/analyze.h analyze/../trie/trie.h

preprocessor.o: preprocessor/preprocessor.c preprocessor/preprocessor.h \
preprocessor/../vector/vector.h preprocessor/../trie/trie.h

vector.o:  vector/vector.c vector/vector.h
trie.o:		trie/trie.c 	trie/trie.h
main.o: main.c assembler/assembler.h

output_unit.o: output_unit/output_unit.c output_unit/output_unit.h \
 output_unit/../trie/trie.h output_unit/../vector/vector.h \
 output_unit/../share/share.h output_unit/../share/../trie/trie.h \
 output_unit/../share/../vector/vector.h

preprocessor.o: preprocessor/preprocessor.c preprocessor/preprocessor.h\
preprocessor/../vector/vector.h\
trie/trie.h

share.o: share/share.c share/share.h\
share/../vector/vector.h\
share/../trie/trie.h

 %.o:
		$(CC) $(CFLAGES) -c $< -o $(OBJ_DIR)/$@
