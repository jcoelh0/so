#include <stdlib.h>
#include <assert.h>
#include "utils.h"
#include "queue.h"
#include "timer.h"
#include "global.h"
#include "logger.h"
#include "book.h"
#include "library.h"
#include "librarian.h"
#include "process.h"
#include "thread.h"

// used in request->id
#define REQ_TERMINATION        -1
#define REQ_COLLECT_BOOKS      -2
#define REQ_ENROLL_STUDENT     -3
#define REQ_DISENROLL_STUDENT  -4

typedef struct _Request_
{
   struct _Book_** books;
   int id;
   int requisited;
} Request;

static Request* newTerminationRequest();
static Request* newRequisiteBooksRequest(struct _Book_** books, int id); // The book requisition is directly made by the student to the library.
static Request* newCollectBooksRequest();
static Request* newEnrollStudentRequest();
static Request* newDisenrollStudentRequest();

enum State
{
   NONE = 0,
   NORMAL,
   BREAKFAST,
   LUNCH,
   DINNER,
   SLEEPING,
   BOOK_REQ,
   BOOK_COLLECT,
   HAVING_FUN,
   DONE
};

static const char* stateText[10] =
{
   "  ",
   FACE_NEUTRAL,
   FOOD_BREAD,
   FOOD_STEAMING_BOWL,
   FOOD_POULTRY_LEG,
   FACE_SLEEPING,
   BOOK_CLOSED,
   BOOKS,
   COMPUTER_OLD_PERSONAL,
   FACE_SMILING_WITH_SUNGLASSES
};

/* TODO: put your code here */

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

//Variables for shared memory
key_t key4 = 11; //ShmLogMessage
char* shmAddr4;
int shmid4;

key_t key5 = 12;
int shmid5;
ShmLibMessage * shmInt1 = NULL;

//Semaphores for message System - logger
sem_t* log_Disp4;
sem_t* log_Copied4;

sem_t* lib_Disp5;
sem_t* lib_Copied5;

void sendLogMessageLib(int logId, char* text);
void* receiveMessages(void* arg);

static const char* descText = "Librarian:";

static int idCount = 0;
static int currIdSize = 0;
static int currIdMaxSize = 128;
static int* currId = (int*)memAlloc(sizeof(int)*currIdMaxSize);

static void currIdIn(int id);
static int containsId(int id);
static void removeId(int id);


static int enrolledStudents;
static State state;
static int logId;

static int alive;
static Queue* reqQueue = newQueue(NULL);
static Queue* pendingRequests = newQueue(NULL);

static int aliveLibrarian();
static void life();
static void sleep();
static void eat(int meal); // 0: breakfast; 1: lunch; 2: dinner
static void handleRequests();
static void collectBooks();
static void handleRequest(Request* req);
static void fun();
static void done();
static int newId();
static char* toStringLibrarian();


void initLibrarian(int line, int column)
{
   assert (line >= 0);
   assert (column >= 0);

   enrolledStudents = 0;
   state = NONE;
   alive = 1;
   static const char* translations[] = {
      "Librarian:", "",
      FACE_NEUTRAL, "NORMAL",
      FOOD_BREAD, "BREAKFAST",
      FOOD_STEAMING_BOWL, "LUNCH",
      FOOD_POULTRY_LEG, "DINNER",
      FACE_SLEEPING, "SLEEPING",
      BOOK_CLOSED, "BOOK_REQ",
      BOOKS, "BOOK_COLLECT",
      FACE_THINKING, "STUDYING",
      COMPUTER_OLD_PERSONAL, "HAVING_FUN",
      FACE_SMILING_WITH_SUNGLASSES, "DONE",
      NULL
   };
   logId = registerLogger((char*)descText, line ,column , 4, lengthLibrarian(), (char**)translations);
   sendLog(logId, toStringLibrarian());

   //Attach shared memory
   shmid4 = pshmget(key4, sizeof(ShmLogMessage), IPC_CREAT | 0666);
   shmAddr4 =(char*) pshmat(shmid4,0,0);
   shmid5 = pshmget(key5, sizeof(ShmLibMessage), IPC_CREAT | 0666);
   shmInt1 = (ShmLibMessage*) pshmat(shmid5,NULL,0);

   //Inicialize semaphores
   log_Disp4 = psem_open("/logDisp", O_CREAT, 0644);
   log_Copied4 = psem_open("/logCopied", O_CREAT, 0644);

   lib_Disp5 = psem_open("/libDisp", O_CREAT, 0644);
   lib_Copied5 = psem_open("/libCopied", O_CREAT, 0644);

}

void sendLogMessageLib(int logId, char* text){
  psem_wait(log_Copied4);
  ShmLogMessage* temp;
  temp = (ShmLogMessage*) malloc(sizeof(ShmLogMessage));
  temp->logId = logId;
  strcpy(temp->text,text);
  memcpy(shmAddr4,&temp,sizeof(ShmLogMessage));
  free(temp);
  psem_post(log_Disp4);
}

void destroyLibrarian()
{
}

int logIdLibrarian()
{
   return logId;
}

void* mainLibrarian(void* arg)
{
  pthread_t messageThread;
  thread_create(&messageThread, NULL, receiveMessages, NULL);
   life();
  pthread_join(messageThread, NULL);

   return NULL;
}

void* receiveMessages(void* arg){
  while(aliveLibrarian()){
    psem_wait(lib_Disp5);
    if (shmInt1 == 0){
      reqEnrollStudent();
    }
    else{
      reqDisenrollStudent();
    }
    psem_post(lib_Copied5);
  }
  return NULL;
}

static void life()
{
   while(aliveLibrarian())
   {
      sleep();
      eat(0);
      handleRequests();
      collectBooks();
      eat(1);
      handleRequests();
      collectBooks();
      eat(2);
      fun();
   }
   done();
}

static int aliveLibrarian()
{
   /** TODO:
    * 1: librarian should be alive until a request for termination and an empty reqQueue
    **/
   if(sizeQueue(reqQueue) != 0 && containsId(REQ_TERMINATION) == 1)
   {
       return 1;
   }
   else
   {
   	   alive = 0;
   	   return 0;
   }
}

static void sleep()
{
   /** TODO:
    * 1: sleep (state: SLEEPING). Don't forget to spend time randomly in
    *    interval [global->MIN_SLEEPING_TIME_UNITS, global->MAX_SLEEPING_TIME_UNITS]
    **/
    state = SLEEPING;
  sendLogMessageLib(logId, toStringLibrarian());
	spend(randomInt(global->MIN_SLEEPING_TIME_UNITS, global->MAX_SLEEPING_TIME_UNITS));
}

static void eat(int meal) // 0: breakfast; 1: lunch; 2: dinner
{
	assert (meal >= 0 && meal <= 2);

   	/** TODO:
    * 1: eat (state: BREAKFAST or LUNCH or DINNER). Don't forget to spend time randomly in
    *    interval [global->MIN_EATING_TIME_UNITS, global->MAX_EATING_TIME_UNITS]
    **/
    switch (meal)
    {
		case 0:
			state = BREAKFAST;
			break;
		case 1:
			state = LUNCH;
			break;
		case 2:
			state = DINNER;
			break;
	}
  sendLogMessageLib(logId, toStringLibrarian());
	spend(randomInt(global->MIN_EATING_TIME_UNITS, global->MAX_EATING_TIME_UNITS));
}

static void handleRequests()
{
   /** TODO:
    * 1: choose a random number (n) in interval [global->MIN_REQUESTS_PER_PERIOD, global->MAX_REQUESTS_PER_PERIOD]
    * 2. accept n requests (except if termination is requested)
    * 3. requests are placed (elsewhere) in queue reqQueue
    * 4. use function handleRequest to handle a single request.
    * 5. Don't forget to spend time randomly in interval [global->MIN_HANDLE_REQUEST_TIME_UNITS, global->MAX_HANDLE_REQUEST_TIME_UNITS]
    **/

	int n = randomInt(global->MIN_REQUESTS_PER_PERIOD, global->MAX_REQUESTS_PER_PERIOD);

	while(aliveLibrarian() && n != 0)
	{
		handleRequest((Request *)outQueue(reqQueue));
		n--;
	}
	spend(randomInt(global->MIN_HANDLE_REQUEST_TIME_UNITS, global->MAX_HANDLE_REQUEST_TIME_UNITS));
}

static void collectBooks()
{
   /** TODO:
    * 1: traverse all seats searching for books in empty seats, collect those books and put them in library bookshelf
    * 2. check of pending requests can be attended (in order)
    * 3. Don't forget to spend time randomly in interval [global->MIN_HANDLE_REQUEST_TIME_UNITS, global->MAX_HANDLE_REQUEST_TIME_UNITS]
    **/
    for(int i = 0; i < numSeats(); i++)
    {
    	if(!seatOccupied(i) && booksInSeat(i))
    	{
    		collectBooksLibrary(i);
    	}
    	//2. The book requisition is directly made by the student to the library.
    }
    spend(randomInt(global->MIN_HANDLE_REQUEST_TIME_UNITS, global->MAX_HANDLE_REQUEST_TIME_UNITS));
}

static void handleRequest(Request* req)
{
   assert (req != NULL);

   /** TODO:
    * 1: handle each request
    *
    * 2. check of pending requests can be attended (in order)
    * 3. Don't forget to spend time randomly in interval [global->MIN_HANDLE_REQUEST_TIME_UNITS, global->MAX_HANDLE_REQUEST_TIME_UNITS]
    **/

   switch(req->id)
   {
      case REQ_TERMINATION:
      	 alive = 0;
         break;
      case REQ_COLLECT_BOOKS:
      	 collectBooks();
      	 removeId(REQ_COLLECT_BOOKS);
         break;
      case REQ_ENROLL_STUDENT:
      	 enrolledStudents++;
      	 removeId(REQ_ENROLL_STUDENT);
         break;
      case REQ_DISENROLL_STUDENT:
      	 enrolledStudents--;
      	 removeId(REQ_DISENROLL_STUDENT);
         break;
      default: // book requisition
      	 /*if(booksAvailableInLibrary(books) == 0)
      	 {
   			 inQueue(pendingRequests, req);

      	 }
      	 The book request is directly made by the student to the library.
      	 */
         break;
   }
   spend(randomInt(global->MIN_HANDLE_REQUEST_TIME_UNITS, global->MAX_HANDLE_REQUEST_TIME_UNITS));
}

static void fun()
{
   /** TODO:
    * 1: have fun (state: HAVING_FUN). Don't forget to spend time randomly in
    *    interval [global->MIN_FUN_TIME_UNITS, global->MAX_FUN_TIME_UNITS]
    **/
    state = HAVING_FUN;
    sendLogMessageLib(logId, toStringLibrarian());
    spend(randomInt(global->MIN_FUN_TIME_UNITS, global->MAX_FUN_TIME_UNITS));
}

static void done()
{
   /** TODO:
    * 1:  life of librarian is over (state: DONE).
    **/
    state = DONE;
    sendLogMessageLib(logId, toStringLibrarian());
}

int lengthLibrarian()
{
   return 4+12*2+11;
}

void reqEnrollStudent()
{
   /** TODO:
    * 1: queue reqQueue should be updated and a notification send to librarian active entity
    * 2: does not wait for response.
    **/
    inQueue(reqQueue, newEnrollStudentRequest());
    newId();
    currIdIn(REQ_ENROLL_STUDENT);
}

void reqDisenrollStudent()
{
   /** TODO:
    * 1: queue reqQueue should be updated and a notification send to librarian active entity
    * 2: does not wait for response.
    **/
    inQueue(reqQueue, newDisenrollStudentRequest());
    newId();
    currIdIn(REQ_DISENROLL_STUDENT);
}

void reqTermination()
{
   /** TODO:
    * 1: queue reqQueue should be updated and a notification send to librarian active entity
    * 2: does not wait for response.
    **/
    inQueue(reqQueue, newTerminationRequest());
    newId();
    currIdIn(REQ_TERMINATION);
}

void reqCollectBooks()
{
   /** TODO:
    * 1: queue reqQueue should be updated and a notification send to librarian active entity
    * 2: does not wait for response.
    **/
    inQueue(reqQueue, newCollectBooksRequest());
    newId();
    currIdIn(REQ_COLLECT_BOOKS);
}

int reqBookRequisition(struct _Book_** books) //nãao se faz
{
   assert (books != NULL && *books != NULL);


   return 0;
}

static int newId()
{
   idCount++;
   if (idCount < 0)
      idCount = 1;

   assert (idCount >= 1);

   return idCount;
}

static char* toStringLibrarian()
{
   char* res = (char*)"";

   res = concat_string_in_stack(res, descText);
   res = concat_string_in_stack(res, "\n");

   res = concat_string_in_stack(res, BOX_TOP_LEFT);
   res = concat_string_in_stack(res, utf8HorizontalLine(2));
   res = concat_string_in_stack(res, BOX_HORIZONTAL_DOWN);
   res = concat_string_in_stack(res, utf8HorizontalLine(11));
   res = concat_string_in_stack(res, BOX_HORIZONTAL_DOWN);
   res = concat_string_in_stack(res, utf8HorizontalLine(11));
   res = concat_string_in_stack(res, BOX_HORIZONTAL_DOWN);
   res = concat_string_in_stack(res, utf8HorizontalLine(10));
   res = concat_string_in_stack(res, BOX_TOP_RIGHT);
   res = concat_string_in_stack(res, "\n");
   res = concat_string_in_stack(res, BOX_VERTICAL);
   res = concat_string_in_stack(res, stateText[state]);
   res = concat_string_in_stack(res, BOX_VERTICAL);
   res = concat_string_in_stack(res, "Students:");
   res = concat_string_in_stack(res, intToString(NULL, enrolledStudents, 2));
   res = concat_string_in_stack(res, BOX_VERTICAL);
   res = concat_string_in_stack(res, "Requests:");
   res = concat_string_in_stack(res, intToString(NULL, sizeQueue(reqQueue), 2));
   res = concat_string_in_stack(res, BOX_VERTICAL);
   res = concat_string_in_stack(res, "Delayed:");
   res = concat_string_in_stack(res, intToString(NULL, sizeQueue(pendingRequests), 2));
   res = concat_string_in_stack(res, BOX_VERTICAL);
   res = concat_string_in_stack(res, "\n");
   res = concat_string_in_stack(res, BOX_BOTTOM_LEFT);
   res = concat_string_in_stack(res, utf8HorizontalLine(2));
   res = concat_string_in_stack(res, BOX_HORIZONTAL_UP);
   res = concat_string_in_stack(res, utf8HorizontalLine(11));
   res = concat_string_in_stack(res, BOX_HORIZONTAL_UP);
   res = concat_string_in_stack(res, utf8HorizontalLine(11));
   res = concat_string_in_stack(res, BOX_HORIZONTAL_UP);
   res = concat_string_in_stack(res, utf8HorizontalLine(10));
   res = concat_string_in_stack(res, BOX_BOTTOM_RIGHT);
   res = concat_string_in_stack(res, "\n");

   return stringClone(res);
}

static void currIdIn(int id)
{
   if (currIdSize == currIdMaxSize)
   {
      int* tmp = (int*)memAlloc(sizeof(int)*2*currIdMaxSize);
      for(int i = 0; i < currIdMaxSize; i++)
         tmp[i] = currId[i];
      free(currId);
      currId = tmp;
      currIdMaxSize = 2*currIdMaxSize;
   }
   currId[currIdSize] = id;
   currIdSize++;
}

static int containsId(int id)
{
   int res = 0;
   for(int i = 0; !res && i < currIdSize; i++)
      res = (currId[i] == id);
   return res;
}

static void removeId(int id)
{
   int i;
   for(i = 0; i < currIdSize && currId[i] != id; i++)
      ;
   assert (currId[i] == id);
   currIdSize--;
   for(; i < currIdSize; i++)
      currId[i] = currId[i+1];

   assert (!containsId(id));
}

static Request* newTerminationRequest()
{
   Request* res = (Request*)memAlloc(sizeof(Request));
   res->books = NULL;
   res->id = REQ_TERMINATION;
   res->requisited = 0;
   return res;
}

static Request* newRequisiteBooksRequest(struct _Book_** books, int id)
{
   Request* res = (Request*)memAlloc(sizeof(Request));
   res->books = books;
   res->id = id;
   res->requisited = 0;
   return res;
}

static Request* newCollectBooksRequest()
{
   Request* res = (Request*)memAlloc(sizeof(Request));
   res->books = NULL;
   res->id = REQ_COLLECT_BOOKS;
   res->requisited = 0;
   return res;
}

static Request* newEnrollStudentRequest()
{
   Request* res = (Request*)memAlloc(sizeof(Request));
   res->books = NULL;
   res->id = REQ_ENROLL_STUDENT;
   res->requisited = 0;
   return res;
}

static Request* newDisenrollStudentRequest()
{
   Request* res = (Request*)memAlloc(sizeof(Request));
   res->books = NULL;
   res->id = REQ_DISENROLL_STUDENT;
   res->requisited = 0;
   return res;
}
