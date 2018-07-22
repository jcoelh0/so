/**
 *  \author Davide Cruz
 *  \tester Davide Cruz
 */

#include "direntries.h"
#include "direntries.bin.h"

#include "probing.h"
#include "exception.h"
#include "datatypes.h"
#include "itdealer.h"
#include "fileclusters.h"

#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>


uint32_t soDeleteDirEntry(int pih, const char *name, bool clean)
{
    soProbe(353, "soDeleteDirEntry(%d, %s, %s)\n", pih, name, clean ? "true" : "false");
    //return soDeleteDirEntryBin(pih, name, clean);
    SOInode* inodeParentPointer;
    SODirEntry* dirArray;
    uint32_t fcnMax;
    uint32_t childInode;
    uint32_t i;
    uint32_t j;
    bool found = false;

    inodeParentPointer = iGetPointer(pih);

    //Check if parent inode is a directory
    if (!S_ISDIR(inodeParentPointer->mode)){
      throw SOException(20,"soDeleteDirEntry");
    }
    //Get Cluster that has dirEntry to delete
    dirArray = new SODirEntry[DirentriesPerCluster];
    //Get fcn's to read
    fcnMax = (inodeParentPointer->size / sizeof(SODirEntry)) / DirentriesPerCluster;

    //Read every cluster of the inode
    for(i = 0; i < fcnMax && !found; i++){
      soReadFileCluster(pih, i, dirArray);
      //For each dirEntry in cluster
      for(j = 0; j < DirentriesPerCluster && !found; j++){
        //If DirEntry to delete is found
        if ( strcmp(name,dirArray[j].name) == 0 ){
          found = true;
          if (clean) {
            std::fill(dirArray[j].name,dirArray[j].name+sizeof(dirArray[j].name),0);
            childInode = dirArray[j].in;
            dirArray[j].in = NullReference;
          }
          else{
            dirArray[j].name[SOFS17_MAX_NAME] = dirArray[j].name[0];
            dirArray[j].name[0] = '\0';
            childInode = dirArray[j].in;
          }
          soWriteFileCluster(pih,i,dirArray);
        }
      }
    }
    delete[] dirArray;
    if(found){
      return childInode;
    }
    else{
      throw SOException(2,__FUNCTION__);
    }
}
