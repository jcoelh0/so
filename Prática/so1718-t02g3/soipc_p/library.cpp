#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "utils.h"
#include "sim-alloc.h"
#include "global.h"
#include "logger.h"
#include "library.h"
#include "process.h"

typedef struct _Library_
{
   struct _Book_ bookList[MAX_BOOKS]; // global->NUM_BOOKS
   int bookQuantity[MAX_BOOKS];
   int bookAvailable[MAX_BOOKS];
   int seatOccupied[MAX_SEATS];
   int booksInTransit; // not in the bookshelf or in table
   struct _Book_ booksInSeat[MAX_SEATS][MAX_BOOKS];
   int numBooksInSeat[MAX_SEATS]; // new
   int logIdBookShelf;
   int logIdTables;
   /* TODO: change this structure to your needs */

} Library;

//structure for shared memory to use for messages to logger
typedef struct _ShmLogMessage_
{
   int logId;
   char text[1000];
} ShmLogMessage;

//Variables for shared memory
key_t key3 = 11; //ShmLogMessage
char* shmAddr3;
int shmid3;

//Semaphores for message System - logger
sem_t* log_Disp3;
sem_t* log_Copied3;

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


int totalSizeOfLibraryDataStructure()
{
   return sizeof(Library);
}

static void allocLibraryDataStructure()
{
   /* TODO: change this function to your needs */

   assert (library == NULL);

   library = (Library*)shmAlloc(sizeof(Library));
}

static void freeLibraryDataStructure()
{
   /* TODO: change this function to your needs */

   assert (library != NULL);

   library = NULL;
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
      newBook(&library->bookList[i], n);
      library->bookAvailable[i] = library->bookQuantity[i] = randomInt(global->MIN_BOOK_COPIES, global->MAX_BOOK_COPIES);
   }
   for (int i = 0; i < numSeats(); i++)
   {
      library->seatOccupied[i] = 0;
      library->numBooksInSeat[i] = 0;
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

   invariantLibrary();

   //Attach shared memory
   shmid3 = pshmget(key3, sizeof(ShmLogMessage), IPC_CREAT | 0666);
   shmAddr3 =(char*) pshmat(shmid3,0,0);

   //Inicialize semaphores
   log_Disp3 = psem_open("/logDisp", O_CREAT, 0644);
   log_Copied3 = psem_open("/logCopied", O_CREAT, 0644);
}

void destroyLibrary()
{
   /* TODO: change this function to your needs */

   freeLibraryDataStructure();

   //Destroy Shared memory
   shmdt (shmAddr3);

   //Terminate Semaphores
   psem_close(log_Disp3);
   psem_close(log_Copied3);
}

static void sendLogMessage(int logId, char* text){
  psem_wait(log_Copied3);
  ShmLogMessage* temp;
  temp = (ShmLogMessage*) malloc(sizeof(ShmLogMessage));
  temp->logId = logId;
  strcpy(temp->text,text);
  memcpy(shmAddr3,&temp,sizeof(ShmLogMessage));
  free(temp);
  psem_post(log_Disp3);
}

int booksAvailableInLibrary(struct _Book_** books)
{
   /* TODO: change this function to your needs */

   assert (books != NULL);
   assert (booksExistsInLibrary(books));
   int res = 1;
   for(int i = 0; res && books[i] != NULL; i++)
      res = library->bookAvailable[bookSearch(books[i])] > 0;

   return res;
}

void requisiteBooksFromLibrary(struct _Book_** books)
{
   /* TODO: change this function to your needs */

   assert (books != NULL);
   assert (booksAvailableInLibrary(books));

   for(int i = 0; books[i] != NULL; i++)
   {
      int idx = bookSearch(books[i]);
      library->bookAvailable[idx]--;
      library->booksInTransit++;
   }
   sendLogMessage(library->logIdBookShelf, toStringBookShelfs());

   invariantLibrary();
}

struct _Book_** randomBookListFromLibrary(struct _Book_** result, int n)
{
   /* TODO: change this function to your needs */

   assert (n >= 1 && n < global->NUM_BOOKS);

   if (result == NULL)
   {
      result = (struct _Book_**)shmAlloc((n+1)*sizeof(struct _Book_*));
      for(int i = 0; i < n; i++)
         result[i] = (struct _Book_*)memAlloc(totalSizeOfBook());
   }
   for(int i = 0; i < n; i++)
   {
      int exists;
      int idx;
      do
      {
         idx = randomInt(0,global->NUM_BOOKS-1);
         exists = 0;
         for(int j = 0; !exists && j < i; j++)
            exists = equalBook(result[j], &library->bookList[idx]);
      }
      while(exists);
      *(result[i]) = library->bookList[idx];
   }
   result[n] = NULL;
   return result;
}

int validSeatPosition(int pos)
{
   /* TODO: change this function to your needs */

   int res;
   res = pos >= 0 && pos < numSeats();
   return res;
}

int seatOccupied(int pos)
{
   /* TODO: change this function to your needs */

   assert (validSeatPosition(pos));

   int res;
   res = library->seatOccupied[pos];
   return res;
}

int seatAvailable()
{
   /* TODO: change this function to your needs */

   int res;
   res = getSeat() < numSeats();
   return res;
}

int booksInSeat(int pos)
{
   /* TODO: change this function to your needs */

   assert (validSeatPosition(pos));

   int res;
   res = library->numBooksInSeat[pos] > 0;
   return res;
}

int sit(struct _Book_** books)
{
   /* TODO: change this function to your needs */

   assert (books != NULL);
   assert (booksExistsInLibrary(books));
   assert (seatAvailable());

   int res;
   res = getRandomSeat();
   library->seatOccupied[res] = 1;
   for(int i = 0; books[i] != NULL; i++)
   {
      library->booksInSeat[res][i] = *(books[i]);
      library->numBooksInSeat[res]++;
   }
   library->booksInTransit -= bookListLength(books);
   sendLogMessage(library->logIdTables, toStringTables());

   invariantLibrary();

   return res;
}

void rise(int pos)
{
   /* TODO: change this function to your needs */

   assert (validSeatPosition(pos));
   assert (seatOccupied(pos));

   library->seatOccupied[pos] = 0;
   sendLogMessage(library->logIdTables, toStringTables());

   invariantLibrary();
}

void collectBooksLibrary(int pos)
{
   /* TODO: change this function to your needs */

   assert (validSeatPosition(pos));
   assert (booksInSeat(pos));

   struct _Book_**books = (struct _Book_**)alloca(sizeof(struct _Book_*)*(library->numBooksInSeat[pos]+1));
   for(int i = 0; i < library->numBooksInSeat[pos]; i++)
      books[i] = &library->booksInSeat[pos][i];
   books[library->numBooksInSeat[pos]] = NULL;
   returnBooks(books);
   library->numBooksInSeat[pos] = 0;
   sendLogMessage(library->logIdTables, toStringTables());

   invariantLibrary();
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
      dynamicBooks += library->numBooksInSeat[i];

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
   sendLogMessage(library->logIdBookShelf, toStringBookShelfs());
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
      res = concat_string_in_stack(res, nameBook(&library->bookList[i]));
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
      res = concat_string_in_stack(res, nameBook(&library->bookList[i]));
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
   for(res = 0; res < global->NUM_BOOKS && !equalBook(book, &library->bookList[res]); res++)
      ;

   return res;
}
