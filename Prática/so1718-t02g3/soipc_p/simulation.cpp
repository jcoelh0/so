/**
 *  \brief University library simulation
 *
 * SO's second assignment.
 *
 * \author Miguel Oliveira e Silva - 2017/2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "sim-alloc.h"
#include "global.h"
#include "timer.h"
#include "logger.h"
#include "library.h"
#include "librarian.h"
#include "student.h"
#include "all-courses.h"
#include "process.h"

static struct _Student_** students = NULL;

/* internal functions */
static void help(char* prog, Parameters *params);
static void processArgs(Parameters *params, int argc, char* argv[]);
static void showParams(Parameters *params);
static void go();
static void finish();
static void initSimulation();

static const char* names[] = {
  "Ana", "Paulo", "Luis", "Miguel", "Jose", "Joao", "Antonio", "Abel",
  "Abilio", "Ada", "Manuel", "Manuela", "Afonso", "Adolfo", "Alice", "David",
  "Lara", "Jaime", "Isabel", "Jorge", "Julio", "Julia", "Joana", "Matias",
  "Lidia", "Rui", NULL};
static int* namesUsed = NULL;

int main(int argc, char* argv[])
{
   // default parameter values:
   Parameters params = {
      // library:
      10, 1, 4, 3, 4,
      // courses:
      4, 4, 10, 100,
      // students:
      5, 20, 60,
      // librarian:
      3, 10, 15, 30,
      // student/librarian:
      10, 40, 20, 60, 15, 50
   };

   global = (Parameters*)shmAlloc(sizeof(Parameters));
   *global = params;
   processArgs(global, argc, argv);
   showParams(global);
   printf("<press RETURN>");
   getchar();

   initSimulation();
   go();
   finish();

   return 0;
}

/**
 * launch threads/processes for logger, students and librarian
 */
static void go()
{
   /* TODO: change this function to your needs */

  assert (students != NULL);

  pid_t p;

  //Fork for Logger
  p = pfork();
  if(p == 0){
    mainLogger(NULL);
    exit(0);
  }
  //Fork for Librarian
  p = pfork();
  if(p == 0){
    mainLibrarian(NULL);
    exit(0);
  }
  //Forks for Students
  for(int i=0; i<global->NUM_STUDENTS; i++){
    p = pfork();
    if(p == 0){
      mainStudent(students[i]);
      exit(0);
    }
  }


/* remove the following code: */
//void dummyLogger();
//dummyLogger();
/* end of removal */
}

/**
 * Wait for the death of all philosophers, and request and wait for waiter dead.
 */
static void finish()
{
   /* TODO: change this function to your needs */

}

static void initSimulation()
{
   /* TODO: change this function to your needs */

   srand(time(NULL));
   setTimeUnit(10);
   static const char* filterOutList[] = {
      "\n",
      BOX_TOP_LEFT, BOX_TOP_RIGHT,
      BOX_BOTTOM_LEFT, BOX_BOTTOM_RIGHT,
      BOX_HORIZONTAL_DOWN, BOX_HORIZONTAL_UP,
      BOX_HORIZONTAL, BOX_VERTICAL,
      NULL
   };
   addToFilterOut((char**)filterOutList);

   initLogger();

   int line = 1;
   initLibrary();
   line += numLinesLibrary();

   initLibrarian(line, 0);

   initAllCourses(global->NUM_COURSE_UNITS, line ,lengthLibrarian()+1);

   line += getNumLinesLogger(logIdLibrarian());

   line++;
   static const char* descText = "Students:";
   int logId = registerLogger((char*)descText, line ,0 , 1, strlen(descText), NULL);
   sendLog(logId, (char*)descText);

   line++;
   students = (struct _Student_**)memAlloc(sizeof(struct _Student_*)*global->NUM_STUDENTS);
   namesUsed = (int*)memAlloc(sizeof(int)*stringListLength((char**)names));
   for(int i = 0; i < global->NUM_STUDENTS; i++)
   {
      students[i] = newStudent(NULL, randomString((char**)names, namesUsed, stringListLength((char**)names)), randomCourseList(), line, 0);
      line += getNumLinesLogger(logIdStudent(students[i]));
   }
}

/*********************************************************************/
// No need to change remaining code!

static void help(char* prog, Parameters *params)
{
   assert(prog != NULL);
   assert(params != NULL);

   printf("\n");
   printf("Usage: %s [OPTION] ...\n", prog);
   printf("\n");
   printf("Options:\n");
   printf("\n");
   printf("  -h,--help                                   show this help\n");
   printf("  -l,--line-mode\n");
   printf("  -w,--window-mode (default)\n");
   printf("  -b,--num-books <N>\n");
   printf("     number of different books in library (default is %d)\n", params->NUM_BOOKS);
   printf("  -C,--book-copies <MIN>,<MAX>\n");
   printf("     minimum and maximum number book copies (default is [%d,%d])\n",params->MIN_BOOK_COPIES, params->MAX_BOOK_COPIES);
   printf("  -S,--num-seats <NTABLES>,<SEATS_PER_TABLE>\n");
   printf("     number of tables and seats per table (must be an even number) (default is [%d,%d])\n", params->NUM_TABLES,params->NUM_SEATS_PER_TABLE);
   printf("  -c,--courses <N>\n");
   printf("     number of course units (default is %d)\n", params->NUM_COURSE_UNITS);
   printf("  -m,--max-books-per-course <N>\n");
   printf("     maximum number of books per course (default is %d)\n", params->MAX_BOOKS_PER_COURSE);
   printf("  -1,--book-study-time-units <MIN>,<MAX>\n");
   printf("     min./max. required study time units for book within the course (default is [%d,%d])\n", params->MIN_BOOK_STUDY_TIME_UNITS, params->MAX_BOOK_STUDY_TIME_UNITS);
   printf("  -s,--num-students <N>\n");
   printf("     number of students (default is %d)\n", params->NUM_STUDENTS);
   printf("  -2,--study-time-units <MIN>,<MAX>\n");
   printf("     min./max. study time units in each library visit (default is [%d,%d])\n", params->MIN_STUDY_TIME_UNITS, params->MAX_STUDY_TIME_UNITS);
   printf("  -r,--requests-per-period <MIN>,<MAX>\n");
   printf("     min./max. attend requests in each period (default is [%d,%d])\n", params->MIN_REQUESTS_PER_PERIOD, params->MAX_REQUESTS_PER_PERIOD);
   printf("  -3,--handle-request-time-units <MIN>,<MAX>\n");
   printf("     min./max. time units to handle a request (default is [%d,%d])\n", params->MIN_HANDLE_REQUEST_TIME_UNITS, params->MAX_HANDLE_REQUEST_TIME_UNITS);
   printf("  -4,--eat-time-units <MIN>,<MAX>\n");
   printf("     min./max. eat time units (default is [%d,%d])\n", params->MIN_EATING_TIME_UNITS, params->MAX_EATING_TIME_UNITS);
   printf("  -5,--sleep-time-units <MIN>,<MAX>\n");
   printf("     min./max. sleep time units (default is [%d,%d])\n", params->MIN_SLEEPING_TIME_UNITS, params->MAX_SLEEPING_TIME_UNITS);
   printf("  -6,--fun-study-time-units <MIN>,<MAX>\n");
   printf("     min./max. fun time units (default is [%d,%d])\n", params->MIN_FUN_TIME_UNITS, params->MAX_FUN_TIME_UNITS);
   printf("\n");
}

static void processArgs(Parameters *params, int argc, char* argv[])
{
   assert(params != NULL);
   assert(argc >= 0 && argv != NULL && argv[0] != NULL);

   static struct option long_options[] =
   {
      {"help",                         no_argument,       NULL, 'h'},
      {"--line-mode",                  no_argument,       NULL, 'l'},
      {"--window-mode",                no_argument,       NULL, 'w'},
      {"--num-books",                  required_argument, NULL, 'b'},
      {"--book-copies",                required_argument, NULL, 'C'},
      {"--num-seats",                  required_argument, NULL, 'S'},
      {"--courses",                    required_argument, NULL, 'c'},
      {"--max-books-per-course",       required_argument, NULL, 'm'},
      {"--book-study-time-units",      required_argument, NULL, '1'},
      {"--num-students",               required_argument, NULL, 's'},
      {"--study-time-units",           required_argument, NULL, '2'},
      {"--requests-per-period",        required_argument, NULL, 'r'},
      {"--handle-request-time-units",  required_argument, NULL, '3'},
      {"--eat-time-units",             required_argument, NULL, '4'},
      {"--sleep-time-units",           required_argument, NULL, '5'},
      {"--fun-study-time-units",       required_argument, NULL, '6'},
      {0, 0, NULL, 0}
   };
   int op=0;

   while (op != -1)
   {
      int option_index = 0;

      op = getopt_long(argc, argv, "hlwb:C:S:c:m:1:s:2:r:3:4:5:6:", long_options, &option_index);
      int st,n,m,min,max;
      switch (op)
      {
         case -1:
            break;

         case 'h':
            help(argv[0], params);
            exit(EXIT_SUCCESS);

         case 'l':
            if (!lineMode())
               switchToLineMode();
            break;

         case 'w':
            if (lineMode())
               switchToWindowMode();
            break;

         case 'b':
            st = sscanf(optarg, "%d", &n);
            if (st != 1 || n < 1 || n > MAX_BOOKS)
            {
               fprintf(stderr, "ERROR: invalid number of books \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->NUM_BOOKS = n;
            break;

         case 'C':
            st = sscanf(optarg, "%d,%d", &min, &max);
            if (st != 2 || min < 1 || max < min || max > 99)
            {
               fprintf(stderr, "ERROR: invalid interval for books copies\"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->MIN_BOOK_COPIES = min;
            params->MAX_BOOK_COPIES = max;
            break;

         case 'S':
            st = sscanf(optarg, "%d,%d", &n, &m);
            if (st != 2 || n < 1 || m < 1 || m % 2 != 0 || n*m > MAX_SEATS)
            {
               fprintf(stderr, "ERROR: invalid number of seats \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->NUM_TABLES = n;
            params->NUM_SEATS_PER_TABLE = m;
            break;

         case 'c':
            st = sscanf(optarg, "%d", &n);
            if (st != 1 || n < 1 || n > MAX_COURSE_UNITS)
            {
               fprintf(stderr, "ERROR: invalid number of course units \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->NUM_COURSE_UNITS = n;
            break;

         case 'm':
            st = sscanf(optarg, "%d", &n);
            if (st != 1 || n < 1 || n > params->NUM_BOOKS)
            {
               fprintf(stderr, "ERROR: invalid maximum number of books per course \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->MAX_BOOKS_PER_COURSE = n;
            break;

         case '1':
            st = sscanf(optarg, "%d,%d", &min, &max);
            if (st != 2 || min < 1 || max < min)
            {
               fprintf(stderr, "ERROR: invalid interval for book study time units \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->MIN_BOOK_STUDY_TIME_UNITS = min;
            params->MAX_BOOK_STUDY_TIME_UNITS = max;
            break;

         case 's':
            st = sscanf(optarg, "%d", &n);
            if (st != 1 || n < 1 || n > MAX_STUDENTS)
            {
               fprintf(stderr, "ERROR: invalid number of students \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->NUM_STUDENTS = n;
            break;

         case '2':
            st = sscanf(optarg, "%d,%d", &min, &max);
            if (st != 2 || min < 1 || max < min)
            {
               fprintf(stderr, "ERROR: invalid interval for study time units \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->MIN_STUDY_TIME_UNITS = min;
            params->MAX_STUDY_TIME_UNITS = max;
            break;

         case 'r':
            st = sscanf(optarg, "%d,%d", &min, &max);
            if (st != 2 || min < 1 || max < min)
            {
               fprintf(stderr, "ERROR: invalid interval for request handling in each period \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->MIN_REQUESTS_PER_PERIOD = min;
            params->MAX_REQUESTS_PER_PERIOD = max;
            break;

         case '3':
            st = sscanf(optarg, "%d,%d", &min, &max);
            if (st != 2 || min < 1 || max < min)
            {
               fprintf(stderr, "ERROR: invalid interval for time units to handle a request \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->MIN_HANDLE_REQUEST_TIME_UNITS = min;
            params->MAX_HANDLE_REQUEST_TIME_UNITS = max;
            break;

         case '4':
            st = sscanf(optarg, "%d,%d", &min, &max);
            if (st != 2 || min < 1 || max < min)
            {
               fprintf(stderr, "ERROR: invalid interval for eat time units \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->MIN_EATING_TIME_UNITS = min;
            params->MAX_EATING_TIME_UNITS = max;
            break;

         case '5':
            st = sscanf(optarg, "%d,%d", &min, &max);
            if (st != 2 || min < 1 || max < min)
            {
               fprintf(stderr, "ERROR: invalid interval for sleep time units \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->MIN_SLEEPING_TIME_UNITS = min;
            params->MAX_SLEEPING_TIME_UNITS = max;
            break;

         case '6':
            st = sscanf(optarg, "%d,%d", &min, &max);
            if (st != 2 || min < 1 || max < min)
            {
               fprintf(stderr, "ERROR: invalid interval for fun time units \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            params->MIN_FUN_TIME_UNITS = min;
            params->MAX_FUN_TIME_UNITS = max;
            break;

         default:
            help(argv[0], params);
            exit(EXIT_FAILURE);
            break;
      }
   }

   if (optind < argc)
   {
      fprintf(stderr, "ERROR: invalid extra arguments\n");
      exit(EXIT_FAILURE);
   }
}

static void showParams(Parameters *params)
{
   assert(params != NULL);

   printf("\n");
   printf("Simulation parameters (%s):\n", lineMode() ? "line mode" : "window mode");
   printf("  --num-books: %d\n", params->NUM_BOOKS);
   printf("  --book-copies: [%d,%d]\n", params->MIN_BOOK_COPIES, params->MAX_BOOK_COPIES);
   printf("  --num-seats: [tables:%d,seats-per-table:%d]\n", params->NUM_TABLES, params->NUM_SEATS_PER_TABLE);
   printf("  --courses: %d\n", params->NUM_COURSE_UNITS);
   printf("  --max-books-per-course: %d\n", params->MAX_BOOKS_PER_COURSE);
   printf("  --book-study-time-units: [%d,%d]\n", params->MIN_BOOK_STUDY_TIME_UNITS, params->MAX_BOOK_STUDY_TIME_UNITS);
   printf("  --num-students: %d\n", params->NUM_STUDENTS);
   printf("  --study-time-units: [%d,%d]\n", params->MIN_STUDY_TIME_UNITS, params->MAX_STUDY_TIME_UNITS);
   printf("  --requests-per-period: [%d,%d]\n", params->MIN_REQUESTS_PER_PERIOD, params->MAX_REQUESTS_PER_PERIOD);
   printf("  --handle-request-time-units: [%d,%d]\n", params->MIN_HANDLE_REQUEST_TIME_UNITS, params->MAX_HANDLE_REQUEST_TIME_UNITS);
   printf("  --eat-time-units: [%d,%d]\n", params->MIN_EATING_TIME_UNITS, params->MAX_EATING_TIME_UNITS);
   printf("  --sleep-time-units: [%d,%d]\n", params->MIN_SLEEPING_TIME_UNITS, params->MAX_SLEEPING_TIME_UNITS);
   printf("  --fun-study-time-units: [%d,%d]\n", params->MIN_FUN_TIME_UNITS, params->MAX_FUN_TIME_UNITS);
   printf("\n");
}
