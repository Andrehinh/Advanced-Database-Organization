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
#define TESTPF "test_open_close.bin"

/* main function running all tests */
int main(void)
{
    testName = "test file open close";

    TEST_CHECK(createPageFile(TESTPF));
    TEST_CHECK(destroyPageFile(TESTPF));

    TEST_DONE();

    return 0;
}
