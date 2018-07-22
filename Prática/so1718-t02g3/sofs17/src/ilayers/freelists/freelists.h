/**
 * \addtogroup ilayers
 *
 * @{
 *
 * \defgroup freelists freelists
 *
 * @{
 *
 * \brief Functions to manage the list of free inodes 
 *      and the list of free clusters
 *
 *  \author Artur Pereira 2008-2009, 2016-2017
 *  \author Miguel Oliveira e Silva 2009, 2017
 *  \author António Rui Borges - 2010-2015
 *
 *  \remarks In case an error occurs, every function throws an SOException
 */

#ifndef __SOFS17_FREELISTS__
#define __SOFS17_FREELISTS__

#include <stdint.h>

/**
 *  \brief Allocate a free inode.
 *
 *  An inode is retrieved from the list of free inodes, marked in use, 
 *  associated to the legal file type passed as
 *  a parameter and is generally initialized. 
 *
 *  \param type the inode type (it must represent either a file, or a directory, 
 *      or a symbolic link)
 *  \return the number of the allocated inode
 */
uint32_t soAllocInode(uint32_t type);

/**
 *  \brief Free the referenced inode.
 *
 *  The given inode must be marked as free, and inserted into the list of free inodes.
 *  No other changes should be done to the inode.
 *
 *  \param in number of the inode to be freed
 */
void soFreeInode(uint32_t in);

/* *************************************************** */

/**
 *  \brief Allocate a free cluster.
 *
 *  A cluster is retrieved from the retrieval cache. 
 *  If the cache is empty, it has to be replenished before the operation takes place.
 *
 *  \return the number of the allocated cluster
 */
uint32_t soAllocCluster();

/* *************************************************** */

/**
 * \brief replenish the retrieval cache
 *
 *  References to free clusters should be transfered from the free cluster table
 *  (bit map) or insertion cache to the retrieval cache.
 *
 *  \li Nothing should be done if the retrieval cache is not empty.
 *  \li The insertion cache should only be used if there are no bits at one in the bit map blocks.
 *  \li Only a single block should be processed, even if it is not enough to fulfill the
 *          retrieval cache.
 *  \li When a reference is transfered, the previous location should be filled as \c NullReference.
 *  \li Field \c rmidx of the superblock indicated the block and byte within that block
 *      where the transference should start from:
 *      the value given by <code>rmidx / ReferenceBytesPerBitmapBlock</code> 
 *      indicates the block and <code>rmidx % ReferenceBytesPerBitmapBlock</code> the byte.
 *  \li Field \c rmidx of the superblock should be updated at the end of this operation.
 *  \li Field \c rmidx of the superblock should be \c NullReference when the bit map is empty.
 */
void soReplenish();

/* *************************************************** */

/**
 *  \brief Free the referenced cluster.
 *
 *  The given cluster reference should be inserted into the insertion cache.
 *  If the cache is full, it has to be depleted before the operation takes place.
 *
 *  \param cn the number (reference) of the cluster to be freed
 */
void soFreeCluster(uint32_t cn);

/* *************************************************** */

/**
 * \brief Deplete the insertion cache
 *
 *  All the references in the insertion cache should be transfered to the free cluster table 
 *  (bit map).
 *
 *  \li When a reference is transferred,
 *      the previous location should be filled as NullReference
 *  \li Field \c rmidx of the superblock should be updated, keeping the associated block but
 *          pointing to the first byte with ones.
 *
 */
void soDeplete();

/* *************************************************** */
/** @} closing group freelists */
/** @} closing group ilayers */
/* *************************************************** */

#endif				/* __SOFS17_FREELISTS__ */
