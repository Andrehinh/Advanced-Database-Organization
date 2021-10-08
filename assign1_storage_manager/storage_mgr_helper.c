#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "dberror.h"
#include "storage_mgr.h"

const int OFFSET_PAGE_COUNT = 64;
const unsigned int HEADER_STRING_LEN = 37U;
const char *HEADER_STRING = "DB file by cs525-fall21-group1-china";

char *bytesCopy(char *dest, const char *src, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        dest[i] = src[i];

    return dest;
}

int readInt(const char *src)
{
    return src[0] + (src[1] << 8) + (src[2] << 16) + (src[3] << 24);
}

char *writeInt(char *dest, int value)
{
    dest[0] = (value & 0xFF);
    dest[1] = ((value >> 8) & 0xFF);
    dest[2] = ((value >> 16) & 0xFF);
    dest[3] = ((value >> 24) & 0xFF);

    return dest;
}

RC writePage(int __fd, SM_PageHandle page)
{
    ssize_t wroteSize = write(__fd, page, PAGE_SIZE);

    if (wroteSize != PAGE_SIZE)
    {
        return RC_WRITE_FAILED;
    }

    return RC_OK;
}

RC writeHeaderPage(int __fd)
{
    SM_PageHandle headerPage = calloc(PAGE_SIZE, 1);

    // Set the header string
    bytesCopy(headerPage, HEADER_STRING, HEADER_STRING_LEN);

    // Set the page count to 1
    writeInt(&headerPage[OFFSET_PAGE_COUNT], 1);

    RC wrote_rc = writePage(__fd, headerPage);

    free(headerPage);

    return wrote_rc;
}

RC writeEmptyPage(int __fd)
{
    SM_PageHandle page = calloc(PAGE_SIZE, 1);

    RC wrote_rc = writePage(__fd, page);

    free(page);

    return wrote_rc;
}

RC readDatabaseFileHeader(SM_FileHandle *fHandle)
{
    RC rc = RC_OK;
    int fd = *(int *)(fHandle->mgmtInfo);
    SM_PageHandle headerPage = calloc(PAGE_SIZE, 1);
    ssize_t readSize = read(fd, headerPage, PAGE_SIZE);
    if (readSize != PAGE_SIZE)
    {
        rc = RC_FILE_NOT_VALID;
    }

    if (rc == RC_OK)
    {
        // Verify the header string
        int i;
        for (i = 0; i < HEADER_STRING_LEN; i++)
        {
            if (HEADER_STRING[i] != headerPage[i])
            {
                rc = RC_FILE_NOT_VALID;
                break;
            }
        }
    }

    if (rc == RC_OK)
    {
        // Read the totalNumPages from position OFFSET_PAGE_COUNT
        fHandle->totalNumPages = readInt(&headerPage[OFFSET_PAGE_COUNT]);
    }

    free(headerPage);

    return rc;
}

RC updateDatabaseFileHeader(SM_FileHandle *fHandle)
{
    int fd = *(int *)(fHandle->mgmtInfo);

    lseek(fd, OFFSET_PAGE_COUNT, SEEK_SET);

    int size_page_count = sizeof(int);
    void *pageCount = malloc(size_page_count);
    ssize_t wroteSize = write(fd, writeInt(pageCount, fHandle->totalNumPages), size_page_count);
    free(pageCount);

    if (wroteSize != PAGE_SIZE)
    {
        return RC_WRITE_FAILED;
    }

    return RC_OK;
}
