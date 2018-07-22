#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "utils.h"
#include "book.h"

typedef struct _Book_
{
   int alloc;
   char name[MAX_NAME_SIZE+1];
} Book;

int totalSizeOfBook()
{
   return sizeof(Book);
}

Book* newBook(Book* book, char* name)
{
   assert (name != NULL && strlen(name) > 0);

   if (book == NULL)
   {
      book = (Book*)memAlloc(sizeof(Book));
      book->alloc = 1;
   }
   else
      book->alloc = 0;
   strncpy(book->name, name, MAX_NAME_SIZE+1);
   return book;
}

Book* destroyBook(Book* book)
{
   assert (book != NULL);

   if (book->alloc)
   {
      free(book);
      book = NULL;
   }
   return book;
}

char* nameBook(Book* book)
{
   assert (book != NULL);

   return book->name;
}

int equalBook(Book* book, Book* other)
{
   assert (book != NULL);
   assert (other != NULL);

   return strcmp(book->name, other->name) == 0;
}

int bookListLength(Book** list)
{
   assert (list != NULL);

   int res;
   for(res = 0; list[res] != NULL; res++)
      ;
   return res;
}

