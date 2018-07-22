//Author: Davide Cruz
//Tester: Davide Cruz

#include "mksofs.h"
#include "mksofs.bin.h"

#include "datatypes.h"
#include "rawdisk.h"
#include "exception.h"

#include <errno.h>
#include <stdio.h>

/* see mksofs.h for a description */
void fillInRootDir(uint32_t rtstart)
{
    //fillInRootDirBin(rtstart);

    //Um cluster tem um size de 2048 (BLOCK_SIZE*4). Um DirEntry tem 64 (sizeof(SODirEntry)).
    //Logo existem 32 DirEntry's num cluster (ClusterSize/sizeof(SODirEntry))
    //O primeiro DirEntry é o '.'. O segundo DirEntry é o '..'. Assume-se como enunciado no pdf, que o inode associado destes (root) é o 0.
    //A função escreve os primeiros dois DirEntry e enche o resto do cluster com DirEntry's vazios (name->zeros, inode->NullReference).
    SODirEntry dirRoot1; //DirEntry de '.'
    SODirEntry dirRoot2; //DirEntry de '..'
    SODirEntry rootEmpty; //DirEntry vazia com inode a NullReference
    SODirEntry* arrayDirRoot; //Array de DirEntry com '.' e '..' para escrita no bloco do disco
    SODirEntry* arrayDirEmpty; //Array de DirEntry vazios para escrita no bloco do disco

    strcpy(dirRoot1.name,".");
    for(int i=1; i<SOFS17_MAX_NAME+1; i++) //Encher o resto do char[] com 0's
      dirRoot1.name[i] = 0;
    dirRoot1.in = 0;
    strcpy(dirRoot2.name,"..");
    for(int i=2; i<SOFS17_MAX_NAME+1; i++) //Encher o resto da string com 0's
      dirRoot2.name[i] = 0;
    dirRoot2.in = 0;
    std::fill(rootEmpty.name,rootEmpty.name+sizeof(rootEmpty.name),0); //Encher o char[] com 0's
    rootEmpty.in = NullReference;

    // A preparar arrays(alocados na heap) para a escrita no disco.
    arrayDirRoot = new SODirEntry[BlockSize/sizeof(SODirEntry)];
    arrayDirRoot[0] = dirRoot1;
    arrayDirRoot[1] = dirRoot2;
    for(unsigned int i = 2; i < sizeof(arrayDirRoot); i++)
      arrayDirRoot[i] = rootEmpty;

    arrayDirEmpty = new SODirEntry[BlockSize/sizeof(SODirEntry)];
    for(unsigned int i = 0; i < sizeof(arrayDirEmpty); i++)
      arrayDirEmpty[i] = rootEmpty;

    //Escrever os blocos do Cluster no disco
    for(unsigned int i = 0; i < BlocksPerCluster; i++){
      if (i==0){
        soWriteRawBlock(rtstart, arrayDirRoot);
      }
      else
        soWriteRawBlock(rtstart+i, arrayDirEmpty);
    }
    //Delete arrays
    delete[] arrayDirRoot;
    delete[] arrayDirEmpty;

}
