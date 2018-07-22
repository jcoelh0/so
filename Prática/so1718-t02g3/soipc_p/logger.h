/**
 *  \brief Log module (visualizes the simulation).
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef LOGGER_H
#define LOGGER_H

void initLogger();
void termLogger();;
int validLineModeTranslation(char** lineModeTranslations);
int registerLogger(char* name, int line, int column, int numLines, int numColumns, char** lineModeTranslations);
int lineMode();
void switchToLineMode();
void switchToWindowMode();
void addToFilterOut(char** remove);
int validLogId(int logId);
int getLineLogger(int logId);
int getColumnLogger(int logId);
int getNumLinesLogger(int logId);
int getNumColumnsLogger(int logId);
void sendLog(int logId, char* text);
void* mainLogger(void* arg);

#endif
