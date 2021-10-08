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
#define TESTPF "test_page_data.bin"

static void testWriteAndReadWithRandomData(SM_FileHandle *fh);

/* main function running all tests */
int main(void)
{
    testName = "test page data";

    time_t t;
    srand((unsigned)time(&t));

    SM_FileHandle fh;
    TEST_CHECK(createPageFile(TESTPF));
    TEST_CHECK(openPageFile(TESTPF, &fh));

    int runTimes = 100 + (rand() >> 24);

    ASSERT_TRUE((runTimes >= 100), "Randomly determine the times to test");
    printf("Test write and read with random data %i times! \n", runTimes);

    int i = 0;
    while (i++ < runTimes)
    {
        testWriteAndReadWithRandomData(&fh);
    }

    printf("\nAll %i times tests are OK! \n\n", runTimes);

    TEST_CHECK(closePageFile(&fh));
    TEST_CHECK(destroyPageFile(TESTPF));
    TEST_DONE();

    return 0;
}

void testWriteAndReadWithRandomData(SM_FileHandle *fh)
{
    SM_PageHandle page_to_save = malloc(PAGE_SIZE);
    SM_PageHandle page_back = malloc(PAGE_SIZE);

    // Generate random data for the page to write.
    int i;
    for (i = 0; i < PAGE_SIZE; i++)
    {
        page_to_save[i] = rand() >> 24;
    }

    printf("----------------------------------------------------------\n");

    int pageNum = rand() >> 24;
    printf("Randomly test to write and read on the page number %i! \n", pageNum);

    TEST_CHECK(writeBlock(pageNum, fh, page_to_save));

    // Read the page back
    TEST_CHECK(readBlock(pageNum, fh, page_back));

    // Compare the page_back with page_to_save
    for (i = 0; i < PAGE_SIZE; i++)
    {
        if (page_back[i] == page_to_save[i])
        {
            continue;
        }

        ASSERT_TRUE((page_back[i] == page_to_save[i]), "character in page read from disk is the one we expected.");
    }

    printf("The data read back is the same as what it wrote. \n");

    free(page_to_save);
    free(page_back);

    TEST_DONE();
}