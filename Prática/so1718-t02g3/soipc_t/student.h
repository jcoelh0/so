/**
 *  \brief student module.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef STUDENT_H
#define STUDENT_H

#include "global.h"
#include "course-unit.h"

struct _Student_; // private data structure

// export to simulation:
struct _Student_* newStudent(struct _Student_* student, char* name, struct _CourseUnit_** courses, int line, int column);
void* mainStudent(void* args);
int logIdStudent(struct _Student_* student);

#endif
