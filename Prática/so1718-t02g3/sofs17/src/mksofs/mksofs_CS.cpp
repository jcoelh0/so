//Author: Patricia Vale
//Tester: Patricia Vale

#include "mksofs.h"
#include "mksofs.bin.h"

#include "datatypes.h"
#include "rawdisk.h"
#include "exception.h"

#include <inttypes.h> 
#include <errno.h>

#include <math.h>

/* see mksofs.h for a description */
void computeStructure(uint32_t ntotal, uint32_t itotal,
            uint32_t * itsizep, uint32_t * rmsizep, uint32_t * ctotalp)
{
    // computeStructureBin(ntotal, itotal, itsizep, rmsizep, ctotalp);
    if(itotal == 0) {
		*itsizep = ceil((ntotal / 8) / InodesPerBlock);
	}
	else {
		*itsizep = ceil(itotal / InodesPerBlock);
	}
	
	uint32_t restBlocks = ntotal - 1- *itsizep;
	
	uint32_t i = 0, n = 0, temp = restBlocks;
	
	while(i < ceil((float)temp / BlocksPerCluster)) {
		n++;
		i += ReferencesPerBitmapBlock;
		temp--;
	}
	
	*rmsizep = n;
	
	*ctotalp = (restBlocks - n) / BlocksPerCluster;
	
	*itsizep += (restBlocks - n) % BlocksPerCluster;
}
