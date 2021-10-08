.PHONY: all test1 test2 clean

test1: | clean test_assign2_1

test2: | clean test_assign2_2

all: | clean test_assign2_1 test_assign2_2

DEPS = dberror.c storage_mgr.c buffer_mgr.c buffer_mgr_stat.c

test_assign2_1:
	@cc -o test_assign2_1.o test_assign2_1.c $(DEPS)
	@./test_assign2_1.o

test_assign2_2:
	@cc -o test_assign2_2.o test_assign2_2.c $(DEPS)
	@./test_assign2_2.o

clean:
	@rm -f *.bin *.o
