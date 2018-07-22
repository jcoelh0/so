#include <stdlib.h>
#include <assert.h>
#include "sim-alloc.h"
#include "utils.h"
#include "global.h"
#include "timer.h"
#include "logger.h"
#include "book.h"
#include "library.h"
#include "librarian.h"
#include "student.h"
#include "process.h"

enum State
{
   NONE = 0,
   NORMAL,
   BREAKFAST,
   LUNCH,
   DINNER,
   SLEEPING,
   REQ_BOOKS,
   REQ_SEAT,
   STUDYING,
   HAVING_FUN,
   DONE
};

static const char* stateText[11] =
{
   "  ",
   FACE_NEUTRAL,
   FOOD_BREAD,
   FOOD_PIZZA,
   FOOD_POT,
   FACE_SLEEPING,
   BOOKS,
   SEAT,
   FACE_THINKING,
   DANCER" ",
   FACE_SMILING_WITH_SUNGLASSES
};

typedef struct _Student_
{
   int alloc;
   char* name;
   State state;
   struct _CourseUnit_** courses;
   int numCourses;
   int* concludedCourses;
   int actualCourse;
   int completionPercentage;
   struct _Book_** bookList;
   struct _Book_** studyBookList;
   int* studyTime;
   int logId;
} Student;

//structure for shared memory to use for messages to logger
typedef struct _ShmLogMessage_
{
   int logId;
   char text[1000];
} ShmLogMessage;

typedef struct _ShmLibMessage_
{
   int enroll;
} ShmLibMessage;

static void life(Student* student);
static void enrollUniversity(Student* student);
static void unEnrollUniversity(Student* student);
static void enrollCourse(Student* student);
static void sleep(Student* student);
static void eat(Student* student, int meal); // 0: breakfast; 1: lunch; 2: dinner
static void study(Student* student);
static void fun(Student* student);
static void done(Student* student);
static int courseConcluded(Student* student);
static char* toStringStudent(struct _Student_* student);
static int lengthStudent();
static int chooseBooksToStudy(Student* student);
static int bookSearch(Student* student, struct _Book_* book);

//Funcions to send messages
static void sendLogMessage(int logId, char* text);

//Variables for shared memory
key_t key1 = 11; //ShmLogMessage
char* shmAddr1;
int shmid1;

key_t key2 = 12;
int shmid2;
ShmLibMessage * shmInt = NULL;

//Semaphores for message System - logger
sem_t* log_Disp1;
sem_t* log_Copied1;

sem_t* lib_Disp;
sem_t* lib_Copied;

struct _Student_* newStudent(struct _Student_* student, char* name, struct _CourseUnit_** courses, int line, int column)
{
   assert (name != NULL && strlen(name) > 0 && strlen(name) <= MAX_NAME_SIZE);
   assert (courses != NULL);
   assert (line >= 0);
   assert (column >= 0);

   if (student == NULL)
   {
      student = (Student*)memAlloc(sizeof(Student));
      student->alloc = 1;
   }
   else
      student->alloc = 0;

   student->name = name;
   student->state = NONE;
   student->courses = courses;
   for(student->numCourses = 0; courses[student->numCourses] != NULL; student->numCourses++)
      ;
   student->concludedCourses = (int*)memAlloc(sizeof(int)*student->numCourses);
   student->actualCourse = -1;
   student->completionPercentage = 0;
   student->bookList = NULL;
   student->studyBookList = NULL;
   student->studyTime = NULL;
   static const char* translations[] = {
      FACE_NEUTRAL, " NORMAL",
      FOOD_BREAD, " BREAKFAST",
      FOOD_PIZZA, " LUNCH",
      FOOD_POT, " DINNER",
      FACE_SLEEPING, " SLEEPING",
      BOOKS, " REQ_BOOKS",
      SEAT, " REQ_SEAT",
      FACE_THINKING, " STUDYING",
      DANCER" ", " HAVING_FUN",
      FACE_SMILING_WITH_SUNGLASSES, " DONE",
      NULL
   };
   student->logId = registerLogger((char*)"Student:", line ,column , 3, lengthStudent(), (char**)translations);
   sendLog(student->logId, toStringStudent(student));

   return student;
}

struct _Student_* destroyStudent(struct _Student_* student)
{
   assert (student != NULL);

   free(student->concludedCourses);
   if (student->alloc)
   {
      free(student);
      student = NULL;
   }

   return student;
}

void* mainStudent(void* args)
{
   Student* student = (Student*)args;

   //Attach shared memory
   shmid1 = pshmget(key1, sizeof(ShmLogMessage), IPC_CREAT | 0666);
   shmAddr1 =(char*) pshmat(shmid1,0,0);
   shmid2 = pshmget(key2, sizeof(ShmLogMessage), IPC_CREAT | 0666);
   shmInt = (ShmLibMessage*) pshmat(shmid2,NULL,0);

   //Inicialize semaphores
   log_Disp1 = psem_open("/logDisp", O_CREAT, 0644);
   log_Copied1 = psem_open("/logCopied", O_CREAT, 0644);

   lib_Disp = psem_open("/libDisp", O_CREAT, 0644);
   lib_Copied = psem_open("/libCopied", O_CREAT, 0644);

   life(student);
   return NULL;
}

static void sendLogMessage(int logId, char* text){
  psem_wait(log_Copied1);
  ShmLogMessage* temp;
  temp = (ShmLogMessage*) malloc(sizeof(ShmLogMessage));
  temp->logId = logId;
  strcpy(temp->text,text);
  memcpy(shmAddr1,&temp,sizeof(ShmLogMessage));
  free(temp);
  psem_post(log_Disp1);
}


int logIdStudent(struct _Student_* student)
{
   return student->logId;
}

static void life(Student* student)
{
   enrollUniversity(student);
   for(int c = 0; c < student->numCourses; c++)
   {
      enrollCourse(student);
      do
      {
         sleep(student);
         eat(student, 0);
         study(student);
         eat(student, 1);
         study(student);
         eat(student, 2);
         fun(student);
      }
      while(!courseConcluded(student));
   }
   unEnrollUniversity(student);
   done(student);
}

static void enrollUniversity(Student* student)
{
   /* TODO: student should notify librarian */
   //reqEnrollStudent();
  psem_wait(lib_Copied);
  shmInt->enroll = 1;
  psem_post(lib_Disp);
}

static void unEnrollUniversity(Student* student)
{
   /* TODO: student should notify librarian */
  // reqDisenrollStudent();
  psem_wait(lib_Copied);
  shmInt->enroll = 0;
  psem_post(lib_Disp);
}

static void enrollCourse(Student* student)
{
   /** TODO:
    * 1: set the student state;
    * 2: choose a random non-concluded course;
    * 3: get booklist from course;
    * 4: initialize student relevant state
    **/
    student->state = NORMAL;
    student->actualCourse = randomInt(0,student->numCourses);
    student->bookList = bookListCourseUnit(student->courses[student->actualCourse]);
    sendLogMessage(student->logId,toStringStudent(student));
}

static void sleep(Student* student)
{
   /** TODO:
    * 1: sleep (state: SLEEPING). Don't forget to spend time randomly in
    *    interval [global->MIN_SLEEPING_TIME_UNITS, global->MAX_SLEEPING_TIME_UNITS]
    **/
    student->state = SLEEPING;
    sendLogMessage(student->logId,toStringStudent(student));
    spend(randomInt(global->MIN_SLEEPING_TIME_UNITS,global->MAX_SLEEPING_TIME_UNITS));
}

static void eat(Student* student, int meal) // 0: breakfast; 1: lunch; 2: dinner
{
   assert (meal >= 0 && meal <= 2);

   /** TODO:
    * 1: eat (state: BREAKFAST or LUNCH or DINNER). Don't forget to spend time randomly in
    *    interval [global->MIN_EATING_TIME_UNITS, global->MAX_EATING_TIME_UNITS]
    **/
    if(meal == 0)
      student->state = BREAKFAST;
    else if (meal == 1)
      student->state = LUNCH;
    else
      student->state = DINNER;

    sendLogMessage(student->logId,toStringStudent(student));

    spend(randomInt(global->MIN_SLEEPING_TIME_UNITS,global->MAX_SLEEPING_TIME_UNITS));
}

static void study(Student* student)
{
   assert (student->completionPercentage == 100 || student->studyTime != NULL);

   if (student->completionPercentage < 100)
   {
      int n = chooseBooksToStudy(student); // (no need to understand the algorithm)

      /** TODO:
       * 1: request librarian to requisite chosen books (state: REQ_BOOKS), wait until available
       * 2: request a free seat in library (state: REQ_SEAT)
       * 3: sit
       * 4: study (state: STUDYING). Don't forget to spend time randomly in
       *    interval [global->MIN_STUDY_TIME_UNITS, global->MAX_STUDY_TIME_UNITS]
       * 5: use time spent to update completion of course (studyTime field).
       *    Distribute time *equally* on all books studied (regardless of being completed)
       * 6: update field completionPercentage (by calling completionPercentageCourseUnit)
       * 7: check if completed and act accordingly
       * 8: rise from the seat (study session finished)
       **/
      student->state = REQ_BOOKS;
      sendLogMessage(student->logId,toStringStudent(student));
      while(booksAvailableInLibrary(student->studyBookList) == 0);
      requisiteBooksFromLibrary(student->studyBookList);

      student->state = REQ_SEAT;
      sendLogMessage(student->logId,toStringStudent(student));
      while(seatAvailable() == 0);
      int pos = sit(student->studyBookList);

      student->state = STUDYING;
      sendLogMessage(student->logId,toStringStudent(student));
      int timeSpentStudying = randomInt(global->MIN_STUDY_TIME_UNITS,global->MAX_STUDY_TIME_UNITS);
      spend(timeSpentStudying);

      for(int i = 0; i < n; i++){
        student->studyTime[i] += timeSpentStudying/n;
      }
      int res = completionPercentageCourseUnit(student->courses[student->actualCourse], student->studyBookList, student->studyTime);

      if (res >= 100){
        student->concludedCourses[student->actualCourse] = 1;
        _CourseUnit_* course;
			  int r;
			  do{
  				r = randomInt(0, student->numCourses);
  				course = student->courses[r];
        }while(course == NULL);

        student->actualCourse = r;
        student->bookList = bookListCourseUnit(student->courses[student->actualCourse]);
      }

      rise(pos);
      // leave books in table
      student->studyBookList = NULL;
   }
}

static void fun(Student* student)
{
   /** TODO:
    * 1: have fun (state: HAVING_FUN). Don't forget to spend time randomly in
    *    interval [global->MIN_FUN_TIME_UNITS, global->MAX_FUN_TIME_UNITS]
    **/
    student->state = HAVING_FUN;

    sendLogMessage(student->logId,toStringStudent(student));

    spend(randomInt(global->MIN_FUN_TIME_UNITS, global->MAX_FUN_TIME_UNITS));
}

static void done(Student* student)
{
   /** TODO:
    * 1:  life in university is over (state: DONE).
    **/
    student->state = DONE;
    sendLogMessage(student->logId,toStringStudent(student));
}

static int courseConcluded(Student* student)
{
   return student->completionPercentage == 100;
}

static char* toStringStudent(struct _Student_* student)
{
   char* res = (char*)"";
   res = concat_string_in_stack(res, BOX_TOP_LEFT);
   res = concat_string_in_stack(res, utf8HorizontalLine(MAX_NAME_SIZE+3));
   res = concat_string_in_stack(res, BOX_TOP_RIGHT);
   res = concat_string_in_stack(res, BOX_TOP_LEFT);
   for(int i = 0; i < student->numCourses; i++)
   {
      int l = strlen(nameCourseUnit(student->courses[i]))+5;
      res = concat_string_in_stack(res, utf8HorizontalLine(l));
      if (i < student->numCourses-1)
         res = concat_string_in_stack(res, BOX_HORIZONTAL_DOWN);
   }
   res = concat_string_in_stack(res, BOX_TOP_RIGHT);
   if (student->studyBookList != NULL && *student->studyBookList != NULL)
   {
      res = concat_string_in_stack(res, BOX_TOP_LEFT);
      for(int b = 0; student->studyBookList[b] != NULL; b++)
      {
         if (b > 0)
            res = concat_string_in_stack(res, BOX_HORIZONTAL_DOWN);
         int l = strlen(nameBook(student->studyBookList[b]));
         res = concat_string_in_stack(res, utf8HorizontalLine(l));
      }
      res = concat_string_in_stack(res, BOX_TOP_RIGHT);
   }
   res = concat_string_in_stack(res, utf8HorizontalSpace(global->MAX_BOOKS_PER_COURSE*3+1));
   res = concat_string_in_stack(res, "\n");
   res = concat_string_in_stack(res, BOX_VERTICAL);
   int l = MAX_NAME_SIZE - strlen(student->name);
   res = concat_string_in_stack(res, utf8HorizontalSpace(l));
   res = concat_string_in_stack(res, student->name);
   res = concat_string_in_stack(res, ":");
   res = concat_string_in_stack(res, stateText[student->state]);
   res = concat_string_in_stack(res, BOX_VERTICAL);
   res = concat_string_in_stack(res, BOX_VERTICAL);
   for(int i = 0; i < student->numCourses; i++)
   {
      res = concat_string_in_stack(res, nameCourseUnit(student->courses[i]));
      res = concat_string_in_stack(res, ":");
      if (student->concludedCourses[i])
         res = concat_string_in_stack(res, "100%");
      else if (i == student->actualCourse)
         res = concat_string_in_stack(res, percentageToString(NULL, student->completionPercentage));
      else
         res = concat_string_in_stack(res, " -- ");
      if (i < student->numCourses-1)
         res = concat_string_in_stack(res, BOX_VERTICAL);
   }
   res = concat_string_in_stack(res, BOX_VERTICAL);
   if (student->studyBookList != NULL && *student->studyBookList != NULL)
   {
      res = concat_string_in_stack(res, BOX_VERTICAL);
      for(int b = 0; student->studyBookList[b] != NULL; b++)
      {
         if (b > 0)
            res = concat_string_in_stack(res, " ");
         res = concat_string_in_stack(res, nameBook(student->studyBookList[b]));
      }
      res = concat_string_in_stack(res, BOX_VERTICAL);
   }
   res = concat_string_in_stack(res, utf8HorizontalSpace(global->MAX_BOOKS_PER_COURSE*3+1));
   res = concat_string_in_stack(res, "\n");
   res = concat_string_in_stack(res, BOX_BOTTOM_LEFT);
   res = concat_string_in_stack(res, utf8HorizontalLine(MAX_NAME_SIZE+3));
   res = concat_string_in_stack(res, BOX_BOTTOM_RIGHT);
   res = concat_string_in_stack(res, BOX_BOTTOM_LEFT);
   for(int i = 0; i < student->numCourses; i++)
   {
      int l = strlen(nameCourseUnit(student->courses[i]))+5;
      res = concat_string_in_stack(res, utf8HorizontalLine(l));
      if (i < student->numCourses-1)
         res = concat_string_in_stack(res, BOX_HORIZONTAL_UP);
   }
   res = concat_string_in_stack(res, BOX_BOTTOM_RIGHT);
   if (student->studyBookList != NULL && *student->studyBookList != NULL)
   {
      res = concat_string_in_stack(res, BOX_BOTTOM_LEFT);
      for(int b = 0; student->studyBookList[b] != NULL; b++)
      {
         if (b > 0)
            res = concat_string_in_stack(res, BOX_HORIZONTAL_UP);
         int l = strlen(nameBook(student->studyBookList[b]));
         res = concat_string_in_stack(res, utf8HorizontalLine(l));
      }
      res = concat_string_in_stack(res, BOX_BOTTOM_RIGHT);
   }
   res = concat_string_in_stack(res, utf8HorizontalSpace(global->MAX_BOOKS_PER_COURSE*3+1));

   return stringClone(res);
}

static int lengthStudent()
{
   return MAX_NAME_SIZE+5+MAX_COURSE_UNITS*8+global->MAX_BOOKS_PER_COURSE*3;
}

static int chooseBooksToStudy(Student* student)
{
  int numBooks = bookListLength(student->bookList);

  int n = randomInt(1, numBooks);
  int b = 0;
  int idxList[numBooks];
  int idxListSize = 0;
  while(b < numBooks)
  {
     while (b < numBooks && student->studyTime[b] >= minStudyBookTimeUnitsCourseUnit(student->courses[student->actualCourse], student->bookList[b]))
        b++;
     if (b < numBooks)
     {
        idxList[idxListSize++] = b;
        b++;
     }
  }
  assert (idxListSize > 0);
  if (n > idxListSize)
     n = idxListSize;

  student->studyBookList = (struct _Book_**)memAlloc(sizeof(struct _Book_*)*(n+1));
  for(int i = 0; i < n; i++)
  {
     int exists;
     do
     {
        int r = randomInt(0, idxListSize-1);
        student->studyBookList[i] = student->bookList[idxList[r]];
        exists = 0;
        for(int j = 0; !exists && j < i; j++)
        {
           exists = (student->studyBookList[i] == student->studyBookList[j]);
        }
     }
     while(exists);
  }
  student->studyBookList[n] = NULL;

  return n;
}

static int bookSearch(Student* student, struct _Book_* book)
{
   int res;
   for(res = 0; student->bookList[res] != NULL && book != student->bookList[res]; res++)
      ;

   return res;
}
