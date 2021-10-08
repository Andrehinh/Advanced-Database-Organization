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
#define TESTPF "test_page_count.bin"

/* main function running all tests */
int main(void)
{
    testName = "test page count";

    TEST_CHECK(createPageFile(TESTPF));

    SM_FileHandle fh;
    TEST_CHECK(openPageFile(TESTPF, &fh));
    ASSERT_TRUE(fh.totalNumPages == 1, "expect 1 page in new file");

    const int newCapacity = 4;
    TEST_CHECK(ensureCapacity(newCapacity, &fh));
    ASSERT_EQUALS_INT(newCapacity, fh.totalNumPages, "expect 4 pages");
    TEST_CHECK(closePageFile(&fh));

    TEST_CHECK(openPageFile(TESTPF, &fh));
    ASSERT_EQUALS_INT(newCapacity, fh.totalNumPages, "expect 4 pages after close and re-open");
    TEST_CHECK(closePageFile(&fh));

    TEST_CHECK(destroyPageFile(TESTPF));

    TEST_DONE();

    return 0;
}
