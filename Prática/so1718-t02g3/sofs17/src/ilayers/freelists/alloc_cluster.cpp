/**
 *  \author Pedro Salgado 80051
 *  \tester Pedro Salgado 80051
 */

#include "freelists.h"
#include "freelists.bin.h"

#include "probing.h"
#include "exception.h"
#include "superblock.h"
#include "datatypes.h"
#include "sbdealer.h"

#include <errno.h>

/*
 * Dictates to be obeyed by the implementation:
 * - error ENOSPC should be thrown if there is no free clusters
 * - after the reference is removed, 
 *      its location should be filled with NullReference
 */

uint32_t soAllocCluster()
{
    soProbe(501, "soAllocCluster()\n");
    SOSuperBlock * sb = sbGetPointer();
    
    if(sb->rcache.idx == 53)
      soReplenish();
    if(sb->rcache.idx == NullReference)
      throw SOException(ENOSPC, __FUNCTION__);

    uint32_t cluster;
    cluster = sb->rcache.ref[sb->rcache.idx];
    sb->rcache.ref[sb->rcache.idx] = NullReference;
    sb->rcache.idx += 1;
    sbSave();

    return cluster;
    //return soAllocClusterBin();
}
