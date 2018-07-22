/*
 * \author João Coelho
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

#include "inode.h"
#include "datatypes.h"

#include "inodeattr.h"

#include "probing.h"
#include "exception.h"
#include "superblock.h"
#include "czdealer.h"
#include "itdealer.h"

#include "rawdisk.h"

#include "fileclusters.h"


#include "freelists.h"
#include "direntries.h"

/*
 *  \brief Delete a link to a file from a directory and possibly the file it refers to from the file system.
 *
 *  It tries to emulate <em>unlink</em> system call.
 *
 *  \param path path to the file to be deleted
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soUnlink(const char *path)
{
    soProbe(226, "soUnlink(\"%s\")\n", path);

    try
    {
        char* xpath = strdupa(path);           // duplica o array de chars "path"
        char* direntryName = strdupa(basename(xpath));   // nome da direntry a ser criada
        char* dirName = dirname(xpath);             // path do diretório 

        uint32_t inD = soTraversePath(dirName);     // número do Inode do diretório
        
        uint32_t ihD = iOpen(inD);
        
        uint32_t inL = soGetDirEntry(ihD, direntryName); // número do Inode do link    
         
       
        if(inL == NullReference)  
        {        
            throw SOException(ENOENT, __FUNCTION__);    
        }
        
        
        uint32_t ihL = iOpen(inL);            
        SOInode* inP;
        inP = iGetPointer(ihL);
        
        if((inP->mode & S_IFREG) == S_IFREG)		//ver se o Inode é do tipo FileReg
        {           
            
            uint32_t refcnt = iDecLnkcnt(ihL);
            
            soDeleteDirEntry(ihD, direntryName, iGetAccess(ihD));
            if (refcnt == 0){
                soFreeInode(inL);
            } 
        }
        if((inP->mode & S_IFLNK) == S_IFLNK)			//ver se o Inode é do tipo SymLink
        {           
            soFreeInode(inL);
            soDeleteDirEntry(ihD, direntryName, iGetAccess(ihD));
        }
     
        iSave(ihL);
        iClose(ihL);
        iSave(ihD);
        iClose(ihD);

		return 0;
    }
    catch(SOException & err)
    {
        return -err.en;
    }

    return 0;
}
