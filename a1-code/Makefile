CFLAGS = -Wall -g
CC     = gcc $(CFLAGS)

commando : cmdcol.o cmd.o util.o commando.o
	$(CC) -o commando cmdcol.o cmd.o util.o commando.o
	@echo commando is ready

commando.o : commando.c commando.h
	$(CC) -c commando.c

cmdcol.o : cmdcol.c commando.h
	$(CC) -c $<

cmd.o : cmd.c commando.h
	$(CC) -c $<

util.o : util.c commando.h
	$(CC) -c $<

clean :
	rm -f commando *.o

# Testing Targets
test : test-functions test-commando

test-functions : test_functions
	@printf "Tests for correctness of functions\n"
	./test_functions
	@printf "\n"
	@printf "Valgrind with function tests\n"
	valgrind ./test_functions

test-commando : commando
	@printf "Tests for correctness of commando program\n"
	chmod u+x test_commando.sh standardize_pids
	./test_commando.sh

test_utils.o : test_utils.c test_utils.h
	$(CC) -c $<

test_functions.o : test_functions.c commando.h test_utils.h
	$(CC) -c $<

test_functions : test_functions.o cmd.o cmdcol.o test_utils.o
	$(CC) -o $@ $^

clean-tests :
	rm -f test_functions test-data/*.tmp

# fixes two small issues with tests that may cause correct code to
# fail because test data has slightly altered during transfers
fix-tests:
	touch test-data/stuff/empty
	chmod u+x test-data/stuff/table.sh
	chmod u+x test-data/table.sh
