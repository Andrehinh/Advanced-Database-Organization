#include "buffer_mgr.h"
#include "buffer_mgr_strategy_fifo.c"
#include "buffer_mgr_strategy_lru.c"

PageTableItem *getAvailableFrame(BM_BufferPool *const bm)
{
    PageTableItem *pf = NULL;

    switch (bm->strategy)
    {
    case RS_FIFO:
        pf = getAvailableFrame_FIFO(bm->mgmtData);
        break;
    case RS_LRU:
        // TODO
        pf = getAvailableFrame_LRU(bm->mgmtData);
        break;
    case RS_CLOCK:
        // TODO
        printf("strategy RS_CLOCK\n");
        break;
    case RS_LFU:
        // TODO
        printf("strategy RS_LFU\n");
        break;
    case RS_LRU_K:
        // TODO
        printf("strategy RS_LRU_K\n");
        break;
    default:
        printf("strategy default\n");
        break;
    }

    return pf;
}
