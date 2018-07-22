/**
 *  \author Joao Marques Correia e Patricia Vale
 *  \tester Joao Marques Correia e Patricia Vale
 */


#include <libgen.h>
#include "inodeattr.h"
#include "datatypes.h"
#include "itdealer.h"
#include "direntries.h"
#include "direntries.bin.h"

#include "probing.h"
#include "exception.h"
#include <errno.h>

uint32_t soTraversePath(char *path)
{
    soProbe(301, "soTraversePath(%s)\n", path);
    //return soTraversePathBin(path);   

    uint32_t iNumber;
	
	char *pathCopy = strdupa(path);												//copy path content to ensure original is not altered
    char *base = strdupa(basename(pathCopy));									//get base name (the next component of path, after the last slash)
    char *dir = dirname(pathCopy);												//get directory name (the second to last component on the path string, before the last slash)
	
	uint32_t length = strlen(path);
	if(length == 1 && path[0] == '/')											//if path only has 1 char and its a slash then its root
    {
		iNumber = 0;															//return first inode number
		return iNumber;															//recursive call end condition, meaning it reached root
    }


    if(strcmp(dir,"/") == 0)													//if dir has no slashes (meaning its not the last component of path)
    {
    	int ih = iOpen(0);														//open inode
        if(!iCheckAccess(ih, 01)) throw SOException(EACCES,__FUNCTION__);		//checks for execute permission
    	iNumber = soGetDirEntry(ih, base);										//get base inode number
    	if(iNumber == NullReference) throw SOException(ENOENT,__FUNCTION__); 	//checks if inode exists
        iSave(ih);																//save inode
		return iNumber;
    }

    int ih = iOpen(soTraversePath(dir));										//open inode number given by soTraversePath recursive call
    if(!iCheckAccess(ih, 01)) throw SOException(EACCES,__FUNCTION__);			//checks for execute permission
    iNumber = soGetDirEntry(ih, base);											//get base inode number
    if(iNumber == NullReference) throw SOException(ENOENT,__FUNCTION__);		//checks if inode exists
    iSave(ih);																	//save inode
    return iNumber;
}
