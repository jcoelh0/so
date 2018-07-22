/**
 *  \brief All courses module.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef ALL_COURSES_H
#define ALL_COURSES_H

#include "course-unit.h"

// export to simulation:
void initAllCourses(int n, int line, int column);
int logIdAllCourses();
struct _CourseUnit_** randomCourseList();
struct _CourseUnit_** listAllCourses();
int lengthAllCourses();

#endif
