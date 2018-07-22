#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "utils.h"
#include "sim-alloc.h"
#include "global.h"
#include "logger.h"
#include "library.h"
#include "thread.h"

typedef struct _Library_
{
   struct _Book_** bookList;
   int* bookQuantity;
   int* bookAvailable;
   int* seatOccupied;
   int booksInTransit; // not in the bookshelf or in table
   struct _Book_*** booksInSeat;
   int logIdBookShelf;
   int logIdTables;
   /* TODO: change this structure to your needs */

} Library;

static const char* descText = "Library:";
static Library* library = NULL;

static void allocLibraryDataStructure();
static void freeLibraryDataStructure();
static void invariantLibrary();
static int booksExistsInLibrary(struct _Book_** books);
static int booksRequisited(struct _Book_** books);
static void returnBooks(struct _Book_** books);
static char* toStringBookShelfs();
static char* toStringTables();
static int getSeat();
static int getRandomSeat();
static int lengthBookShelf();
static int lengthTable();
static int lengthAllTables();
static int bookSearch(struct _Book_* book);

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int totalSizeOfLibraryDataStructure()
{
   return sizeof(Library) + totalSizeOfBook()*sizeof(struct _Book_*)*global->NUM_BOOKS +
          2*sizeof(int)*global->NUM_BOOKS + sizeof(int)*(global->NUM_TABLES*global->NUM_SEATS_PER_TABLE) +
          sizeof(struct _Book_*)*(global->NUM_TABLES*global->NUM_SEATS_PER_TABLE);
}

static void allocLibraryDataStructure()
{
   /* TODO: change this function to your needs */

   assert (library == NULL);
   
   library = (Library*)shmAlloc(sizeof(Library));
   library->bookList = (struct _Book_**)shmAlloc(sizeof(struct _Book_*)*global->NUM_BOOKS);
   for (int i = 0; i < global->NUM_BOOKS; i++)
      library->bookList[i] = (struct _Book_*)shmAlloc(totalSizeOfBook());
   library->bookQuantity = (int*)shmAlloc(sizeof(int)*global->NUM_BOOKS);
   library->bookAvailable = (int*)shmAlloc(sizeof(int)*global->NUM_BOOKS);
   library->seatOccupied = (int*)shmAlloc(sizeof(int)*global->NUM_TABLES*global->NUM_SEATS_PER_TABLE);
   library->booksInSeat = (struct _Book_***)shmAlloc(sizeof(struct _Book_**)*global->NUM_TABLES*global->NUM_SEATS_PER_TABLE);
   
}

static void freeLibraryDataStructure()
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   assert (library != NULL);
      
   free(library->booksInSeat);
   free(library->seatOccupied);
   free(library->bookAvailable);
   free(library->bookQuantity);
   for (int i = 0; i < global->NUM_BOOKS; i++)
      free(library->bookList[i]);
   free(library->bookList);
   free(library);
   library = NULL;
   
   pthread_mutex_unlock(&mutex);
}

void initLibrary()
{
   /* TODO: change this function to your needs */

   if (library == NULL)
      allocLibraryDataStructure();
      
   char n[3];
   strcpy(n, "Bx");
   for (int i = 0; i < global->NUM_BOOKS; i++)
   {
      n[1] = (char)((int)'a'+i);
      library->bookList[i] = newBook(library->bookList[i], n);
      library->bookAvailable[i] = library->bookQuantity[i] = randomInt(global->MIN_BOOK_COPIES, global->MAX_BOOK_COPIES);
   }

   for (int i = 0; i < numSeats(); i++)
   {
      library->seatOccupied[i] = 0;
      library->booksInSeat[i] = NULL;
   }
   library->booksInTransit = 0;

   static const char* bookShelfLineModeTranslations[] = {
      "Library:", "",
      NULL
   };

   library->logIdBookShelf = registerLogger((char*)"BookShelf:", 0 ,0 , 9, lengthBookShelf(), (char**)bookShelfLineModeTranslations);
   static const char* tablesLineModeTranslations[] = {
      FACE_PENSIVE, "SEAT_TAKEN",
      BLACK_RECTANGLE, "SEAT_EMPTY",
      BOOK_OPEN, "BOOKS_OPEN",
      BOOKS, "BOOKS_LEFT",
      NULL
   };
   
   library->logIdTables = registerLogger((char*)"Tables:", 2 ,lengthBookShelf()+2 , 6, lengthAllTables(), (char**)tablesLineModeTranslations);
   sendLog(library->logIdBookShelf, toStringBookShelfs());
   sendLog(library->logIdTables, toStringTables());

      printf("Options:\n");
   invariantLibrary();
      printf("Options:\n");

}

void destroyLibrary()
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);
   freeLibraryDataStructure();
   pthread_mutex_unlock(&mutex);
}

int booksAvailableInLibrary(struct _Book_** books)
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   assert (books != NULL);
   assert (booksExistsInLibrary(books));

   int res = 1;
   for(int i = 0; res && books[i] != NULL; i++)
      res = library->bookAvailable[bookSearch(books[i])] > 0;

   return res;
   
   pthread_mutex_unlock(&mutex);
}

void requisiteBooksFromLibrary(struct _Book_** books)
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   assert (books != NULL);
   assert (booksAvailableInLibrary(books));

   for(int i = 0; books[i] != NULL; i++)
   {
      int idx = bookSearch(books[i]);
      library->bookAvailable[idx]--;
      library->booksInTransit++;
   }
   sendLog(library->logIdBookShelf, toStringBookShelfs());

   invariantLibrary();
   
   pthread_mutex_unlock(&mutex);

}

struct _Book_** randomBookListFromLibrary(struct _Book_** result, int n)
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   assert (n >= 1 && n < global->NUM_BOOKS);
   
   if (result == NULL)
      result = (struct _Book_**)shmAlloc((n+1)*sizeof(struct _Book_*));
   for(int i = 0; i < n; i++)
   {
      int exists;
      int idx;
      do
      {
         idx = randomInt(0,global->NUM_BOOKS-1);
         exists = 0;
         for(int j = 0; !exists && j < i; j++)
            exists = (result[j] == library->bookList[idx]);
      }
      while(exists);
      result[i] = library->bookList[idx];
   }
   
   pthread_mutex_unlock(&mutex);

   result[n] = NULL;
   return result;
}

int validSeatPosition(int pos)
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   int res;
   res = pos >= 0 && pos < numSeats();
   pthread_mutex_unlock(&mutex);

   return res;
}

int seatOccupied(int pos)
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   assert (validSeatPosition(pos));

   int res;
   res = library->seatOccupied[pos];
   pthread_mutex_unlock(&mutex);

   return res;
}

int seatAvailable()
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   int res;
   res = getSeat() < numSeats();
   pthread_mutex_unlock(&mutex);

   return res;
}

int booksInSeat(int pos)
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   assert (validSeatPosition(pos));

   int res;
   res = library->booksInSeat[pos] != NULL;
   pthread_mutex_unlock(&mutex);

   return res;
}

int sit(struct _Book_** books)
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   assert (books != NULL);
   assert (booksExistsInLibrary(books));
   assert (seatAvailable());

   int res;
   res = getRandomSeat();
   library->seatOccupied[res] = 1;
   library->booksInSeat[res] = books;
   library->booksInTransit -= bookListLength(books);
   sendLog(library->logIdTables, toStringTables());

   invariantLibrary();
   pthread_mutex_unlock(&mutex);

   return res;
}

void rise(int pos)
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   assert (validSeatPosition(pos));
   assert (seatOccupied(pos));

   library->seatOccupied[pos] = 0;
   sendLog(library->logIdTables, toStringTables());

   invariantLibrary();
   pthread_mutex_unlock(&mutex);

}

void collectBooksLibrary(int pos)
{
   /* TODO: change this function to your needs */
   pthread_mutex_lock(&mutex);

   assert (validSeatPosition(pos));
   assert (booksInSeat(pos));

   returnBooks(library->booksInSeat[pos]);
   library->booksInSeat[pos] = NULL;
   sendLog(library->logIdTables, toStringTables());

   invariantLibrary();

   pthread_mutex_unlock(&mutex);
}

int numLinesLibrary()
{
   return getNumLinesLogger(library->logIdBookShelf);
}

int numSeats()
{
   return global->NUM_TABLES*global->NUM_SEATS_PER_TABLE;
}

static void invariantLibrary()
{
   int initialBooks = 0;
   int dynamicBooks = library->booksInTransit;
   for (int i = 0; i < global->NUM_BOOKS; i++)
   {
      initialBooks += library->bookQuantity[i];
      dynamicBooks += library->bookAvailable[i];
   }
   for (int i = 0; i < numSeats(); i++)
   {
      struct _Book_** books = library->booksInSeat[i];
      if (library->booksInSeat[i] != NULL)
      {
         while(*books != NULL)
         {
            dynamicBooks++;
            books++;
         }
      }
   }

   if (lineMode())
   {
      if (!(library->booksInTransit >= 0))
         printf("[Library - invariant]: (booksInTransit:%d >= 0)\n", library->booksInTransit);
      if (!(initialBooks >= 1))
         printf("[Library - invariant]: (initialBooks:%d >= 1)\n", initialBooks);
      if (!(dynamicBooks >= 0))
         printf("[Library - invariant]: (dynamicBooks:%d >= 0)\n", dynamicBooks);
      if (!(initialBooks == dynamicBooks))
         printf("[Library - invariant]: initialBooks:%d == dynamicBooks:%d\n", initialBooks, dynamicBooks);
   }
   assert (library->booksInTransit >= 0);
   assert (initialBooks >= 1);
   assert (dynamicBooks >= 0);
   assert (initialBooks == dynamicBooks);
}

static int booksExistsInLibrary(struct _Book_** books)
{
   assert (books != NULL);

   int res = 1;
   for(int i = 0; res && books[i] != NULL; i++)
      res = bookSearch(books[i]) < global->NUM_BOOKS;

   return res;
}

static int booksRequisited(struct _Book_** books)
{
   assert (books != NULL);
   assert (booksExistsInLibrary(books));

   int res = 1;
   for(int i = 0; res && books[i] != NULL; i++)
   {
      int idx = bookSearch(books[i]);
      res = library->bookAvailable[idx] < library->bookQuantity[idx];
   }

   return res;
}

static void returnBooks(struct _Book_** books)
{
   assert (books != NULL);
   assert (booksExistsInLibrary(books));
   assert (booksRequisited(books));

   for(int i = 0; books[i] != NULL; i++)
   {
      int idx = bookSearch(books[i]);
      library->bookAvailable[idx]++;
   }
   sendLog(library->logIdBookShelf, toStringBookShelfs());
}

static char* toStringBookShelfs()
{
   char* res = (char*)"";

   res = concat_string_in_stack(res, descText);
   res = concat_string_in_stack(res, "\n");
   int half = global->NUM_BOOKS/2;
   // top
   res = concat_string_in_stack(res, BOX_TOP_LEFT);
   for(int i = 0; i < half; i++)
   {
      res = concat_string_in_stack(res, utf8HorizontalLine(5));
      if (i < half-1)
         res = concat_string_in_stack(res, BOX_HORIZONTAL_DOWN);
   }
   res = concat_string_in_stack(res, BOX_TOP_RIGHT);
   res = concat_string_in_stack(res, "\n");
   res = concat_string_in_stack(res, BOX_VERTICAL);
   for(int i = 0; i < half; i++)
   {
      res = concat_string_in_stack(res, nameBook(library->bookList[i]));
      res = concat_string_in_stack(res, ":");
      res = concat_string_in_stack(res, intToString((char*)alloca(3), library->bookAvailable[i], 2));
      res = concat_string_in_stack(res, BOX_VERTICAL);
   }
   res = concat_string_in_stack(res, "\n");
   res = concat_string_in_stack(res, BOX_BOTTOM_LEFT);
   for(int i = 0; i < half; i++)
   {
      for(int j = 0; j < 5; j++)
         res = concat_string_in_stack(res, " ");
      if (i < half-1)
         res = concat_string_in_stack(res, BOX_HORIZONTAL_UP);
   }
   res = concat_string_in_stack(res, BOX_BOTTOM_RIGHT);
   res = concat_string_in_stack(res, "\n\n\n");

   // bottom
   res = concat_string_in_stack(res, BOX_TOP_LEFT);
   for(int i = half; i < global->NUM_BOOKS; i++)
   {
      for(int j = 0; j < 5; j++)
         res = concat_string_in_stack(res, " ");
      if (i < global->NUM_BOOKS-1)
         res = concat_string_in_stack(res, BOX_HORIZONTAL_DOWN);
   }
   res = concat_string_in_stack(res, BOX_TOP_RIGHT);
   res = concat_string_in_stack(res, "\n");
   res = concat_string_in_stack(res, BOX_VERTICAL);
   for(int i = half; i < global->NUM_BOOKS; i++)
   {
      res = concat_string_in_stack(res, nameBook(library->bookList[i]));
      res = concat_string_in_stack(res, ":");
      res = concat_string_in_stack(res, intToString((char*)alloca(3), library->bookAvailable[i], 2));
      res = concat_string_in_stack(res, BOX_VERTICAL);
   }
   res = concat_string_in_stack(res, "\n");
   res = concat_string_in_stack(res, BOX_BOTTOM_LEFT);
   for(int i = half; i < global->NUM_BOOKS; i++)
   {
      res = concat_string_in_stack(res, utf8HorizontalLine(5));
      if (i < global->NUM_BOOKS-1)
         res = concat_string_in_stack(res, BOX_HORIZONTAL_UP);
   }
   res = concat_string_in_stack(res, BOX_BOTTOM_RIGHT);
   res = concat_string_in_stack(res, "\n");

   return stringClone(res);
}

static char* toStringTables()
{
   char* res = (char*)"";

   for(int t = 0; t < global->NUM_TABLES; t++)
   {
      if (t > 0)
         res = concat_string_in_stack(res, "  ");
      for(int s = 0; s < global->NUM_SEATS_PER_TABLE/2; s++)
      {
         int idx = t*(global->NUM_SEATS_PER_TABLE/2)+s;
         if (idx < numSeats() && library->seatOccupied[idx])
            res = concat_string_in_stack(res, " "FACE_PENSIVE);
         else
            res = concat_string_in_stack(res, " "BLACK_RECTANGLE);
      }
      res = concat_string_in_stack(res, " ");
   }
   res = concat_string_in_stack(res, "\n");
   for(int t = 0; t < global->NUM_TABLES; t++)
   {
      if (t > 0)
         res = concat_string_in_stack(res, "  ");
      res = concat_string_in_stack(res, BOX_TOP_LEFT);
      res = concat_string_in_stack(res, utf8HorizontalLine(lengthTable()-2));
      res = concat_string_in_stack(res, BOX_TOP_RIGHT);
   }
   res = concat_string_in_stack(res, "\n");
   for(int p = 0; p < 2; p++) // to insert position to place books!
   {
      for(int t = 0; t < global->NUM_TABLES; t++)
      {
         if (t > 0)
            res = concat_string_in_stack(res, "  ");
         res = concat_string_in_stack(res, BOX_VERTICAL);
         //for(int i = 0; i < lengthTable()-2; i++)
         for(int s = 0; s < global->NUM_SEATS_PER_TABLE/2; s++)
         {
            int idx = p*global->NUM_TABLES*global->NUM_SEATS_PER_TABLE/2 + t*(global->NUM_SEATS_PER_TABLE/2)+s;
            if (idx < numSeats() && booksInSeat(idx))
            {
               if (library->seatOccupied[idx])
                  res = concat_string_in_stack(res, BOOK_OPEN);
               else
                  res = concat_string_in_stack(res, BOOKS);
            }
            else
               res = concat_string_in_stack(res, "  ");
            if (s < global->NUM_SEATS_PER_TABLE/2 - 1)
               res = concat_string_in_stack(res, " ");
         }
         res = concat_string_in_stack(res, BOX_VERTICAL);
      }
      res = concat_string_in_stack(res, "\n");
   }
   for(int t = 0; t < global->NUM_TABLES; t++)
   {
      if (t > 0)
         res = concat_string_in_stack(res, "  ");
      res = concat_string_in_stack(res, BOX_BOTTOM_LEFT);
      res = concat_string_in_stack(res, utf8HorizontalLine(lengthTable()-2));
      res = concat_string_in_stack(res, BOX_BOTTOM_RIGHT);
   }
   res = concat_string_in_stack(res, "\n");
   for(int t = 0; t < global->NUM_TABLES; t++)
   {
      if (t > 0)
         res = concat_string_in_stack(res, "  ");
      for(int s = 0; s < global->NUM_SEATS_PER_TABLE/2; s++)
      {
         int idx = global->NUM_TABLES*global->NUM_SEATS_PER_TABLE/2 + t*(global->NUM_SEATS_PER_TABLE/2)+s;
         if (idx < numSeats() && library->seatOccupied[idx])
            res = concat_string_in_stack(res, " "FACE_PENSIVE);
         else
            res = concat_string_in_stack(res, " "BLACK_RECTANGLE);
      }
      res = concat_string_in_stack(res, " ");
   }
   res = concat_string_in_stack(res, "\n");

   return stringClone(res);
}

static int getSeat()
{
   int res;
   for(res = 0; res < numSeats() && (library->seatOccupied[res] || booksInSeat(res)); res++)
      ;
   return res;
}

static int getRandomSeat()
{
   int res = -1;
   int count = randomInt(1, numSeats());
   for(int c = 0; c < count; c++)
   {
      res = (res+1) % numSeats();
      while(library->seatOccupied[res] || booksInSeat(res))
         res = (res+1) % numSeats();
   }
   if (library->seatOccupied[res] || booksInSeat(res))
      res = numSeats();
   return res;
}


static int lengthBookShelf()
{
   int res = strlen(descText);
   if (res < ((global->NUM_BOOKS+1)/2*6)+1)
      res = ((global->NUM_BOOKS+1)/2*6)+1;
   return res;
}

static int lengthTable()
{
   return (global->NUM_SEATS_PER_TABLE/2)*3+1;
}

/*
 * one table with 4 seats:
 *  -- -- 
 * |     |
 *  -- -- 
 */
static int lengthAllTables()
{
   return global->NUM_TABLES*lengthTable() + /* table interval: */ (global->NUM_TABLES-1)*2;
   //return 2*((numSeats()+numSeats()%2)/2)+1;
}

static int bookSearch(struct _Book_* book)
{
   int res;
   for(res = 0; res < global->NUM_BOOKS && book != library->bookList[res]; res++)
      ;

   return res;
}

