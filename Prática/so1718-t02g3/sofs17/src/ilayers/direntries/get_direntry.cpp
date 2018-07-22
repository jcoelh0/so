/**
 *  \author Pedro Salgado 80051
 *  \tester Pedro Salgado 80051
 */

#include "direntries.h"
#include "direntries.bin.h"

#include "probing.h"
#include "exception.h"
#include "datatypes.h"
#include "inode.h"
#include "itdealer.h"
#include "fileclusters.h"

#include <errno.h>

uint32_t soGetDirEntry(int pih, const char *name)
{
    soProbe(351, "soGetDirEntry(%d, %s)\n", pih, name);
    //return soGetDirEntryBin(pih, name);
    SOInode* inode;
    bool found = false;
    inode = iGetPointer(pih);
    SODirEntry buffer [DirentriesPerCluster];

    uint32_t rpi= inode->size / sizeof(SODirEntry);
    uint32_t index = rpi / DirentriesPerCluster;				
    uint32_t  childinode;
    for (uint32_t i = 0; i <= index; i++)
    {
        soReadFileCluster(pih,i,buffer);
        for (uint32_t j = 0; j < DirentriesPerCluster; j++)
        {
            if (strcmp(name,buffer[j].name) == 0)
            {
            found = true;
            childinode = buffer[j].in;
            }
	    if(found) break;
	}
     }
	if(!found)
	{
	    return NullReference;
	}
	return childinode;
}
