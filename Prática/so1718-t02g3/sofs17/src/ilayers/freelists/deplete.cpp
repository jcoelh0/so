/**
 *  \author Joao Marques Correia
 *  \tester Joao Marques Correia
 */

#include <math.h>

#include "rawdisk.h"
#include "sbdealer.h"
#include "superblock.h"
#include "datatypes.h"

#include "freelists.h"
#include "freelists.bin.h"

#include "probing.h"
#include "exception.h"

#include <errno.h>
#include <stdio.h>

/*
 * Even if some of them are not functionally necessary,
 * the following dictates must be obyed by the implementation:
 * - if crefs is equal to zero, 
 *      first transfer as much as possible to head cache
 * - 
 */
void writeBitMap(SOSuperBlock *superBlock);																			//deplete icache references to BitMap
void soDeplete(void)
{
    soProbe(541, "soDeplete()\n");
	//soDepleteBin();
	
	SOSuperBlock* superBlock = sbGetPointer();
		
	if(superBlock->rmidx == NullReference)																			//in case BitMap is empty copy references from icache to rcache
	{
		while((superBlock->rcache.idx > 0) && (superBlock->icache.idx > 0))											//while icache is not empty and/or rcache isent full
		{			
			superBlock->rcache.ref[superBlock->rcache.idx - 1] = superBlock->icache.ref[superBlock->icache.idx-1];	//copy reference from icache to rcache
					
			superBlock->icache.ref[superBlock->icache.idx-1] = NullReference;										//clear icache reference
			superBlock->rcache.idx = superBlock->rcache.idx - 1;													//decrease icache idx
			superBlock->icache.idx = superBlock->icache.idx - 1;													//decrease rcache idx
		}
		
		if(superBlock->icache.idx != 0)																				//if icache still has references
		{
			writeBitMap(superBlock);
		}
	}
	else
	{
		writeBitMap(superBlock);																					//in case BitMap isent empty
	}
	
	sbSave();
}

void writeBitMap(SOSuperBlock *superBlock)
{
	uint32_t block, byte, bit, ref, refn;
	uint8_t mask;
	
	while(superBlock->icache.idx > 0)																				//while icache still has references
	{
		mask = 0x80;
				
		ref = superBlock->icache.ref[superBlock->icache.idx-1];														//get icache reference
		block = floor((float)ref / ReferencesPerBitmapBlock);														//get block
		refn = ref - (ReferencesPerBitmapBlock * block);															//get reference index in relation to its block
		byte = ceil(refn / 8.0) - 1;																				//get byte
		bit = refn - (byte * 8);																					//get bit
		
		SORefBlock bitMap;
		soReadRawBlock((superBlock->rmstart + block), &bitMap);														//read BitMap block
		
		bitMap.map[byte] = bitMap.map[byte] | (mask >> bit);														//change reference to Free
		if(byte < bitMap.idx) bitMap.idx = byte;																	//update BitMap block index
		soWriteRawBlock((superBlock->rmstart + block), &bitMap);													//write BitMap block
		uint32_t rmdix = (byte + (block * ReferencesPerBitmapBlock));												//overall byte position
		if(superBlock->rmidx > rmdix) superBlock->rmidx = rmdix;													//update rmdix if necessary
		
		superBlock->icache.ref[superBlock->icache.idx - 1] = NullReference;											//clear icache reference
		superBlock->icache.idx = superBlock->icache.idx - 1;														//decrease icache idx
	}
}