#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <string.h>


enum status
{
	queued,
	running,
	blocked,

}; typedef enum status Status;

enum message
{
	none,
	waiting_for_response,
	needs_to_reply

}; typedef enum message MessageStatus;

struct process
{
	int orgPriority;
	int curPriority;
	Status status;
	int pid;
	char*message;
	char*reply;
	MessageStatus messagestatus;

}; typedef struct process Process;

extern List *queue0,*queue1,*queue2;
extern Process* Running;
extern List* blockedQueue;

bool Create(int priority);

bool Fork();

bool Kill(int pid);

bool Exit();

bool Quantum();

bool Send(int pid, char * msg);

bool Receive();

bool Reply(int pid,char* msg);

bool newSemaphore(int semaphore, int initial);

bool SempahoreP(int semaphore);

bool SempahoreV(int semaphore);

bool Procinfo(int pid);

void Totalinfo();
