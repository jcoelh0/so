/**
 *  \brief Library module.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef LIBRARY_H
#define LIBRARY_H

#include "book.h"

// export to simulation:
int totalSizeOfLibraryDataStructure();
void initLibrary();
void destroyLibrary();
int numLinesLibrary();

// export to librarian:
int booksAvailableInLibrary(struct _Book_** books);
void requisiteBooksFromLibrary(struct _Book_** books);
int numSeats();
int validSeatPosition(int pos);
int seatOccupied(int pos);
int booksInSeat(int pos);
void collectBooksLibrary(int pos);

// export to all-course:
struct _Book_** randomBookListFromLibrary(struct _Book_** result, int n);

// export to students:
int seatAvailable();
int sit(struct _Book_** books);
void rise(int pos);

#endif
