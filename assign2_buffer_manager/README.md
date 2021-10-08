# Ideas of implementation:

## PageTable design

We have to define a `PageTable` struct to hold the metadata of the buffer poll.

```c
// Describe the metadata of a page frame.
typedef struct PageTableItem
{
	int pageNum;
	bool dirtyFlag;
	int fixCount;
	char *frame;
} PageTableItem;

// Describe the metadata of a buffer pool.
typedef struct PageTable
{
	SM_FileHandle *pageFileHandle;
	int numPageFrames;
	int numReadIO;
	int numWriteIO;
	PageTableItem *pageFrames;
	char *bufferPool;
	int fifoNext;
} PageTable;

```

And let's arrange the bytes for the whole page table and buffer pool like this:

| Bytes block      | size of bytes                |
| -----------      | -----------                  |
| PageTable        | sizeof(PageTable)            |
| PageTableItem[]  | sizeof(PageTable) * numPages |
| Frame[]          | PAGE_SIZE * numPages         |

## Logic of pinPage()

### FIFO strategy

> Define a `fifoNext` field in the `PageTable` struct, we use it to indicate the next frame we should check and use. The initial value is `0`.

Logic with the cases/status of the frame:
* If frame is empty (`pageNum = NO_PAGE`), just use it.
* If frame is not empty (`pageNum > NO_PAGE`)
* * If frame is not be pinned (`fixCount <= 0`)
* * * If frame is not dirty, just use it
* * * If frame is dirty, force it, and then use it
* * If the frame is being pinned (`fixCount > 0`)
* * * Move to the next frame and repeat the logic.
* * * Exit with error (-1) because no frame is available to use.

> Set `fifoNext` field to indicate the next frame we should check and use. 

### LRU strategy

*[TODO]*

# Test cases:

* Clean tests: `make clean`
* The predefined test 1, the default: `make test1`
* The predefined test 2: `make test2`
* Run all tests: `make all`
