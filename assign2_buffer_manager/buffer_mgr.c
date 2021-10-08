#include "buffer_mgr_stat.h"
#include "buffer_mgr.h"
#include "buffer_mgr_page_table.c"
#include "buffer_mgr_strategy.c"

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy,
                  void *stratData)
{
    SM_FileHandle *fh = malloc(sizeof(SM_FileHandle));
    RC rc = openPageFile((char *)pageFileName, fh);
    if (rc == RC_OK)
    {
        bm->pageFile = (char *)pageFileName;
        bm->numPages = numPages;
        bm->strategy = strategy;

        PageTable *pt = createPageTable(numPages);
        pt->pageFileHandle = fh;
        bm->mgmtData = pt;
    }

    return rc;
}

RC shutdownBufferPool(BM_BufferPool *const bm)
{
    forceFlushPool(bm);
    PageTable *pt = bm->mgmtData;
    closePageFile(pt->pageFileHandle);

    free(pt->pageFileHandle);
    pt->pageFileHandle = NULL;

    disposePageTable(pt);
    return RC_OK;
}

RC forceFlushPool(BM_BufferPool *const bm)
{
    PageTable *pt = bm->mgmtData;
    PageTableItem *pf = pt->pageFrames;

    for (int i = 0; i < bm->numPages; i++, pf++)
    {
        if (pf->fixCount <= 0 && pf->dirtyFlag == true)
        {
            forcePageCore(pt, pf);
        }
    }

    return RC_OK;
}

// Buffer Manager Interface Access Pages
RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    PageTableItem *pf = getPageFrame(bm->mgmtData, page->pageNum);
    if (pf != NULL)
    {
        pf->dirtyFlag = true;
    }

    return RC_OK;
}

RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    PageTableItem *pf = getPageFrame(bm->mgmtData, page->pageNum);
    if (pf != NULL)
    {
        pf->fixCount--;
    }

    return RC_OK;
}

RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    PageTableItem *pf = getPageFrame(bm->mgmtData, page->pageNum);
    if (pf != NULL && pf->fixCount <= 0 && pf->dirtyFlag == true)
    {
        PageTable *pt = bm->mgmtData;
        return forcePageCore(pt, pf);
    }

    return RC_OK;
}

RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page,
           const PageNumber pageNum)
{
    page->pageNum = pageNum;
    PageTableItem *pfCache = getPageFrame(bm->mgmtData, pageNum);
    if (pfCache != NULL)
    {
        pfCache->fixCount++;
        page->data = pfCache->frame;
        return RC_OK;
    }
    else
    {
        // Load the page from storage
        PageTable *pt = bm->mgmtData;

        // Need to determine which page_frame to use to cache the page
        // This is determined by page replacement strategy.
        PageTableItem *pf = getAvailableFrame(bm);

        RC readRC = readBlock(page->pageNum, pt->pageFileHandle, pf->frame);
        pt->numReadIO++;

        if (readRC == RC_OK || readRC == RC_READ_NON_EXISTING_PAGE)
        {
            // It's also ok if a page to pin does not exist, this means the client wants to add new pages.
            // Now let's pin the frame
            pf->fixCount = 1;
            pf->pageNum = page->pageNum;
            page->data = pf->frame;

            return RC_OK;
        }
        else
        {
            return readRC;
        }
    }
}

// Statistics Interface
PageNumber *getFrameContents(BM_BufferPool *const bm)
{
    PageTable *pt = bm->mgmtData;
    PageTableItem *pf = pt->pageFrames;

    PageNumber *frameContents = calloc(bm->numPages, sizeof(PageNumber));
    for (int i = 0; i < bm->numPages; i++)
    {
        frameContents[i] = pf[i].pageNum;
    }
    return frameContents;
}

bool *getDirtyFlags(BM_BufferPool *const bm)
{
    PageTable *pt = bm->mgmtData;
    PageTableItem *pf = pt->pageFrames;

    bool *dirtyFlags = calloc(bm->numPages, sizeof(bool));
    for (int i = 0; i < bm->numPages; i++)
    {
        dirtyFlags[i] = pf[i].dirtyFlag;
    }
    return dirtyFlags;
}

int *getFixCounts(BM_BufferPool *const bm)
{
    PageTable *pt = bm->mgmtData;
    PageTableItem *pf = pt->pageFrames;

    int *fixCounts = calloc(bm->numPages, sizeof(int));
    for (int i = 0; i < bm->numPages; i++)
    {
        fixCounts[i] = pf[i].fixCount;
    }
    return fixCounts;
}

int getNumReadIO(BM_BufferPool *const bm)
{
    PageTable *pt = bm->mgmtData;
    return pt->numReadIO;
}

int getNumWriteIO(BM_BufferPool *const bm)
{
    PageTable *pt = bm->mgmtData;
    return pt->numWriteIO;
}
