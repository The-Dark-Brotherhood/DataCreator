
#include "../inc/DataCreator.h"

int main(int argc, char* argv)
{

  //set up the semaphore
  int semid = semget (KEY, 1, IPC_CREAT | 0777);
  if (semctl (semid, 0, SETALL, init_values) == -1)
  {
    printf("error\n");
  }
  int qID = -1;
  key_t msgKey = 0;
  //Loop checking for existance of queue, sleeping 10 seconds if not found,
  //then searching again
  while((qID = checkForQueue(msgKey)) < 0)
  {
    sleep(10);
  }
  machineProcessingLoop();

  return 0;
}


// FUNCTION      : checkForQueue
// DESCRIPTION   : check for the existance of message queue
//
// PARAMETERS    :
//	key_t msgKey -> message queue key to search by
//
//  RETURNS       :   ID of the message queue,
//                    -1 if the queue does not exist
int checkForQueue(key_t msgKey)
{
  int qID = 0;
  qID = msgget(msgKey,0);

  //msgget returns -1 if the queue does not exist
  return qID;
}

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

// FUNCTION      : sendMessage
// DESCRIPTION   : compile the message into message struct, send to message queue,
//                 call the function to write to log file.
//
// PARAMETERS    :
//	int status       -> the status of the machine
//  int machineID    -> the ID of the machine
//  long messageType -> type of message(always 0)
//  int msgQueueID   -> ID of the message queue where message will be sent
//
//  RETURNS       :   void
void sendMessage(int status, int machineID, long messageType, int msgQueueID)
{
  //put machine info into struct
  MachineMessage message;
  message.type = messageType;
  message.id = machineID;
  message.status = status;
  //get size for msgsend()
  int size = sizeof(message) - sizeof(long);

  //send message to queue
  //msgsend(msgQueueID, (void*)&message, size, 0);

  //write to logfile:
  writeToLog(machineID, status);
}

// FUNCTION      : machineProcessingLoop
// DESCRIPTION   : main processing loop for machine. Send message, write to log
//                 file, then wait 10-30 seconds, repeat. Exits until status 6 is
//                 generated
//
// PARAMETERS    : void
//
//  RETURNS      : void
void machineProcessingLoop(void)
{
  //First get the message queue id
  int qID = 0;

  //get the process ID(to include in message and log)
  int pid = getpid();

  //send first message - ALL OKAY
  sendMessage(EVERYTHING_OKAY, pid, 1, qID);

  //seed the random number generator
  srand(time(0));

  //create the filepath if it doesn't exist
  createFilePathIfNotExists();

  //Loop ends when machine status 6 is generated
  int running = TRUE;
  while(running)
  {
    //sleep random amount of time
    //sleep((rand() % 21)+10); //sleep between 10 & 30 seconds

    //generate random value between 0 & 6
    int machineStatus = ((rand()%7));
    //send the message to the message queue - logging called in sendMessage()
    sendMessage(machineStatus, pid, 0, qID);

    //Machine status = "Machine is Off-Line"
    if (machineStatus == 6)
    {
      running = FALSE;
    }
  }

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
	   //Critical ERROR
	}
  //Start of critical region


  //calculate the current time
  char time[50] = "";
  getTime(time);

  //open file, write log entry, then close file
  FILE * fp = fopen (LOG_FILE_PATH, "a");
  fprintf(fp, "[%s] : DC [%d] - MSG SENT - Status %d (%s)\n", time, pid, status, description);
  fclose(fp);

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
