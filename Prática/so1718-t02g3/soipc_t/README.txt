This source code is an incomplete implementation of an university library
simulation.  It is composed of the following modules:

- thread: POSIX threads wrapper module (removes the burden for result
          verification and error handling)

- process: process and System V IPC wrapper module (removes the burden for
           result verification and error handling)

- utils: some supplementary functions, including a wrapper for malloc (memAlloc)
         that removes the burden for result verification and error handling.

- queue: a simple linked list implementation of a queue with some extra array
         like operations.

- logger: a smart log module, able to visualize information in a window mode
          or in a (classical) line mode.

- timer: module that implements a pause function (named: spend) applicable to
         multiples of a defined period (named: time unit).

- sim-alloc: other supplementary function useful for this simulation.
             In particular, it contains a shmAlloc that is used in memory
             allocation of data structures that are (to be) shared among active
             entities (threads or processes).  In the implementation given it
             is just a memAlloc wrapper.

- global: module containing the simulation parameters data structure, a global
          variable (yuk!) for them, and some macro definition (eg. utf8 symbols)

- book: book module (basically it is simply composed of a name) 

- course-unit: a course unit abstraction.  A course unit is composed of a name,
               a list of books, the minimum study time required for its
               successfully completion (randomly chosen, according the global
               parameters), and a function that does the necessary math to
               estimate the percentage of course completion.

- all-courses: module to abstract a group of courses (those used in the
               simulation). 

- library: a (to be) shared module abstracting a library.  It is composed of a
           bookshelf containing the available books, and a group of tables for
           study.
           A table contains a group of seats that can be used by students to
           study (by sitting in it) when available (empty seat and no books
           in its table place).  It can also be used by the librarian to
           get books from the bookshelf, collect books from empty seats in
           the table, and put back books in the bookshelf.

- librarian: an (to be) active entity representing a single librarian. The
             librarian is responsible for getting books from the bookshelf when
             there is a request from a student (if possible, or else it should
             delay the request until it can be fulfilled); he is also
             responsible for collecting books from empty seats in the table, and
             put them back in the library bookshelf.
             For the purposes of this simulation, he also tracks the number of
             students still active.
             Please note, that all these operations are required to be performed
             by the librarian active entity (unlike what happens in the library
             which is a passive shared module).  Students requests those
             services to the librarian.

- student: an (to be) active entity representing each student (unlike the
           librarian, usually there are more than one).
           Students (when necessary) make requests to the librarian
           (enroll/disenroll from the university, requisite a list of books,
           collect books), and use the library to study (basically they sit
           and rise from available seats in the table).
  
To better understand the librarian and student modules start by looking to the
life function.

--

To ease your work in this assignment, I have annotated the locations in the
provided code in which changes are required. You can, of course, change other
parts of the source code, but please be careful not to destroy important
functionalities and correctness verifications.  These locations can be
identified with comments containing a TODO keyword (as: "to do"). Sometimes,
besides the TODO annotation, a description of the actions required are also
included.

The source code provided contains 53 TODO annotations, in the following 6
files:

- sim-alloc.cpp
- simulation.cpp
- logger.cpp
- library.cpp
- librarian.cpp
- student.cpp

As such, not taking into account possible BUGS in the provided code [2],
a correct implementation only needs to change those files.

--

The source code contains a Makefile, which should be used for project
compilation and cleanup:

make          # compile the code
make clean    # removes object files
make clanall  # removes object file and executables

The Makefile can be changed to adapt to the project needs (ASCII output,
EXCEPTION error handling policy, include thread.o or process.o modules,
and so on).

--

Code development had followed a Design-by-Contract approach [1].
Since C/C++ has no native support for this methodology, we simply used the
C-library assert module (DbC is much more than this).

Asserts are used to verify the correctness of the code, hence it can (sometimes)
catch concurrency errors resulting, for instance, from race-conditions.

Miguel Oliveira e Silva, December 2017

[1] https://en.wikipedia.org/wiki/Design_by_contract

[2] That's programming folks!

