#ifndef COMMANDS_H
#define COMMANDS_H


#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <string.h>

#define MAX_MESSAGE_LENGTH 100
#define MAX_PROCESSES 100
#define MAX_SEMAPHORES 5



typedef enum 
{
	queued,
	running,
	blocked,

}Status;

typedef enum 
{
	none,
	waiting_for_response,
	needs_to_reply
}MessageStatus;

typedef struct 
{
	int orgPriority;
	int curPriority;
	Status status;
	int pid;
	char*message;
	char*reply;
	MessageStatus messagestatus;

}Process;

typedef struct {

	int val;

} Semaphore;


extern List *queue0,*queue1,*queue2;
extern Process* Running;
extern List* blockedQueue;
extern Process*init;
extern bool first_time;


bool Create(int priority);

bool Fork();

bool Kill(int pid);

bool Exit();

bool Quantum();

bool Send(int pid, char * msg);

bool Receive();

bool Reply(int pid,char* msg);

bool newSemaphore(int semaphore, int initial);

bool SemaphoreP(int semaphore);

bool SemaphoreV(int semaphore);

bool Procinfo(int pid);

void Totalinfo();

void Init();
#endif