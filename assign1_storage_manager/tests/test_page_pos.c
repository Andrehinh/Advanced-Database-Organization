#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../storage_mgr.h"
#include "../dberror.h"
#include "../test_helper.h"

// test name
char *testName;

/* test output files */
#define TESTPF "test_page_pos.bin"

/* main function running all tests */
int main(void)
{
    testName = "test page pos";

    SM_FileHandle fh;
    TEST_CHECK(createPageFile(TESTPF));
    TEST_CHECK(openPageFile(TESTPF, &fh));

    ASSERT_TRUE(fh.curPagePos == 0, "expect page pos 0 after open");

    const int newCapacity = 4;
    TEST_CHECK(ensureCapacity(newCapacity, &fh));
    ASSERT_EQUALS_INT(0, fh.curPagePos, "expect page pos 0 (not moved) after ensureCapacity()");

    SM_PageHandle page = malloc(PAGE_SIZE);

    ASSERT_ERROR(writeBlock(-1, &fh, page), "expect error if try to write a page pos negative");
    TEST_CHECK(writeBlock(0, &fh, page));
    TEST_CHECK(writeBlock(1, &fh, page));
    TEST_CHECK(writeBlock(2, &fh, page));
    TEST_CHECK(writeBlock(3, &fh, page));
    TEST_CHECK(writeCurrentBlock(&fh, page));
    ASSERT_EQUALS_INT(RC_OK, writeBlock(6, &fh, page), "expect OK if try to write a page out of range with auto ensureCapacity()");
    ASSERT_EQUALS_INT(0, fh.curPagePos, "expect page pos 0 (not moved) after writeBlock() calls");

    ASSERT_ERROR(readBlock(-1, &fh, page), "expect error if try to read a page out of range");
    ASSERT_ERROR(readPreviousBlock(&fh, page), "expect error if try to read prev page at the begin");
    TEST_CHECK(readBlock(0, &fh, page));
    TEST_CHECK(readBlock(1, &fh, page));
    TEST_CHECK(readBlock(2, &fh, page));
    ASSERT_EQUALS_INT(2, fh.curPagePos, "expect page pos 2 after a readBlock(2) call");
    TEST_CHECK(readNextBlock(&fh, page));
    ASSERT_EQUALS_INT(3, fh.curPagePos, "expect page pos 3 after a readNextBlock() call follows readBlock(2)");
    TEST_CHECK(readBlock(3, &fh, page));
    ASSERT_EQUALS_INT(3, fh.curPagePos, "expect page pos 3 after a readBlock(3) call");

    TEST_CHECK(readBlock(fh.totalNumPages - 1, &fh, page));
    ASSERT_ERROR(readNextBlock(&fh, page), "expect error if try to read next page at the end");
    ASSERT_ERROR(readBlock(fh.totalNumPages, &fh, page), "expect error if try to read a page out of range");

    TEST_CHECK(readLastBlock(&fh, page));
    ASSERT_EQUALS_INT(fh.totalNumPages - 1, fh.curPagePos, "expect page pos of last after a readLastBlock() call");

    TEST_CHECK(readFirstBlock(&fh, page));
    ASSERT_EQUALS_INT(0, fh.curPagePos, "expect page pos 0 after a readFirstBlock() call");

    free(page);

    TEST_CHECK(closePageFile(&fh));
    TEST_CHECK(destroyPageFile(TESTPF));
    TEST_DONE();

    return 0;
}
