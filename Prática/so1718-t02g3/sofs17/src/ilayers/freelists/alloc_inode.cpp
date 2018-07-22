/**
 *  \author Davide Cruz
 *  \tester Davide Cruz
 */

#include "freelists.h"
#include "freelists.bin.h"

#include "probing.h"
#include "exception.h"

#include "sbdealer.h"
#include "itdealer.h"
#include "datatypes.h"

#include <unistd.h>
#include <sys/types.h>

#include <errno.h>
#include <inttypes.h>

#include <time.h>

/*
 * Dictates to be obeyed by the implementation:
 * - error ENOSPC should be thrown if there is no free inodes
 * - the allocated inode must be properly initialized
 */
uint32_t soAllocInode(uint32_t type)
{
    soProbe(502, "soAllocInode(%"PRIu32", %p)\n", type);
    //return soAllocInodeBin(type);
    //Variables
    SOSuperBlock* pSB;
    uint32_t inodeToAlloc;
    uint32_t inodeToAllocHandler;
    SOInode* inodeToAllocPointer;
    uint32_t inodeToAllocNext;
    uint32_t inodeToAllocNextHandler;
    SOInode* inodeToAllocNextPointer;
    uint32_t inodeToAllocPrev;
    uint32_t inodeToAllocPrevHandler;
    SOInode* inodeToAllocPrevPointer;

    //Get Superblock
    soOpenSBDealer();
    pSB = sbGetPointer();
    //Check if there are free inodes
    if (pSB->ifree == 0){
      throw SOException(ENOSPC, "soAllocInode");
    }

    //Get Inode that is at the head of the linked list (To be allocated)
    inodeToAlloc = pSB->ihead;
    inodeToAllocHandler = iOpen(inodeToAlloc);
    inodeToAllocPointer = iGetPointer(inodeToAllocHandler);
    inodeToAllocPrev = inodeToAllocPointer->prev;
    inodeToAllocNext = inodeToAllocPointer->next;
    //Initialize inodeToAlloc
    inodeToAllocPointer->atime = time(NULL);
    inodeToAllocPointer->mtime = time(NULL);
    inodeToAllocPointer->ctime = time(NULL);
    //Set as Dir
    inodeToAllocPointer->mode = type;
    //Set group ID and user ID
    inodeToAllocPointer->owner = getuid();
    inodeToAllocPointer->group = getgid();
    //Save and close InodeToAlloc
    iSave(inodeToAllocHandler);
    iClose(inodeToAllocHandler);

    // If there are no more free inodes change SuperBlock and return
    if(pSB->ifree - 1 == 0){
      pSB->ifree--;
      pSB->ihead = NullReference;
      sbSave();
      //soCloseSBDealer();
      return inodeToAlloc;
    }

    //Get Inode that is next to change Prev value
    inodeToAllocNextHandler = iOpen(inodeToAllocNext);
    inodeToAllocNextPointer = iGetPointer(inodeToAllocNextHandler);
    inodeToAllocNextPointer->prev = inodeToAllocPrev;
    iSave(inodeToAllocNextHandler);
    iClose(inodeToAllocNextHandler);

    //Get Inode that is prev to change Next value
    inodeToAllocPrevHandler = iOpen(inodeToAllocPrev);
    inodeToAllocPrevPointer = iGetPointer(inodeToAllocPrevHandler);
    inodeToAllocPrevPointer->next = inodeToAllocNext;
    iSave(inodeToAllocPrevHandler);
    iClose(inodeToAllocPrevHandler);

    //Changes to Superblock
    pSB->ihead = inodeToAllocNext;
    pSB->ifree--;

    //Save and close SuperBlock
    sbSave();
    //soCloseSBDealer();

    return inodeToAlloc;
}
