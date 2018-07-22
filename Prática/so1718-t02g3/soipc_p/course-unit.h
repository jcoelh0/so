/**
 *  \brief course unit module.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef COURSE_UNIT_H
#define COURSE_UNIT_H

#include "book.h"

struct _CourseUnit_;  // private data structure

// export to all-course:
struct _CourseUnit_* newCourseUnit(struct _CourseUnit_* courseUnit, char* name, struct _Book_** bookList);
struct _CourseUnit_* destroyCourseUnit(struct _CourseUnit_* courseUnit);

// export to students and all-course:
char* nameCourseUnit(struct _CourseUnit_* courseUnit);
struct _Book_** bookListCourseUnit(struct _CourseUnit_* courseUnit);

// export to students:
int containsBookCourseUnit(struct _CourseUnit_* courseUnit, struct _Book_* book);
int containsAllBooksCourseUnit(struct _CourseUnit_* courseUnit, struct _Book_** books);
int minStudyBookTimeUnitsCourseUnit(struct _CourseUnit_* courseUnit, struct _Book_* book);
int completionPercentageCourseUnit(struct _CourseUnit_* courseUnit, struct _Book_** books, int* timeSpent);

#endif
