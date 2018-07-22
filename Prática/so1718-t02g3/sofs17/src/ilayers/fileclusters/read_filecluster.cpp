/**
 *  \author Joao Marques Correia
 *  \tester Joao Marques Correia
 */

#include "czdealer.h"
#include "datatypes.h"

#include "fileclusters.h"
#include "fileclusters.bin.h"

#include "probing.h"
#include "exception.h"

#include <errno.h>
#include <inttypes.h>

void soReadFileCluster(int ih, uint32_t fcn, void *buf)
{
    soProbe(404, "soReadFileCluster(%d, %u, %p)\n", ih, fcn, buf);
    //soReadFileClusterBin(ih, fcn, buf);
		
	if(soGetFileCluster(ih,fcn) == NullReference)							//if cluster is not allocated
	{
		uint32_t temp[BlockSize] = {0x00000000};
		buf = temp;															//return stream of 0's
	}
	else
	{
		soReadCluster(soGetFileCluster(ih,fcn),buf);						//if cluster is allocated return its value
	}
	
}
