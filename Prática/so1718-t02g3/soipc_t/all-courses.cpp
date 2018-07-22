#include <stdlib.h>
#include <assert.h>
#include "utils.h"
#include "global.h"
#include "logger.h"
#include "library.h"
#include "all-courses.h"

static char* toStringAllCourses();

static const char* descText = "Courses:";
static int logId;
static struct _CourseUnit_** courses;
static int numCourses;
static const char* names[] = {
   "ALGA", "C1", "P1", "ISD", "LSD", "P2", "MD", "C2", "LI", "P3", "MCE", "AC1",
   "MPEI", "AC2", "LFA", "AC", "SE", "SO", "AMS", "IIA", "FR", "BD", "IHC", "AR",
   "PEI", "MO", "LE", "C3", "AN", "CE", "IAPS", "TFES", "SC1", "ET", "E1", "MPE",
   "PEE", "POE", "E2", "E3", "EP", "SC2", "PDS", "E4", "AGO", "RT",
   NULL
};
static int* usedNames = (int*)memAlloc(sizeof(int)*stringListLength((char**)names));

static void invariantAllCourses()
{
   assert (stringListLength((char**)names) >= MAX_COURSE_UNITS);
}

void initAllCourses(int n, int line, int column)
{
   assert (n >= 1 && n <= MAX_COURSE_UNITS);
   assert (line >= 0);
   assert (column >= 0);

   numCourses = n;
   courses = (struct _CourseUnit_**)shmAlloc(sizeof(struct _CourseUnit_*)*(n+1));
   for(int c = 0; c < n; c++)
   {
      struct _Book_** bl = randomBookListFromLibrary(NULL, randomInt(1, global->MAX_BOOKS_PER_COURSE));
      courses[c] = newCourseUnit(NULL, randomString((char**)names, usedNames, stringListLength((char**)names)), bl);
   }
   courses[n] = NULL;
   logId = registerLogger((char*)descText, line ,column , 3, lengthAllCourses(), NULL);
   sendLog(logId, toStringAllCourses());

   invariantAllCourses();
}

int logIdAllCourses()
{
   return logId;
}

struct _CourseUnit_** listAllCourses()
{
   return courses;
}

static char* toStringAllCourses()
{
   char* res = (char*)"";

   res = concat_string_in_stack(res, descText);
   res = concat_string_in_stack(res, "\n");

   for(int i = 0; courses[i] != NULL; i++)
   {
      res = concat_string_in_stack(res, BOX_TOP_LEFT);
      int l = strlen(nameCourseUnit(courses[i]));
      res = concat_string_in_stack(res, utf8HorizontalLine(l));
      struct _Book_** list = bookListCourseUnit(courses[i]);
      for(int b = 0; list[b] != NULL; b++)
      {
         int l = strlen(nameBook(list[b]))+1;
         res = concat_string_in_stack(res, utf8HorizontalLine(l));
      }
      res = concat_string_in_stack(res, BOX_TOP_RIGHT);
   }
   res = concat_string_in_stack(res, "\n");
   for(int i = 0; courses[i] != NULL; i++)
   {
      res = concat_string_in_stack(res, BOX_VERTICAL);
      res = concat_string_in_stack(res, nameCourseUnit(courses[i]));
      res = concat_string_in_stack(res, ":");
      struct _Book_** list = bookListCourseUnit(courses[i]);
      for(int b = 0; list[b] != NULL; b++)
      {
         if (b > 0)
           res = concat_string_in_stack(res, ",");
         res = concat_string_in_stack(res, nameBook(list[b]));
      }
      res = concat_string_in_stack(res, BOX_VERTICAL);
   }
   res = concat_string_in_stack(res, "\n");
   for(int i = 0; courses[i] != NULL; i++)
   {
      res = concat_string_in_stack(res, BOX_BOTTOM_LEFT);
      int l = strlen(nameCourseUnit(courses[i]));
      res = concat_string_in_stack(res, utf8HorizontalLine(l));
      struct _Book_** list = bookListCourseUnit(courses[i]);
      for(int b = 0; list[b] != NULL; b++)
      {
         int l = strlen(nameBook(list[b]))+1;
         res = concat_string_in_stack(res, utf8HorizontalLine(l));
      }
      res = concat_string_in_stack(res, BOX_BOTTOM_RIGHT);
   }
   res = concat_string_in_stack(res, "\n");

   return stringClone(res);
}

int lengthAllCourses()
{
   return MAX_NAME_SIZE+5+MAX_COURSE_UNITS*8+global->MAX_BOOKS_PER_COURSE*3;
}

struct _CourseUnit_** randomCourseList()
{
   int n = randomInt(1, numCourses-1);
   struct _CourseUnit_** res = (struct _CourseUnit_**)memAlloc(sizeof(struct _CourseUnit_*)*(n+1));
   for(int i = 0; i < n; i++)
   {
      int exists;
      do
      {
         exists = 0;
         res[i] = courses[randomInt(0, numCourses-1)];
         for(int j = 0; !exists && j < i; j++)
            exists = (res[j] == res[i]);
      }
      while(exists);
   }
   res[n] = NULL;
   return res;
}
