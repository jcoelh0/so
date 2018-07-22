/**
 *  \author Pedro Salgado, João Coelho
 *  \tester Pedro Salgado, João Coelho
 */

#include "fileclusters.h"
#include "fileclusters.bin.h"

#include "probing.h"
#include "exception.h"
#include "inode.h"
#include "superblock.h"
#include "czdealer.h"
#include "itdealer.h"

#include "rawdisk.h"
#include "datatypes.h"

#include <errno.h>
#include <stdint.h>

#define N_INDIRECT  2

static void soGetIndirectFileCluster(SOInode * ip, uint32_t fcn);
static void soGetDoubleIndirectFileCluster(SOInode * ip, uint32_t fcn);


/* ********************************************************* */
uint32_t cnp;

uint32_t soGetFileCluster(int ih, uint32_t fcn)
{
 
	soProbe(403, "soGetFileCluster(%d, %u)\n", ih, fcn);

	
	SOInode* ip;
	ip = iGetPointer(ih);
	
	if(fcn < N_DIRECT)
		cnp =ip->d[fcn];	
	else if(fcn < N_INDIRECT*ReferencesPerCluster + N_DIRECT)
		soGetIndirectFileCluster(ip, fcn - N_DIRECT);
	else
		soGetDoubleIndirectFileCluster(ip, fcn - N_DIRECT - N_INDIRECT*ReferencesPerCluster);
	
	iSave(ih);
	return cnp;
}


static void soGetIndirectFileCluster(SOInode * ip, uint32_t fcn)
{
    soProbe(403, "soGetIndirectFileCluster(%p, %u, %p)\n", ip, fcn);
				

	uint32_t buf[ReferencesPerCluster];			
	uint32_t *ref;				


	if (ip->i1!= NullReference)
	{
		soReadCluster(ip->i1, buf);
		ref = (uint32_t *) buf;
		cnp = ref[fcn%ReferencesPerCluster]; 
	}
	else 
		cnp=NullReference;

}


static void soGetDoubleIndirectFileCluster(SOInode * ip, uint32_t fcn)
{
	soProbe(403, "soGetDoubleIndirectFileCluster(%p, %u, %p)\n", ip, fcn);
	
	uint32_t buf[ReferencesPerCluster];			
	uint32_t *ref;			
	uint32_t clustind = fcn/ReferencesPerCluster;	
	uint32_t refclustind;				


	if(ip->i2 != NullReference)
	{

		soReadCluster(ip->i2, buf);


		ref = (uint32_t *) buf;


		refclustind = ref[clustind];
		
		soReadCluster(refclustind, buf);
		

		ref = (uint32_t *) buf;

		cnp = ref[fcn%ReferencesPerCluster]; 
	}
	else
		cnp=NullReference;
	

}
