/**
 *  \author Patricia Vale
 *  \tester Patricia Vale
 */

#include "freelists.h"
#include "freelists.bin.h"

#include "probing.h"
#include "exception.h"

#include <errno.h>
#include <inttypes.h>

#include "sbdealer.h"

/*
 * Dictates to be obeyed by the implementation:
 * - parameter cn must be validated, 
 *      throwing a proper error if necessary
 */
void soFreeCluster(uint32_t cn)
{
    soProbe(531, "soFreeCluster(%"PRIu32")\n", cn);
	//soFreeClusterBin(cn);
    
    soOpenSBDealer();
	SOSuperBlock* superBlock = sbGetPointer();
	
	if(superBlock->ctotal < cn) throw SOException(22, "soFreeCluster");												//checks if the cluster to be freed exists
																				//if not, throws error
														
	if(superBlock->icache.idx == REFERENCE_CACHE_SIZE)							//if the cache is full, deplete
	{
		soDeplete();
	}
	
	superBlock->icache.ref[superBlock->icache.idx] = cn;					//insert the cluster reference into the icache
	superBlock->icache.idx = superBlock->icache.idx + 1;						//increment the icache idx
	
	sbSave();
}
