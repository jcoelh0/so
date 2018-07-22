/**
 *  \file
 *  \brief Some global constants
 *
 *  \author Artur Pereira - 2016-2017
 */

#ifndef __SOFS17_DATATYPES__
#define __SOFS17_DATATYPES__

#include "rawdisk.h"
#include "superblock.h"
#include "inode.h"
#include "refblock.h"
#include "direntry.h"

#include <inttypes.h>

/** \addtogroup constants constants 
 * \brief Some useful constants
 */
/** \{ */

/** \brief number of inodes per block */
#define InodesPerBlock (BlockSize / sizeof(SOInode))

/** \brief number of references per block */
#define ReferencesPerBlock (BlockSize / sizeof (uint32_t))

/** \brief number of references per cluster */
#define ReferencesPerCluster (ClusterSize / sizeof (uint32_t))

/** \brief number of references per free cluster table (bitmap) block */
#define ReferencesPerBitmapBlock (8*ReferenceBytesPerBitmapBlock)

/** \brief number of blocks per cluster */
#define BlocksPerCluster 4

/** \brief number of bytes per cluster */
#define ClusterSize (BlocksPerCluster * BlockSize)

/** \brief number of direntries per cluster */
#define DirentriesPerCluster     (ClusterSize / sizeof(SODirEntry))

/** \brief null reference */
#define NullReference 0xFFFFFFFF

/** \} */

#endif				/* __SOFS17_DATATYPES__ */

