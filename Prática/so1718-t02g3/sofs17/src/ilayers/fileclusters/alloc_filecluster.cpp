/**
 *  \author Davide Cruz
 *  \tester Davide Cruz
 */

#include "fileclusters.h"
#include "fileclusters.bin.h"

#include "probing.h"
#include "exception.h"
#include "inode.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "itdealer.h"
#include "czdealer.h"
#include "datatypes.h"
#include "freelists.h"


#if 0
static void soAllocIndirectFileCluster(SOInode * ip, uint32_t fcn, uint32_t * cnp);
static void soAllocDoubleIndirectFileCluster(SOInode * ip, uint32_t fcn, uint32_t * cnp);
#endif

/* ********************************************************* */

uint32_t soAllocFileCluster(int ih, uint32_t fcn)
{
    soProbe(401, "soAllocFileCluster(%d, %u)\n", ih, fcn);
    //return soAllocFileClusterBin(ih, fcn);
    //Declare variables (varname1 is used when handling i1 on Inode ih, varname2 and varname3 is used when handling i2)
    SOInode* inodePointer; //Pointer that contains ih
    uint32_t cReturn,c1,c2,c3; //ClusterNum
    uint32_t index1,index2,index3; //Indexes to write the clusterNum
    uint32_t* cRef1; //Pointers that contain clusters c1,c2,c2
    uint32_t* cRef2;
    uint32_t* cRef3;
    uint32_t* cRefClean; //Pointer that contains a cluster filled with NullReference

    //Get inode ih
    inodePointer = iGetPointer(ih);
    //Create clean Cluster and fill it with NullReference
    cRefClean = new uint32_t[ReferencesPerCluster];
    std::fill(cRefClean,cRefClean + ReferencesPerCluster, NullReference);
    // for(int i = 0; i < ReferencesPerCluster; i++){
    //   cRefClean[i] = NullReference;
    // }

    //Check if fcn is valid
    if (fcn < 0 || fcn >= ReferencesPerCluster*ReferencesPerCluster + ReferencesPerCluster + N_DIRECT)
      throw SOException(22, "soAllocFileCluster");

    //If fcn is within array d
    if (fcn < N_DIRECT){
      //Write in d
      cReturn = soAllocCluster();
      inodePointer->clucnt = inodePointer->clucnt + 1;
      inodePointer->d[fcn] = cReturn;
    }
    //if fcn is within array i1
    else if (fcn < ReferencesPerCluster + N_DIRECT){
      //Check if i1 exists and if not creates it
      if(inodePointer->i1 == NullReference){
        c1 = soAllocCluster();
        inodePointer->clucnt = inodePointer->clucnt + 1;
        inodePointer->i1 = c1;
        soWriteCluster(c1,cRefClean);
      }
      else{
        c1 = inodePointer->i1;
      }
      //Read i1 cluter and writes it
      cRef1 = new uint32_t[ReferencesPerCluster];
      soReadCluster(c1,cRef1);
      cReturn = soAllocCluster();
      inodePointer->clucnt = inodePointer->clucnt + 1;
      index1 = fcn-N_DIRECT; //fcn - sizeOf(d)
      cRef1[index1] = cReturn;
      soWriteCluster(c1,cRef1);
      delete[] cRef1;
    }
    //if fcn is within array i2
    else{
      //Check if i2 exists and if not creates it
      if(inodePointer->i2 == NullReference){
        c2 = soAllocCluster();
        inodePointer->clucnt = inodePointer->clucnt + 1;
        inodePointer->i2 = c2;
        soWriteCluster(c2,cRefClean);
      }
      else{
        c2 = inodePointer->i2;
      }
      //Calculate indexes to be used
      index2 = (uint32_t) (fcn - ReferencesPerCluster - N_DIRECT) / ReferencesPerCluster; //(fcn - sizeof(i1) - sizeof(d)) / sizeof(i2)
      index3 = (fcn - ReferencesPerCluster - N_DIRECT) % ReferencesPerCluster;

      cRef2 = new uint32_t[ReferencesPerCluster];
      soReadCluster(c2,cRef2);
      //Check if index2 exists in i2 otherwise assigns a cluster and writes it in i2
      if(cRef2[index2] == NullReference){
        c3 = soAllocCluster();
        inodePointer->clucnt = inodePointer->clucnt + 1;
        cRef2[index2] = c3;
        soWriteCluster(c2,cRef2);
        soWriteCluster(c3,cRefClean);
      }
      else{
        c3 = cRef2[index2];
      }
      //Writes in cRef3 the designated cluster
      cRef3 = new uint32_t[ReferencesPerCluster];
      soReadCluster(c3,cRef3);
      cReturn = soAllocCluster();
      inodePointer->clucnt = inodePointer->clucnt + 1;
      cRef3[index3] = cReturn;
      soWriteCluster(c3,cRef3);
      delete[] cRef2;
      delete[] cRef3;
    }
    delete[] cRefClean;
    iSave(ih);
    return cReturn;
}

#if 0
/* ********************************************************* */

/* only a hint to decompose the solution */
static void soAllocIndirectFileCluster(SOInode * ip, uint32_t afcn, uint32_t * cnp)
{
    soProbe(401, "soAllocIndirectFileCluster(%p, %u, %p)\n", ip, afcn, cnp);

    throw SOException(ENOSYS, __FUNCTION__);
}

/* ********************************************************* */

/* only a hint to decompose the solution */
static void soAllocDoubleIndirectFileCluster(SOInode * ip, uint32_t afcn, uint32_t * cnp)
{
    soProbe(401, "soAllocDoubleIndirectFileCluster(%p, %u, %p)\n", ip, afcn, cnp);

    throw SOException(ENOSYS, __FUNCTION__);
}
#endif
