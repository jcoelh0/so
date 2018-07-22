#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "utils.h"
#include "global.h"
#include "queue.h"
#include "logger.h"
#include "sim-alloc.h"
#include "process.h"
#include "thread.h"


#define MAX_REGISTS 100
#define MAX_FILTER 100

typedef struct _Regist_
{
   char* name;
   int line, column, numLines, numColumns;
   char** lineModeTranslations;
} Regist;

typedef struct _Event_
{
   int logId;
   char* text;
} Event;

//structure for shared memory to use for messages to logger
typedef struct _ShmLogMessage_
{
   int logId;
   char text[1000];
} ShmLogMessage;

static Regist* newRegist(char* name, int line, int column, int numLines, int numColumns, char** lineModeTranslations);
static Event* newEvent(int logId, char* text);
static void destroyEvent(Event* e);

static int eventExists();
static void processEvents();
static void printEvent(Event* e);

//Function for thread
static void* receiveMessages(void* arg);

/* TODO: put your code here */

//Variables for shared memory
key_t key = 11; //ShmLogMessage
char* shmAddr;
int shmid;

//Semaphores for message System - logger
sem_t* log_Disp;
sem_t* log_Copied;


static Queue* queue = newQueue(NULL);
static Regist** areas = (Regist**)memAlloc(MAX_REGISTS*sizeof(Regist*));
static int areasLength = 0;
static int _alive_ = 1;
static int maxLine = 0;
static int _lineMode_ = 0;
static char** filterOut = (char**)memAlloc(MAX_FILTER*sizeof(char*));
static int filterOutLength = 0;

int registerLogger(char* name, int line, int column, int numLines, int numColumns, char** lineModeTranslations)
{
   /* TODO: change this function to your needs */

   assert (name != NULL);
   assert (line >= 0);
   assert (column >= 0);
   assert (numLines > 0);
   assert (numColumns > 0);
   assert (validLineModeTranslation(lineModeTranslations));

   assert (areasLength < MAX_REGISTS);
   Regist* reg = newRegist(name, line, column, numLines, numColumns, lineModeTranslations);
   int res = areasLength;
   areas[areasLength] = reg;
   areasLength++;
   if (maxLine < line + numLines)
      maxLine = line + numLines;
   return res;
}

int alive()
{
   /* TODO: change this function to your needs */

   int res;
   res = _alive_ || eventExists();
   return res;
}

void initLogger()
{
   /* TODO: change this function to your needs */

  //Attach shared memory
  shmid = pshmget(key, sizeof(ShmLogMessage), IPC_CREAT | 0666);
  shmAddr =(char*) pshmat(shmid,0,0);

  //Inicialize semaphores
  log_Disp = psem_open("/logDisp", O_CREAT, 0644);
  log_Copied = psem_open("/logCopied", O_CREAT, 0644);
}

void termLogger()
{
   /* TODO: change this function to your needs */

   _alive_ = 0;

  //Destroy Shared memory
  shmdt (shmAddr);
  shmctl (shmid, IPC_RMID, 0);

  //Terminate Semaphores
  psem_unlink("/logDisp");
  psem_unlink("/logCopied");
}

int validLogId(int logId)
{
   /* TODO: change this function to your needs */

   int res;
   res = logId >= 0 && logId < areasLength;
   return res;
}

int getLineLogger(int logId)
{
   /* TODO: change this function to your needs */

   assert (validLogId(logId));

   int res;
   res = areas[logId]->line;
   return res;
}

int getColumnLogger(int logId)
{
   /* TODO: change this function to your needs */

   assert (validLogId(logId));

   int res;
   res = areas[logId]->column;
   return res;
}

int getNumLinesLogger(int logId)
{
   /* TODO: change this function to your needs */

   assert (validLogId(logId));

   int res;
   res = areas[logId]->numLines;
   return res;
}

int getNumColumnsLogger(int logId)
{
   /* TODO: change this function to your needs */

   assert (validLogId(logId));

   int res;
   res = areas[logId]->numColumns;
   return res;
}

void sendLog(int logId, char* text)
{
   /* TODO: change this function to your needs */

   assert (validLogId(logId));
   assert (text != NULL);

   Event* e = newEvent(logId, text);
   inQueue(queue, (void*)e);

}

void* mainLogger(void* arg)
{
   if (!_lineMode_)
      clearConsole();

    //initialize thread to capture messages
  pthread_t messageThread;
  thread_create(&messageThread, NULL, receiveMessages, NULL);
   while(alive())
   {

      /** TODO:
       * 1: wait for a log event (or termination)
       * 2. processEvents (if any)
       **/
      if(eventExists()){

        processEvents();

      }
   }
   pthread_join(messageThread, NULL);
   return NULL;
}

static void* receiveMessages(void* arg){
  psem_wait(log_Disp);
  ShmLogMessage* temp;
  temp = (ShmLogMessage*) malloc(sizeof(ShmLogMessage));
  memcpy(temp,shmAddr,sizeof(ShmLogMessage));
  sendLog(temp->logId,temp->text);
  free(temp);
  psem_post(log_Copied);
  return NULL;
}

static int eventExists()
{
   return sizeQueue(queue) > 0;
}

static void processEvents()
{
   assert (eventExists());

   while(sizeQueue(queue) > 0)
   {
      Event* e = (Event*)outQueue(queue);
      printEvent(e);
      destroyEvent(e);
   }
}

// returns -1 if it is not a prefix, otherwise it returns the length of prefix
static int isPrefix(char* text, char* prefix)
{
   int res = 0;
   while(res >= 0 && prefix[res])
   {
      if (text[res] == prefix[res])
         res++;
      else
         res = -1;
   }
   return res;
}

static void printEvent(Event* e)
{
   assert (e != NULL);

   if (_lineMode_)
   {
      char *p = e->text;
      int spacePrinted = 1;
      int printed;

      char* logLine = (char*)"";
      logLine = concat_string_in_stack(logLine, areas[e->logId]->name);
      logLine = concat_string_in_stack(logLine, " ");

      while(*p != '\0')
      {
         printed = 0;
         // remove filterOut strings:
         int res = -1;
         for(int i = 0; res < 0 && i < filterOutLength; i++)
            res = isPrefix(p, filterOut[i]);
         if (res > 0)
         {
            p += res;
            if (!spacePrinted)
               logLine = concat_string_in_stack(logLine, " ");
            spacePrinted = 1;
            printed = 1;
         }

         char** translations = areas[e->logId]->lineModeTranslations;
         if (translations != NULL)
         {
            int res = 0;
            while (*translations != NULL)
            {
               res = isPrefix(p, *translations);
               if (res > 0)
               {
                  p += res;
                  logLine = concat_string_in_stack(logLine, (*(translations+1)));
                  if (strlen(*(translations+1)) > 0)
                  {
                     spacePrinted = (*(translations+1))[strlen(*(translations+1)) - 1] == ' ';
                  }
                  printed = 1;
               }
               translations+=2;
            }
         }

         if (!printed)
         {
            if (!spacePrinted || *p != ' ')
            {
               char tmp[2];
               tmp[0] = *p;
               tmp[1] = '\0';
               logLine = concat_string_in_stack(logLine, tmp);
               spacePrinted = *p == ' ';
            }
            p++;
         }
      }
      // trim
      while(*logLine != '\n' && *logLine == ' ')
         logLine++;
      for(int i = strlen(logLine)-1; logLine[i] == ' '; i++)
         logLine[i] = '\0';
      printf("%s\n", logLine);
   }
   else
   {
      char* t = e->text;

      int l = 0;
      int done = (*t == '\0');
      while(!done)
      {
         int end;
         for(end = 0; t[end] != '\0' && t[end] != '\n';end++)
            ;
         assert (t[end] == '\0' || t[end] == '\n');

         done = (t[end] == '\0');
         if (!done)
            t[end] = '\0';
         moveCursor(areas[e->logId]->line+l, areas[e->logId]->column);
         printf("%s", t);
         fflush(stdout);
         t += end+1;
         l++;
      }
   }
}

int validLineModeTranslation(char** lineModeTranslations)
{
   int res = 1;
   if (lineModeTranslations != NULL)
   {
      char** translations = lineModeTranslations;
      while (res && *translations != NULL)
      {
         res = *(translations+1) != NULL;
         translations+=2;
      }
   }
   return res;
}

/**
 * In line mode all "logging" is filtered and presented line by line
 */
int lineMode()
{
   return _lineMode_;
}

void switchToLineMode()
{
   assert (!lineMode());

   _lineMode_ = 1;
}

void switchToWindowMode()
{
   assert (lineMode());

   _lineMode_ = 0;
}

void addToFilterOut(char** remove)
{
   assert (remove != NULL);

   while(*remove != NULL)
   {
      assert (filterOutLength < MAX_FILTER);
      filterOut[filterOutLength] = *remove;
      filterOutLength++;
      remove++;
   }
}

static Regist* newRegist(char* name, int line, int column, int numLines, int numColumns, char** lineModeTranslations)
{
   Regist* res = (Regist*)memAlloc(sizeof(Regist));
   res->name = name;
   res->line = line;
   res->column = column;
   res->numLines = numLines;
   res->numColumns = numColumns;
   res->lineModeTranslations = lineModeTranslations;
   return res;
}

static Event* newEvent(int logId, char* text)
{
   Event* res = (Event*)memAlloc(sizeof(Event));
   res->logId = logId;
   res->text = strdup(text);
   return res;
}

static void destroyEvent(Event* e)
{
   free(e->text);
   free(e);
}

void dummyLogger()
{
   // this function is not to be used!!! (it is simply to show something in the given code)

   if (eventExists())
      processEvents();
}
