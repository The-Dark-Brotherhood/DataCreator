#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <time.h>

#define MESSAGE_LENGTH  100
#define MACHINE_ON      1
#define EVERYTHING_OKAY 0
#define KEY             1337
#define LOG_FOLDER_PATH "/tmp/"
#define LOG_FILE_PATH   "/tmp/dataCreator.log"
#define FALSE           0
#define TRUE            1
#define KEY_PATH        "/tmp/"


typedef struct Messages{
  long type;
  int id;
  int status;
} MachineMessage;

void sendMessage(int status, int machineID, long messageType, int msgQueueID);
void machineProcessingLoop(int msgQueueID);
void writeToLog(int pid, int status);
void createFilePathIfNotExists(void);
int setUpLogSemaphore(void);
void closeLogSemaphore(int semId);
void getTime(char* output);
int getQueueID(void);

//the following is defined in DataCreatorLogging.h
extern struct sembuf acquire_operation;
extern struct sembuf release_operation;
