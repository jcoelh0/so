/**
 *  \author João Coelho
 *  \tester João Coelho
 */

#include "direntries.h"
#include "direntries.bin.h"

#include "probing.h"
#include "exception.h"

#include <czdealer.h>
#include <fileclusters.h>
#include <itdealer.h>

#include <errno.h>
#include <datatypes.h>

void soRenameDirEntry(int pih, const char *name, const char *newName)
{
	soProbe(354, "soRenameDirEntry(%d, %s, %s)\n", pih, name, newName);
	//soRenameDirEntryBin(pih, name, newName);

	SOInode *in;
	bool entryFound = false;
	in = iGetPointer(pih);

	uint32_t dRpi= in->size / sizeof(SODirEntry); //number of direntry references per inode

	uint32_t dpc = DirentriesPerCluster; //number of direntries per cluster
	SODirEntry buf[dpc];

	uint32_t idxLC = dRpi / dpc; //index of last cluster

	for (uint32_t i = 0; i <= idxLC; i++)
	{
		soReadFileCluster(pih,i,buf);
		
		for (uint32_t k = 0; k < dpc; k++)
		{
			if (strcmp(buf[k].name,name) == 0)
			{
				entryFound = true;
				strncpy(buf[k].name, newName, SOFS17_MAX_NAME+1);
				break;
			}
		}
		if(entryFound)
		{
			soWriteFileCluster(pih,idxLC,buf);
			break;
		}
	}
	if(!entryFound)
		throw SOException(ENOENT, __FUNCTION__);
	
}
