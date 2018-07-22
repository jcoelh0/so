/**
 *  \brief Global parameters and common functions.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include "sim-alloc.h"

typedef struct _Parameters_
{
   // library:
   int NUM_BOOKS;
   int MIN_BOOK_COPIES;
   int MAX_BOOK_COPIES;
   int NUM_TABLES;          
   int NUM_SEATS_PER_TABLE;
   // courses:
   int NUM_COURSE_UNITS;
   int MAX_BOOKS_PER_COURSE;          
   int MIN_BOOK_STUDY_TIME_UNITS;
   int MAX_BOOK_STUDY_TIME_UNITS;
   // students:
   int NUM_STUDENTS;
   int MIN_STUDY_TIME_UNITS;
   int MAX_STUDY_TIME_UNITS;
   // librarian:
   int MIN_REQUESTS_PER_PERIOD;
   int MAX_REQUESTS_PER_PERIOD;
   int MIN_HANDLE_REQUEST_TIME_UNITS;
   int MAX_HANDLE_REQUEST_TIME_UNITS;
   // student/librarian:
   int MIN_EATING_TIME_UNITS;
   int MAX_EATING_TIME_UNITS;
   int MIN_SLEEPING_TIME_UNITS;
   int MAX_SLEEPING_TIME_UNITS;
   int MIN_FUN_TIME_UNITS;
   int MAX_FUN_TIME_UNITS;
} Parameters;

extern Parameters* global; // global variable with simulation parameters

// internal limits:
#define MAX_NAME_SIZE 8
#define MAX_BOOKS 26
#define MAX_STUDENTS 20
#define MAX_COURSE_UNITS 10
#define MAX_SEATS 20

#define BOX_BOTTOM_LEFT "\xE2\x94\x94"                   // '\u2514'
#define BOX_BOTTOM_RIGHT "\xE2\x94\x98"                  // '\u2518'
#define BOX_HORIZONTAL "\xE2\x94\x80"                    // '\u2500'
#define BOX_HORIZONTAL_DOWN "\xE2\x94\xAC"               // '\u252c'
#define BOX_HORIZONTAL_UP "\xE2\x94\xB4"                 // '\u2534'
#define BOX_TOP_LEFT "\xE2\x94\x8C"                      // '\u250c'
#define BOX_TOP_RIGHT "\xE2\x94\x90"                     // '\u2510'
#define BOX_VERTICAL "\xE2\x94\x82"                      // '\u2502'
#ifdef ASCII
#define BLACK_RECTANGLE "--"
#define BOOK_CLOSED "BC"
#define BOOK_OPEN "BO"
#define BOOKS "BK"
#define COMPUTER_OLD_PERSONAL "F "
#define DANCER "F"
#define FACE_NEUTRAL "N "
#define FACE_PENSIVE "SS"
#define FACE_SLEEPING "S "
#define FACE_SMILING_WITH_SUNGLASSES "D "
#define FACE_THINKING "T "
#define FOOD_BREAD "E "
#define FOOD_PIZZA "E "
#define FOOD_POT "E "
#define FOOD_POULTRY_LEG "E "
#define FOOD_STEAMING_BOWL "E "
#define SEAT "S "
#else
#define BLACK_RECTANGLE "\xE2\x96\xAC "                  // '\u25ac'
#define BOOK_CLOSED "\xF0\x9F\x93\x95 "                  // 0x1f4d5
#define BOOK_OPEN "\xF0\x9F\x93\x96 "                    // 0x1f4d6
#define BOOKS "\xF0\x9F\x93\x9A "                        // 0x1f4da
#define COMPUTER_OLD_PERSONAL "\xF0\x9F\x96\xB3 "        // 0x1f5b3
#define DANCER "\xF0\x9F\x92\x83"                        // 0x1f483
#define FACE_NEUTRAL "\xF0\x9F\x98\x90 "                 // 0x1f610
#define FACE_PENSIVE "\xF0\x9F\x98\x94 "                 // 0x1f614
#define FACE_SLEEPING "\xF0\x9F\x98\xB4 "                // 0x1f634
#define FACE_SMILING_WITH_SUNGLASSES "\xF0\x9F\x98\x8E " // 0x1f60e
#define FACE_THINKING "\xF0\x9F\xA4\x94 "                // 0x1f914
#define FOOD_BREAD "\xF0\x9F\x8D\x9E "                   // 0x1f35e
#define FOOD_PIZZA "\xF0\x9F\x8D\x95 "                   // 0x1f355
#define FOOD_POT "\xF0\x9F\x8D\xB2 "                     // 0x1f372
#define FOOD_POULTRY_LEG "\xF0\x9F\x8D\x97 "             // 0x1f357
#define FOOD_STEAMING_BOWL "\xF0\x9F\x8D\x9C "           // 0x1f35c
#define SEAT "\xF0\x9F\x92\xBA "                         // 0x1f4ba
#endif

#define utf8HorizontalLine(n) \
   ({ \
      char* res = (char*)alloca(sizeof(BOX_HORIZONTAL)*(n)+1); \
      res[0] = '\0'; \
      for(int i = 0; i < n; i++) \
         strcat(res, BOX_HORIZONTAL); \
      res; \
   })

#define utf8HorizontalSpace(n) \
   ({ \
      char* res = (char*)alloca(n+1); \
      res[0] = '\0'; \
      for(int i = 0; i < n; i++) \
         strcat(res, " "); \
      res; \
   })

#endif
