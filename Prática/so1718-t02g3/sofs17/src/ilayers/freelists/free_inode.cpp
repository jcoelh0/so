/**
 *  \author João Coelho
 *  \tester João Coelho
 */

#include "freelists.h"
#include "freelists.bin.h"

#include "probing.h"
#include "exception.h"

#include "sbdealer.h"
#include "itdealer.h"

#include <errno.h>
#include <inttypes.h>


/*
 * Dictates to be obeyed by the implementation:
 * - parameter in must be validated, 
 *      throwing a proper error if necessary
 */
void soFreeInode(uint32_t in)
{
    soProbe(532, "soFreeInode(%" PRIu32 ")\n", in);
    //soFreeInodeBin(in);
	
	SOSuperBlock *sb = sbGetPointer();
   	int inodeh = iOpen(in);	
    SOInode* inode = iGetPointer(inodeh);
    
	//Verifica se o indice do in é válido
    if (in >= sb->itotal || in == 0)
		throw SOException(EINVAL, __FUNCTION__);	

    if ((inode->mode | INODE_FREE) == inode->mode) return;
    
	//Se a lista estiver vazia
    if (sb->ifree == 0){
     	sb->ihead = in;
     	
		//Preenchimento dos campos do inode 
      	inode->mode = inode->mode | INODE_FREE;
      	inode->next = in;
      	inode->prev = in;
      	
      	//Atualizar contagem do número de inode livres
      	sb->ifree++;
	}
    else{
		inode->mode = inode->mode | INODE_FREE;
		
		//Abertura do Handler do inode
      	int iHead_handler = iOpen(sb->ihead);
      	//Carregar ponteiro inode
      	SOInode* pointer_inode = iGetPointer(iHead_handler);
      	//Abrir inode tail
      	int iTail_handler = iOpen(pointer_inode->prev);
      	//Carregar o inode no fim da lista
    	SOInode* pointer_iTail = iGetPointer(iTail_handler);
		//Colocar o inode libertado a seguir ao último inode da lista de inodes lives
  		pointer_iTail->next = in;
  		
   		inode->prev = pointer_inode->prev;
      	inode->next = sb->ihead;
      	pointer_inode->prev = in;
      	
      	//Guardar inodes
      	iSave(iTail_handler);
    	iSave(iHead_handler);
      	iSave(inodeh);
      	
      	//Atualizar contagem do número de inode livres
      	sb->ifree++;
	}
	sbSave();
}
