#include "../inc/DataCreator.h"


struct sembuf acquire_operation = { 0, -1, SEM_UNDO };
struct sembuf release_operation = { 0, 1, SEM_UNDO };

unsigned short init_values[1] = { 1 };

// FUNCTION      : getTime
// DESCRIPTION   : get the current time, formatted according to specifications
//                 stored in the parameter passed to function
//
// PARAMETERS    :
//	char* output -> pointer to string where formatted time will be ouput
//
//  RETURNS       :   void
void getTime(char* output)
{
  //Get time info in time_t struct
  time_t rawtime;
  struct tm *info;
  time( &rawtime );
  info = localtime(&rawtime);

  //Set the time output to match the requirements
  //first paramater is where to store outputted string
  strftime(output, 49, "%F %T",info);
}

// FUNCTION      : writeToLog
// DESCRIPTION   : generates output for the logfile, writes to log file
//                  using a semaphore. Calls getTime() in order to get current
//                  time for log file.
//
// PARAMETERS    : int pid -> process ID
//                 int status -> status of machine
//
//  RETURNS      : void
void writeToLog(int pid, int status)
{
  //get the semaphore ID, create if it doesn't exist
  int semid = semget (KEY, 1, IPC_CREAT | 0777);


  //get the description of the status - values 0-6
  char description[255] = "";
  switch(status)
  {
    case 0:
      strcpy(description, "Everything is OKAY");
      break;
    case 1:
      strcpy(description, "Hydraulic Pressure Failure");
      break;
    case 2:
      strcpy(description, "Safety Button Failure");
      break;
    case 3:
      strcpy(description, "No Raw Material in the Process");
      break;
    case 4:
      strcpy(description, "Operating Temperature Out of Range");
      break;
    case 5:
      strcpy(description, "Operator Error");
      break;
    case 6:
      strcpy(description, "Machine is Off-line");
      break;
    default:
      //There should only be a value between 0-6
      strcpy(description, "ERROR READING STATUS");
      break;
  }

  //Check semaphore to see if can access critical region
  if (semop (semid, &acquire_operation, 1) == -1)
	{
	   printf("critical error\n");//Critical ERROR
	}
  //Start of critical region


  //calculate the current time
  char time[50] = "";
  getTime(time);

  //open file, write log entry, then close file
  FILE * fp = fopen (LOG_FILE_PATH, "a");
  if(fp != NULL)
  {
    fprintf(fp, "[%s] : DC [%d] - MSG SENT - Status %d (%s)\n", time, pid, status, description);
    fclose(fp);
  }

  //End of critical region

  if (semop (semid, &release_operation, 1) == -1)
  {
    //Critical ERROR
  }
}

// FUNCTION      : createFilePathIfNotExists
// DESCRIPTION   : this function creates a filepath if that filepath
//               - filepath set in #defines of header file
//
// PARAMETERS    : none
//
//  RETURNS      : void
void createFilePathIfNotExists(void)
{
  struct stat st = {0};
  if (stat(LOG_FOLDER_PATH, &st) == -1)
  {
    mkdir(LOG_FOLDER_PATH, 0700);
  }
}

// FUNCTION      : setUpLogSemaphore
// DESCRIPTION   : this function creates a semaphore that is used by the DCs
//                 for accessing the log file.
//
// PARAMETERS    : none
//
//  RETURNS      : int - semaphore Id
int setUpLogSemaphore(void)
{
  int semid = semget (KEY, 1, IPC_CREAT | 0777);
  if (semctl (semid, 0, SETALL, init_values) == -1)
  {
    printf("error\n");
  }
  return semid;
}

// FUNCTION      : closeLogSemaphore
// DESCRIPTION   : this function closes the semaphore being used by the DC
//
// PARAMETERS    : int semId - semaphore Id
//
//  RETURNS      : none
void closeLogSemaphore(int semId)
{
  semctl (semId, 0, IPC_RMID, 0);

}
