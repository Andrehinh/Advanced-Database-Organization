#include "storage_mgr_helper.c"

/* manipulating page files */
void initStorageManager(void)
{
}

RC createPageFile(char *fileName)
{
    RC wrote_rc = RC_WRITE_FAILED;

    int fd = creat(fileName, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        return wrote_rc;
    }

    wrote_rc = writeHeaderPage(fd);
    if (wrote_rc == RC_OK)
    {
        wrote_rc = writeEmptyPage(fd);
    }

    return wrote_rc;
}

RC openPageFile(char *fileName, SM_FileHandle *fHandle)
{
    int fd = open(fileName, O_RDWR);
    if (fd == -1)
    {
        return RC_FILE_NOT_FOUND;
    }

    int *fdp = malloc(sizeof(int));
    *fdp = fd;
    fHandle->mgmtInfo = fdp;
    fHandle->fileName = fileName;
    fHandle->curPagePos = 0;

    return readDatabaseFileHeader(fHandle);
}

RC closePageFile(SM_FileHandle *fHandle)
{
    updateDatabaseFileHeader(fHandle);

    int *fdp = fHandle->mgmtInfo;
    int closed = close(*fdp);
    if (closed == 0)
    {
        free(fdp);
        return RC_OK;
    }

    return RC_FILE_HANDLE_NOT_INIT;
}

RC destroyPageFile(char *fileName)
{
    return remove(fileName);
}

/* reading blocks from disc */
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    if (pageNum < 0 || fHandle->totalNumPages <= pageNum)
    {
        return RC_READ_NON_EXISTING_PAGE;
    }

    int fd = *(int *)(fHandle->mgmtInfo);

    // Since we have a reserved header page, so the page 0 starts at offset PAGE_SIZE.
    // Means: int offset = (pageNum + 1) * PAGE_SIZE;
    int targetOffset = (pageNum + 1) * PAGE_SIZE;
    int currentOffset = lseek(fd, 0, SEEK_CUR);

    if (currentOffset != targetOffset)
    {
        int offset = targetOffset - currentOffset;
        lseek(fd, offset, SEEK_CUR);
    }

    ssize_t readSize = read(fd, memPage, PAGE_SIZE);

    // Update curPagePos
    fHandle->curPagePos = pageNum;

    return RC_OK;
}

int getBlockPos(SM_FileHandle *fHandle)
{
    return fHandle->curPagePos;
}

RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(0, fHandle, memPage);
}

RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos - 1, fHandle, memPage);
}

RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos + 1, fHandle, memPage);
}

RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->totalNumPages - 1, fHandle, memPage);
}

/* writing blocks to a page file */
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    if (pageNum < 0)
    {
        return RC_WRITE_FAILED;
    }

    if (fHandle->totalNumPages <= pageNum)
    {
        ensureCapacity(pageNum + 1, fHandle);
    }

    int fd = *(int *)(fHandle->mgmtInfo);

    lseek(fd, (pageNum + 1) * PAGE_SIZE, SEEK_SET);

    RC rc = writePage(fd, memPage);

    return rc;
}

RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return writeBlock(fHandle->curPagePos, fHandle, memPage);
}

RC appendEmptyBlock(SM_FileHandle *fHandle)
{
    int fd = *(int *)(fHandle->mgmtInfo);

    // Seek to end position anyway.
    lseek(fd, (fHandle->totalNumPages + 1) * PAGE_SIZE, SEEK_SET);

    RC rc = writeEmptyPage(fd);

    if (rc == RC_OK)
    {
        fHandle->totalNumPages += 1;
    }

    return rc;
}

RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle)
{
    RC rc = RC_OK;

    while (numberOfPages > fHandle->totalNumPages)
    {
        rc = appendEmptyBlock(fHandle);
        if (rc != RC_OK)
        {
            break;
        }
    }

    return rc;
}
