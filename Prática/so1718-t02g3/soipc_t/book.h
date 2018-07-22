/**
 *  \brief book module.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef BOOK_H
#define BOOK_H

#include "global.h"

struct _Book_; // private data structure

// export to library:
int totalSizeOfBook();
struct _Book_* newBook(struct _Book_* book, char* name);
struct _Book_* destroyBook(struct _Book_* book);

// export to library, students, and all-courses:
char* nameBook(struct _Book_* book);

// export to course-unit:
int equalBook(struct _Book_* book, struct _Book_* other);

// export to course-unit, students:
int bookListLength(struct _Book_** list);

#endif
