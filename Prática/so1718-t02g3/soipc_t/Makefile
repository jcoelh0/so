# to generate core file execute in bash:
#    ulimit -c unlimited

.PHONY: all clean cleanall

OBJS=utils.o queue.o timer.o sim-alloc.o global.o logger.o book.o course-unit.o library.o librarian.o student.o all-courses.o simulation.o  # if necessary add new modules
OBJS_THREAD=thread.o

CPPFLAGS=-Wall -ggdb -pthread     # if necessary add/remove options
SYMBOLS=-DEXIT_POLICY             # -DEXCEPTION_POLICY or -DEXIT_POLICY; for ascii output: -DACSII

all: simulation-thread

simulation-thread: $(OBJS)  $(OBJS_THREAD)
	g++ $(SYMBOLS) $(CPPFLAGS) $(OBJS) $(OBJS_THREAD) -o simulation-thread

%.o: %.cpp %.h
	g++ $(SYMBOLS) $(CPPFLAGS) -c $<

clean:
	rm -fv $(OBJS) $(OBJS_THREAD) $(OBJS_PROCESS) core

cleanall: clean
	rm -fv simulation-thread simulation-process

