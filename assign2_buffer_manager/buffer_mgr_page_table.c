#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "buffer_mgr_stat.h"
#include "buffer_mgr.h"

PageTable *createPageTable(const int numPages)
{
    PageTable *pt = calloc(1, sizeof(PageTable) + sizeof(PageTableItem) * numPages + PAGE_SIZE * numPages);
    pt->pageFrames = (void *)(pt + 1);
    pt->bufferPool = (void *)(pt->pageFrames + numPages);
    pt->numPageFrames = numPages;
    pt->numReadIO = 0;
    pt->numWriteIO = 0;

    int i;
    PageTableItem *pf = pt->pageFrames;
    for (i = 0; i < numPages; i++, pf++)
    {
        pf->pageNum = NO_PAGE;
        pf->dirtyFlag = false;
        pf->fixCount = 0;
        pf->frame = &(pt->bufferPool[i * PAGE_SIZE]);
    }

    // Set FIFO next to head of the first frame.
    pt->fifoNext = 0;

    return pt;
}

void disposePageTable(const PageTable *pt)
{
    free((void *)pt);
    pt = NULL;
}

PageTableItem *getPageFrame(const PageTable *const pt, const int pageNum)
{
    PageTableItem *pf = pt->pageFrames;

    for (int i = 0; i < pt->numPageFrames; i++)
    {
        if (pf[i].pageNum == pageNum)
        {
            return &(pf[i]);
        }
    }
    return NULL;
}

RC forcePageCore(PageTable *const pt, PageTableItem *const pf)
{
    RC rc = writeBlock(pf->pageNum, pt->pageFileHandle, pf->frame);
    pt->numWriteIO++;

    if (rc == RC_OK)
    {
        pf->dirtyFlag = false;
    }
    return rc;
}
