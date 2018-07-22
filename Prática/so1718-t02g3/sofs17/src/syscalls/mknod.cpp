/*
 * \author  Joao Marques Correia
 */

#include "datatypes.h"
#include "direntries.h"
#include "itdealer.h"
#include "freelists.h"
#include "inodeattr.h"

#include "syscalls.h"
#include "syscalls.bin.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <libgen.h>
#include <string.h>

#include "probing.h"
#include "exception.h"

/*
 *  \brief Create a regular file with size 0.
 *
 *  It tries to emulate <em>mknod</em> system call.
 *
 *  \param path path to the file
 *  \param mode type and permissions to be set:
 *                    a bitwise combination of S_IFREG, S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH,
 *                    S_IWOTH, S_IXOTH
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soMknod(const char *path, mode_t mode)
{
    soProbe(228, "soMknod(\"%s\", %u)\n", path, mode);

    try
    {
        /* replace next line with your code */
        //soMknodBin(path, mode);		
		
        char* pathCopy = strdupa (path);           									//path safeguard
        char* file = strdupa(basename(pathCopy));    								//file name
        char* dir = dirname(pathCopy);              								//file directory

        uint32_t dirNumber = soTraversePath(dir);      								//directory Inode number

        if(dirNumber == NullReference) throw SOException(ENOENT,__FUNCTION__); 		//checks if directory exists

        uint32_t dirHandler = iOpen(dirNumber);										//directory Inode Handler
        uint32_t fileNumber = soGetDirEntry(dirHandler, file);						//file Inode number

        if(fileNumber != NullReference) throw SOException(EEXIST,__FUNCTION__);		//checks if name is unique(if there's no match on the File Number)

        SOInode* dirInode = iGetPointer(dirHandler);

        if(S_ISDIR(dirInode->mode) || S_ISLNK(dirInode->mode)) 				        //checks if parent is a link or directory
		{
            fileNumber = soAllocInode(S_IFREG);										//alloc new Inode
            uint32_t fileHandler = iOpen(fileNumber);								//open file Inode
            iSetAccess(fileHandler,(uint16_t)mode);									//set access
            soAddDirEntry(dirHandler,file,fileNumber);								//add file entry to directory Inode
			iIncLnkcnt(fileHandler);												//increase LinkCount on the directory Inode
			iSave(fileHandler);														//save file Inode
            iSave(dirHandler);  						 							//save directory Inode
			iClose(fileHandler);													//close file Inode
            iClose(dirHandler);  						 							//close directory Inode
        }
        else
		{
            iClose(dirHandler);  						 							//close directory Inode
            throw SOException(ENOTDIR,__FUNCTION__);								//if parent is not a link or directory
        }
    }
    catch(SOException & err)
    {
        return -err.en;
    }

    return 0;
}
