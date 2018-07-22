#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "utils.h"
#include "sim-alloc.h"
#include "global.h"
#include "course-unit.h"

typedef struct _CourseUnit_
{
   int alloc;
   struct _Book_** bookList;
   char* name;
   int* minStudyBookTimeUnits;
} CourseUnit;

static int searchBook(CourseUnit* courseUnit, struct _Book_* book);

CourseUnit* newCourseUnit(CourseUnit* courseUnit, char* name, struct _Book_** bookList)
{
   assert (name != NULL && strlen(name) > 0);
   assert (bookList != NULL); 

   if (courseUnit == NULL)
   {
      courseUnit = (CourseUnit*)shmAlloc(sizeof(CourseUnit));
      courseUnit->alloc = 1;
   }
   else
      courseUnit->alloc = 0;
   courseUnit->name = name;
   courseUnit->bookList = bookList;
   int len = bookListLength(bookList);
   courseUnit->minStudyBookTimeUnits = (int*)shmAlloc(sizeof(int)*len);
   for(int i = 0; i < len; i++)
      courseUnit->minStudyBookTimeUnits[i] = randomInt(global->MIN_BOOK_STUDY_TIME_UNITS, global->MAX_BOOK_STUDY_TIME_UNITS);
   return courseUnit;
}

CourseUnit* destroyCourseUnit(CourseUnit* courseUnit)
{
   assert (courseUnit != NULL);

   free(courseUnit->minStudyBookTimeUnits);
   if (courseUnit->alloc)
   {
      free(courseUnit);
      courseUnit = NULL;
   }
   else
   {
      courseUnit->minStudyBookTimeUnits = NULL;
   }
   return courseUnit;
}

char* nameCourseUnit(CourseUnit* courseUnit)
{
   assert (courseUnit != NULL);

   return courseUnit->name;
}

struct _Book_** bookListCourseUnit(CourseUnit* courseUnit)
{
   assert (courseUnit != NULL);
   return courseUnit->bookList; // should be a copy!
}

int containsBookCourseUnit(CourseUnit* courseUnit, struct _Book_* book)
{
   assert (courseUnit != NULL);
   assert (book != NULL);

   return searchBook(courseUnit, book) < bookListLength(courseUnit->bookList);
}

int containsAllBooksCourseUnit(CourseUnit* courseUnit, struct _Book_** books)
{
   assert (courseUnit != NULL);
   assert (books != NULL);

   int res = 1;
   for(int i = 0; res && books[i] != NULL; i++)
      res = containsBookCourseUnit(courseUnit, books[i]);

   return res;
}

int minStudyBookTimeUnitsCourseUnit(CourseUnit* courseUnit, struct _Book_* book)
{
   assert (courseUnit != NULL);
   assert (book != NULL);
   assert (containsBookCourseUnit(courseUnit, book));

   return courseUnit->minStudyBookTimeUnits[searchBook(courseUnit, book)];
}

int completionPercentageCourseUnit(CourseUnit* courseUnit, struct _Book_** books, int* timeSpent)
{
   assert (courseUnit != NULL);
   assert (books != NULL);
   assert (timeSpent != NULL);
   assert (bookListLength(books) == bookListLength(bookListCourseUnit(courseUnit)));
   assert (containsAllBooksCourseUnit(courseUnit, books));

   int res;
   double sum = 0;
   double sumGoal = 0;
   for(int i = 0; i < bookListLength(books); i++)
   {
      assert (courseUnit->minStudyBookTimeUnits[i] > 0);
      assert (timeSpent[i] >= 0);
      sum += timeSpent[i] >= courseUnit->minStudyBookTimeUnits[i] ? courseUnit->minStudyBookTimeUnits[i] : timeSpent[i];
      sumGoal += courseUnit->minStudyBookTimeUnits[i];
   }

   assert (sum <= sumGoal);

   res = (int)(100.0*sum/sumGoal);

   //printf("[completionPercentageCourseUnit] res: %d\n", res);

   assert (res >= 0 && res <= 100);

   return res;
}

static int searchBook(CourseUnit* courseUnit, struct _Book_* book)
{
   int res;
   struct _Book_** bookList = bookListCourseUnit(courseUnit);
   for(res = 0; bookList[res] != NULL && !equalBook(book, bookList[res]); res++)
      ;
   return res;
}

