# Ideas of implementation:

1. The layout of the database file is:

* The first page, reserved for program use. Let's call it the header page.
* The second page is the first data page. So the `readFirstBlock()` function reads start at the offset `PAGE_SIZE` from beginning.

2. Header page layout

* The header page (or the database file) begins with the byte sequence corresponds to string "DB file by cs525-fall21-group1-china"
* We can simply determine whether a database file is valid to this program by checking the header string.
* The 4-byte integer at offset 64 stores the value of `totalNumPages`.


# Test cases:

* Clean tests: `make clean`
* The predefined test: `make`
* Run all tests: `make all`
* Test file open & close: `make test_open_close`
* Test page count: `make test_page_count`
* Test page position: `make test_page_pos`
* Test page data: `make test_page_data`
