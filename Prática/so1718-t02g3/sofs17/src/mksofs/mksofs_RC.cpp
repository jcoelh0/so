#include "mksofs.h"
#include "mksofs.bin.h"

#include "datatypes.h"
#include "rawdisk.h"
#include "exception.h"

#include <errno.h>

/* see mksofs.h for a description */
void resetClusters(uint32_t cstart, uint32_t ctotal)
{
    //resetClustersBin(cstart, ctotal);
	char* arrayEmpty;
	arrayEmpty = new char[BlockSize];
	for(unsigned int i=0;i<BlockSize;i++)
		arrayEmpty[i]=0;
	for(unsigned int i=ctotal;i>0;i--){
		soWriteRawBlock(cstart+(ctotal-i),arrayEmpty);
	}

}
