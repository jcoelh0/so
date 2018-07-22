/*
 * \author Patrícia Vale, 80006
 */

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

#include "datatypes.h"
#include "direntries.h"
#include "itdealer.h"
#include "freelists.h"
#include "inodeattr.h"

/*
 *  \brief Create a directory.
 *
 *  It tries to emulate <em>mkdir</em> system call.
 *
 *  \param path path to the file
 *  \param mode permissions to be set:
 *          a bitwise combination of S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failute
 */
int soMkdir(const char *path, mode_t mode)
{
    soProbe(232, "soMkdir(\"%s\", %u)\n", path, mode);
    
    try
    {
        /* replace next line with your code */
        //soMkdirBin(path, mode);
        
        char* pathCp = strdupa(path);           									// save path
        char* file = strdupa(basename(pathCp));    	    						    // new directory name
        char* dir = dirname(pathCp);              	    							// parent directory
        
        uint32_t dirNum = soTraversePath(dir);      								// inode number of parent directory
        
        if(dirNum == NullReference) {
            throw SOException(ENOENT,__FUNCTION__); 		                        // checks if parent directory doesn't exist
        }
        
        uint32_t dirh = iOpen(dirNum);										        // inode handler of parent directory
        uint32_t fileNum = soGetDirEntry(dirh, file);						        // inode number of new directory
        
        if(fileNum != NullReference) {
            throw SOException(EEXIST,__FUNCTION__);		                            // checks if new directory name already exists
        }
        
        SOInode* dirInode = iGetPointer(dirh);
        
        if(S_ISDIR(dirInode->mode) || S_ISLNK(dirInode->mode)) {                    // checks if parent is a directory or a link
            fileNum = soAllocInode(S_IFDIR);					    				// alloc new inode for new directory
            uint32_t fileh = iOpen(fileNum);								        // open new directory inode
            iSetAccess(fileh,(uint16_t)mode);   								    // access
            soAddDirEntry(dirh, file, fileNum);		        						// add new directory to parent directory
			iIncLnkcnt(dirh);		        										// increase link count of the parent directory
            soAddDirEntry(fileh, ".", fileNum);                                     // add '.'
            soAddDirEntry(fileh, "..", dirNum);                                     // add '..'
			iSave(fileh);										    				// save new directory inode
            iSave(dirh);  						 		        					// save parent directory inode
			iClose(fileh);							        						// close new directory inode
            iClose(dirh);  						         							// close parent directory inode
        }
        else {
            iClose(dirh);  						 							        // close parent directory inode
            throw SOException(ENOTDIR,__FUNCTION__);								// error if parent is not a directory or a link
        }

        
    }
    catch(SOException & err)
    {
        return -err.en;
    }

    return 0;
}
