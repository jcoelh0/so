/**
 *  \author Patrícia Vale, 80006
 *  \tester Patrícia Vale, 80006
 */
 
 
#include "czdealer.h"
#include "datatypes.h"

#include "fileclusters.h"
#include "fileclusters.bin.h"

#include "probing.h"
#include "exception.h"

#include <errno.h>
#include <inttypes.h>

void soWriteFileCluster(int ih, uint32_t fcn, void *buf)
{
    soProbe(405, "soWriteFileCluster(%d, %u, %p)\n", ih, fcn, buf);
    //soWriteFileClusterBin(ih, fcn, buf);
    
	if(soGetFileCluster(ih,fcn) == NullReference) {
		soWriteCluster(soAllocFileCluster(ih,fcn),buf);
	}
	else {
		soWriteCluster(soGetFileCluster(ih,fcn),buf);
	}
}
