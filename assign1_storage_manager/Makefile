.PHONY: test test_open_close test_page_count test_page_pos test_page_data clean

test: | clean test_assign1

all: | clean test_open_close test_page_count test_page_pos test_page_data test_assign1

test_assign1:
	@cc -o test_assign1.o test_assign1_1.c dberror.c storage_mgr.c
	@./test_assign1.o

test_open_close: | clean test_open_close_o

test_open_close_o:
	@cc -o test_open_close.o tests/test_open_close.c dberror.c storage_mgr.c
	@./test_open_close.o

test_page_count: | clean test_page_count_o

test_page_count_o:
	@cc -o test_page_count.o tests/test_page_count.c dberror.c storage_mgr.c
	@./test_page_count.o

test_page_pos: | clean test_page_pos_o

test_page_pos_o:
	@cc -o test_page_pos.o tests/test_page_pos.c dberror.c storage_mgr.c
	@./test_page_pos.o

test_page_data: | clean test_page_data_o

test_page_data_o:
	@cc -o test_page_data.o tests/test_page_data.c dberror.c storage_mgr.c
	@./test_page_data.o

clean:
	@rm -f *.bin *.o
