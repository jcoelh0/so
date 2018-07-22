/*
 * \author Davide Cruz
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
#include "itdealer.h"
#include "inodeattr.h"
#include "fileclusters.h"
#include "freelists.h"
#include "direntries.h"

/*
 *  \brief Delete a directory.
 *
 *  It tries to emulate <em>rmdir</em> system call.
 *
 *  The directory should be empty, ie. only containing the '.' and '..' entries.
 *
 *  \param path path to the directory to be deleted
 *
 *  \return 0 on success;
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soRmdir(const char *path)
{
    soProbe(233, "soRmdir(\"%s\")\n", path);

    try
    {
        //soRmdirBin(path);
        uint32_t inodeDir;
        uint32_t inodeDirHandler;
        uint32_t inodeParent;
        uint32_t inodeParentHandler;
        SOInode* inodeDirPointer;

        char* pathCp = strdupa(path);
        char* dirName = strdupa(basename(pathCp));

        //Get inode number
        inodeDir = soTraversePath(pathCp);
        //Check if Path Exists
        if(inodeDir == NullReference) throw SOException(ENOENT,__FUNCTION__);
        inodeDirHandler = iOpen(inodeDir);
        inodeDirPointer = iGetPointer(inodeDirHandler);
        //Check if Inode is Dir
        if (!S_ISDIR(inodeDirPointer->mode)){
          throw SOException(ENOTDIR, __FUNCTION__);
        }
        //Check if Dir is empty
        soCheckEmptiness(inodeDirHandler);
        inodeParent = soGetDirEntry(inodeDirHandler,"..");
        inodeParentHandler = iOpen(inodeParent);
        //Delete Ref da DirToRemove do pai
        soDeleteDirEntry(inodeParentHandler, dirName);
        //Decrement lnkcnt do pai
        iDecLnkcnt(inodeParentHandler);
        //Limpar clusters do inode
        soFreeFileClusters(inodeDirHandler,0);
        //Libertar inode
        soFreeInode(inodeDir);
        iSave(inodeDirHandler);
        iSave(inodeParentHandler);
        iClose(inodeDirHandler);
        iClose(inodeParentHandler);
    }
    catch(SOException & err)
    {
        return -err.en;
    }

    return 0;
}
