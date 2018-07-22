/**
 *  \brief librarian module.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef LIBRARIAN_H
#define LIBRARIAN_H

// export to simulation:
void initLibrarian(int line, int column);
void destroyLibrarian();
int logIdLibrarian();
void* mainLibrarian(void* arg);
int lengthLibrarian();
void reqTermination();

// export to students:
void reqEnrollStudent();
void reqDisenrollStudent();
void reqCollectBooks();
int reqBookRequisition(struct _Book_** books);

#endif
