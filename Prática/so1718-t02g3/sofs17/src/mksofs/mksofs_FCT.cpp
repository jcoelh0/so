//Author: Joao Marques
//Tester: Joao Marques

#include <math.h>
#include "mksofs.h"
#include "mksofs.bin.h"

#include "datatypes.h"
#include "rawdisk.h"
#include "exception.h"

#include <errno.h>

/* see mksofs.h for a description */
void fillInFreeClusterTable(uint32_t rmstart, uint32_t ctotal)
{
    // fillInFreeClusterTableBin(rmstart, ctotal);							//remove commentary to test original function
	
	uint32_t blockNum = ceil ((float)ctotal / ReferencesPerBitmapBlock);	//minimum number of Bit Map blocks necessary
																			//to store all the Cluster usage values
	
	for(uint32_t i = 0; i <= blockNum; i++)									//traverse all Bit Map blocks
	{
		SORefBlock block;													//creates the block instance
		block.cnt = 0;
		block.idx = 0;
		uint8_t mask = 0x80;												//creates the necessary mask to manipulate the block bytes
		
		for(uint32_t y = 0; y < ReferenceBytesPerBitmapBlock; y++)			//traverse all the bytes from the "i" block
		{
			for(uint32_t z = 0; z < 8; z++)									//traverse all the bits from the "y" byte
			{
				if(ctotal > 0)												//if cluster is assigned
				{
					block.cnt++;
					if(i == 0 && y == 0 && z == 0) 							//ran once at the start, to ensure Root is assigned
					{
						block.cnt--;										//decrements the previously incremented count value
						block.map[y] = 0x00;
					}
					else block.map[y] = block.map[y] |  mask;				
					ctotal--;												//decrements the number of clusters that need to be assigned
				}
				else block.map[y] = block.map[y] & (mask ^ 0xFF);			//if no more clusters are assigned, fill with in-use bits
				mask = mask >> 1;											//shift mask bit position
			}
			
			mask = 0x80;													//reset mask
		}	
		soWriteRawBlock((rmstart + i), &block);								//write the block on th Bit Map		
	}
}
