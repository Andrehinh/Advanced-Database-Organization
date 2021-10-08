#include "buffer_mgr.h"

PageTableItem *getAvailableFrame_FIFO(PageTable *const pt)
{
    PageTableItem *pf = NULL;

    // The frame we want to pin
    int tryTimes = 0;
    int frameOrder = pt->fifoNext;
    while (true)
    {
        pf = &(pt->pageFrames[frameOrder]);
        // Check if the frame caches a page
        if (pf->pageNum > NO_PAGE)
        {
            // Check the `dirtyFlag and `fixCount` to determine if we can evict it
            if (pf->fixCount <= 0)
            {
                if (pf->dirtyFlag == true)
                {
                    forcePageCore(pt, pf);
                }
                else
                {
                }

                // We can use this frame now
                break;
            }
            else
            {
                // Try next frame?
                if (tryTimes++ < pt->numPageFrames)
                {
                    frameOrder++;
                }
                else
                {
                    return NULL;
                }
            }
        }
        else
        {
            // We can just use this frame.
            break;
        }
    }

    // Move FIFO_next to next frame order.
    pt->fifoNext = (frameOrder + 1) % (pt->numPageFrames);

    return pf;
}
