/*
 * \author Pedro Salgado 80051
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
#include "inode.h"
#include "inodeattr.h"
#include "itdealer.h"
#include "czdealer.h"
#include "fileclusters.h"
#include "direntries.h"
#include "datatypes.h"

#include "probing.h"
#include "exception.h"

/*
 *  \brief Truncate a regular file to a specified length.
 *
 *  It tries to emulate <em>truncate</em> system call.
 *
 *  \param path path to the file
 *  \param length new size for the regular size
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soTruncate(const char *path, off_t length)
{
    soProbe(231, "soTruncate(\"%s\", %u)\n", path, length);

    try{

        char* pathc = strdupa(path);            

        uint32_t pin;
        pin = soTraversePath(pathc);         
        int inodehandler = iOpen(pin);
        SOInode* inode = iGetPointer(inodehandler);
        
        if (S_ISREG(inode->mode)){   
            uint32_t cluster = length / ClusterSize; 
            uint32_t resto = length % ClusterSize;       

            uint32_t filesize = ClusterSize / sizeof(uint32_t);   
            
            if (inode->size > length){                           
                soFreeFileClusters(inodehandler,  cluster); 
                uint32_t buffer[filesize];

                soReadFileCluster(inodehandler, inode->size, &buffer); 

                uint32_t poserase = resto / sizeof(uint32_t);  
                for (; poserase < filesize; poserase++ ){
                    buffer[poserase] = 0x00000000;
                }
                soWriteFileCluster(inodehandler, inode->size, &buffer); 
            }
            else if(inode->size < length){ 
                uint32_t buffer[filesize];
                soReadFileCluster(inodehandler, inode->size-1, &buffer); 
                if (cluster != inode->size){
                    uint32_t fill = (inode->size % ClusterSize) / sizeof(uint32_t);   
                    while (fill < filesize){
                        buffer[fill++] = '\0';                         
                    }

                    soWriteFileCluster(inodehandler, inode->size, buffer);
                    uint32_t i = 0;
                    while (i < filesize){
                        buffer[i] = '\0';
                        i++;
                    }

                    while (inode->size < (cluster)  ){
                        soWriteFileCluster(inodehandler, inode->size, &buffer);
                    }
                }
                else{
                    uint32_t fill = (inode->size % ClusterSize) / sizeof(uint32_t);
                    uint32_t end = resto / sizeof(uint32_t);
                    while (fill < end){
                        buffer[fill++] = '\0';
                    }
                    soWriteFileCluster(inodehandler, inode->size-1, buffer);
                }
            }
            inode->size = length;
        }
        else{
            throw SOException(-EINVAL,__FUNCTION__);  
        }

        iSave(inodehandler);
        iClose(inodehandler);
        return 0;
    }
    catch (SOException & err){
        return -err.en;
    }
}
