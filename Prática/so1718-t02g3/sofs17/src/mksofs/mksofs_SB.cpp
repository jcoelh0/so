#include "mksofs.h"
#include "mksofs.bin.h"

#include "datatypes.h"
#include "rawdisk.h"
#include "exception.h"

#include <errno.h>

/* see mksofs.h for a description */
void fillInSuperBlock(const char *name, uint32_t ntotal, uint32_t itsize, uint32_t rmsize)
{
    	//fillInSuperBlockBin(name, ntotal, itsize, rmsize);

	SOSuperBlock s;
    	SOReferenceCache rcache;
    	SOReferenceCache icache;

    	s.magic = 0xFFFF;
    	s.version = 0x2017;
    	strcpy(s.name, name);
    	s.mntstat = 1;
    	s.mntcnt = 0;
    	s.ntotal = ntotal;
    	s.itstart = 1;
    	s.itsize = itsize;
    	s.itotal = itsize * InodesPerBlock;
    	s.ifree = s.itotal - 1;
    	s.ihead = 1;
    	s.rmstart = itsize + 1;
    	s.rmsize = rmsize;
    	s.rmidx = 0;
    	s.czstart = itsize + rmsize + 1;
    	s.ctotal = (ntotal - itsize - rmsize - 1) / BlocksPerCluster;
    	s.cfree = s.ctotal - 1;
    	rcache.idx = REFERENCE_CACHE_SIZE;
    	for(unsigned int i=0; i < REFERENCE_CACHE_SIZE; i++){
      		rcache.ref[i] = NullReference;
    	}
    	icache.idx = 0;
    	for(unsigned int i=0; i < REFERENCE_CACHE_SIZE; i++){
      		icache.ref[i] = NullReference;
    	}
    	s.rcache = rcache;
    	s.icache = icache;

    	soWriteRawBlock(0, &s);
}
