#include "commands.h"

List *queue1, *queue0, *queue2;
bool first_time;
static int pid_valueGenerator = 1;
// Running to NULL for setup???? -- should be set to init
Process *init;
Process *Running;

// 0 to 4 sem ID
Semaphore sem[5];

// Note since im using queue


// only one process can be running at a time so I will just make a Variable that that holds the running process
// Need to make a init processes, most likly best in main

void printProcess(Process *item)
{
	if (item == NULL)
		return;

	char *message = item->message;
	char *reply = item->reply;
	printf("PID: %d\nOrignal Priority: %d\nCurrent Priority: %d \n", item->pid, item->orgPriority, item->curPriority);
	if (item->messagestatus == none)
		printf("message status: None\n");
	else if (item->messagestatus == waiting_for_response)
		printf("message status: Waiting\n");
	else
		printf("message status: Need to Reply\n");

	printf("Message: %s\n", message);
	printf("reply: %s\n", reply);
	if (item->status == queued)
		printf("Status: queued\n");
	else if (item->status == running)
		printf("Status: running\n");
	else
		printf("Status: blocked\n");
}

void printQueueInfo(List *queue)
{
	Node *current = queue->pFirstNode;

	if (current == NULL)
	{
		printf("No processes in the queue.\n");
		return;
	}

	while (current != NULL)
	{
		printProcess((Process *)current->pItem);
		current = current->pNext;
	}
}

void Init()
{
	Process *temp = (Process *)malloc(sizeof(Process));
	temp->pid = 0;
	init = temp;
	Running = init;
	if (!first_time)
	{
		queue0 = List_create();
		queue1 = List_create();
		queue2 = List_create();
		// Set it to to true, for after first time
		first_time = true;
	}
}

void initSem()
{
	for (int i = 0; i < 5; i++)
	{
		sem[i].val = 0;
		sem[i].processesWaiting = List_create();
		sem[i].created = false;
	}
}

bool compareFunct(void *pItem, void *pComparisonArg)
{
	Process *temp = (Process *)pItem;
	int *pidCompare = (int *)pComparisonArg;
	if (temp->pid == *pidCompare)
		return true;

	return false;
}

bool UnblockcompareFunct(void *pItem, void *pComparisonArg)
{
	Process *temp = (Process *)pItem;
	int *pidCompare = (int *)pComparisonArg;
	*pidCompare = 1;
	if (temp->status != blocked)
		return true;

	return false;
}

// looks for the first aviable process in queue
// from highest priority to lowest
Process *getfromQueue() // helper functions
{
	Process *availProcess = NULL;
	if (List_count(queue0) > 0)
	{
		List_first(queue0);
		availProcess = (Process *)List_remove(queue0);
	}
	
	else if (List_count(queue1) > 0)
	{
		List_first(queue1);
		availProcess = (Process *)List_remove(queue1);
	}
	else if (List_count(queue2) > 0)
	{
		List_first(queue2);
		availProcess = (Process *)List_remove(queue2);
	}

	return availProcess;
}

Process *getUnblockfromQueue() // helper functions
{
	Process *availProcess = NULL;
	int m = 0;
	if (List_count(queue0) > 0)
	{
		List_first(queue0);
		if ((Process *)List_search(queue0, UnblockcompareFunct, &m))
		{
			availProcess = List_remove(queue0);
		}
	}
	else if (List_count(queue1) > 0)
	{
		List_first(queue1);
		if ((Process *)List_search(queue1, UnblockcompareFunct, &m))
		{
			availProcess = List_remove(queue1);
		}
	}
	else if (List_count(queue2) > 0)
	{
		List_first(queue2);
		if ((Process *)List_search(queue2, UnblockcompareFunct, &m))
		{
			availProcess = List_remove(queue2);
		}
	}

	return availProcess;
}

// Proccess functions ----------------------------------------------------------------------------

bool Create(int priority)
{

	Process *process = (Process *)malloc(sizeof(Process));
	process->curPriority = priority;
	process->orgPriority = priority;
	process->status = queued;
	process->pid = pid_valueGenerator++;
	process->message = NULL;
	process->reply = NULL;
	process->messagestatus = none;
	if (Running->pid == 0)
	{
		Running = process;
		return true;
	}
	int success;
	if (priority == 0)
		success = List_append(queue0, process); // highest priorty
	else if (priority == 1)
		success = List_append(queue1, process);
	else
		success = List_append(queue2, process);

	if (success == -1)
		return false;
	else
		return true;
}

bool Fork()
{
	// the init process should fail
	if (Running->pid == 0 || Running == NULL)
	{
		printf("CANNOT FORK INIT\n");
		return false;
	}

	Process *process = (Process *)malloc(sizeof(Process));
	process->curPriority = Running->orgPriority;
	process->orgPriority = Running->orgPriority;
	process->status = queued;
	process->pid = pid_valueGenerator++;
	process->message = NULL;
	process->reply = NULL;
	process->messagestatus = none;
	int success;
	if (Running->orgPriority == 0)
		success = List_append(queue0, process); // highest priorty
	else if (Running->orgPriority == 1)
		success = List_append(queue1, process);
	else
		success = List_append(queue2, process);

	if (success == -1)
		return false;
	else
		return true;
}

bool Kill(int pid) // kills no matter what excluding init, rn does not kill running
{
	if (Running->pid == pid)
	{
		Exit(); // using function below
		return true;
	}
	bool found = false;
	List *queues[] = {queue0, queue1, queue2};
	for (int i = 0; i < 3; ++i)
	{
		List_first(queues[i]);
		void *value = List_search(queues[i], compareFunct, &pid);
		if (value != NULL)
		{
			free(List_remove(queues[i]));
			found = true;
			break;
		}
	}
	return found;
}

bool Exit() // removes running, not freeing init
{
	if (Running->pid == 0)
	{
		// check if there are blocked processes
		void *tempvalue = getfromQueue();
		// if tempvalue = null then init is the only process remaining and kill simulation
		if (tempvalue == NULL)
		{
			free(Running);
			Running = NULL;
			exit(1); // could pick better way
			return true;
		}
		// if temp does not equal NULL the other processes should be blocked

		return false;
	}

	// this section for if the Running does not have init process, but a normal process
	free(Running);
	Running = NULL;
	// find a unblock process
	Process *value = getUnblockfromQueue();
	if (value != NULL)
	{
		Running = value; // set the new Running
		return true;
	}
	else
	{
		Running = init;
		return true;
	}
}

bool Quantum() // simulates a timer running out, so put it back to the queue
{

	if (Running->pid == 0)
	{
		return false;
	}
	const int priorty = Running->curPriority;
	Process *ChosenOne = getUnblockfromQueue();
	if (ChosenOne == NULL) // if null then the current process gets to stay
	{
		return false;
	}
		

	if (priorty == 0)
		List_append(queue0, Running);

	else if (priorty == 1)
		List_append(queue1, Running);
	else if (priorty == 1)
		List_append(queue2, Running);

	Running = ChosenOne;
	return true;
}

bool Send(int pid, char *msg) // assume data already allocated
{
	if (Running != NULL)
	{
		Process *recv = NULL;
		List *queues[] = {queue0, queue1, queue2};
		for (int i = 0; i < 3; i++)
		{
			// look for the process to be sent to
			List_first(queues[i]);
			recv = List_search(queues[i], compareFunct, &pid);
			// found
			if (recv != NULL)
			{
				break;
			}
		}
		// if pid not found after all the queues
		if (recv == NULL)
		{
			// print statement
			return false;
		}

		strncpy(recv->message, msg, MAX_MESSAGE_LENGTH - 1);
		recv->messagestatus = waiting_for_response;
		Running->status = blocked;
		// List_append(blockedQueue, Running);
		void *tempvalue = getfromQueue();
		Running = (Process *)tempvalue;
		return true;
	}
	else
	{
		return false;
	}
}

bool Receive()
{
	if (Running != NULL)
	{
		if (Running->messagestatus == waiting_for_response)
		{
			printf("Message received; %s \n", Running->message);
			Running->messagestatus = none;
			Process *send = (Process *)getfromQueue();
			send->status = queued;
			Running = send;
			return true;
		}
	}
	return false;
}

bool Reply(int pid, char *msg)
{
	// Seach for the pid process
	Process *replyTo = NULL;
	List *queues[] = {queue0, queue1, queue2};
	for (int i = 0; i < 3; i++)
	{
		List_first(queues[i]);
		replyTo = (Process *)List_search(queues[i], compareFunct, &pid);

		if (replyTo != NULL)
		{
			break;
		}
	}

	if (replyTo != NULL)
	{
		strncpy(replyTo->reply, msg, MAX_MESSAGE_LENGTH - 1);
		replyTo->messagestatus = needs_to_reply;
		if (replyTo->status == blocked)
		{
			replyTo = queued;
			List_append(queues[replyTo->curPriority], replyTo);
		}
		return true;
	}
	return false;
}

bool newSemaphore(int semaphore, int initial)
{
	// 0 to 4 sem ID
	if (semaphore >= 0 && semaphore < 5 && initial >= 0)
	{
		if (!sem[semaphore].created)
		{
			sem[semaphore].val = initial;
			sem[semaphore].created = true;
			return true;
		}
		else
		{
			printf("Sem already made \n");
		}
	}
	else
	{
		printf("Invalid Sem ID \n");
	}
	return false;
}

bool SemaphoreP(int semaphore) // blocks the process if S<=0
{ // 0 to 4 sem ID
	
	//check 
	if (!(semaphore >= 0 && semaphore < 5))
	{
		
		return false;
	}
	if(sem[semaphore].created == false)
	{
		return false;
	}

	if (sem[semaphore].val > 0 )
	{
		//decrement by 1 but do not block
		sem[semaphore].val--;
	}
	else 
	{
		// need to block process
		Running->status = blocked;
		List_prepend(sem[semaphore].processesWaiting,Running); // add to the p block queue
		// get new process to running
		const int priorty = Running->curPriority;
		if (priorty == 0)
		List_append(queue0, Running);
		else if (priorty == 1)
		List_append(queue1, Running);
		else if (priorty == 1)
		List_append(queue2, Running);

		// add old value to the queue
		Process* temp = getUnblockfromQueue();
		if (temp == NULL)
		{
			Running = init;
		}
		else
		{
			Running = temp;
		}
		

	}
	
	
	return true;
}

bool SemaphoreV(int semaphore)
{
	// // 0 to 4 sem ID
	if (!(semaphore >= 0 && semaphore < 5))
	{
		
		return false;
	}
	if(sem[semaphore].created == false)
	{
		return false;
	}

	sem[semaphore].val++;

	if (sem[semaphore].val > 0) // two cases now 
	{

		Process* temp = (Process*)List_trim(sem[semaphore].processesWaiting);
		if(temp != NULL)
		{
			temp->status = queued;
			sem[semaphore].val--;
		}
	}
	return true;
	//--------------------------------------------IDK what u were doing -------------------
	// if (semaphore >= 0 && semaphore < 5)
	// {
	// 	// increment by 1
	// 	sem[semaphore].val++;

	// 	// No process waiting in sem
	// 	if (!List_count(sem[semaphore].processesWaiting))
	// 	{

	// 		return true;
	// 	}

	// 	Process *processWaiting = (Process *)List_trim(sem[semaphore].processesWaiting);
	// 	if (processWaiting == NULL)
	// 	{
	// 		return false;
	// 	}

	// 	processWaiting->status = queued;
	// 	List *assignQueue;
	// 	if (processWaiting->curPriority == 0)
	// 	{
	// 		assignQueue = queue0;
	// 	}
	// 	else if (processWaiting->curPriority == 1)
	// 	{
	// 		assignQueue = queue1;
	// 	}
	// 	else if (processWaiting->curPriority == 2)
	// 	{
	// 		assignQueue = queue2;
	// 	}
	// 	else
	// 	{
	// 		free(processWaiting); // WHY ARE YOU FREEING
	// 		return false;
	// 	}

	// 	if (List_prepend(assignQueue, processWaiting) == LIST_SUCCESS)
	// 	{
	// 		return true;
	// 	}
	// 	else
	// 	{
	// 		free(processWaiting);
	// 		return false;
	// 	}
	// }
	// return false;
}

bool Procinfo(int pid)
{
	Process *process = NULL;
	List *queues[] = {queue0, queue1, queue2};
	for (int i = 0; i < 3; ++i)
	{
		List_first(queues[i]);
		process = List_search(queues[i], compareFunct, &pid);
		if (process != NULL)
			break;
	}
	// When process pid found in the queues
	if (process != NULL)
	{
		printf("PID %d:\n", process->pid);
		printf("Original Priority: %d\n", process->orgPriority);
		printf("Current Priority: %d\n", process->curPriority);
		printf("Status: ");
		switch (process->status)
		{
		case queued:
			printf("Queued\n");
			break;
		case running:
			printf("Running\n");
			break;
		case blocked:
			printf("Blocked\n");
			break;
		}
		if (process->messagestatus != none)
		{
			printf("Message Status: ");
			switch (process->messagestatus)
			{
			case waiting_for_response:
				printf("Waiting for Response\n");
				break;
			case needs_to_reply:
				printf("Needs to Reply\n");
				break;
			}
			printf("Message: %s\n", process->message);
			printf("Reply: %s\n", process->reply);
		}
		return true;
	}
	else
	{
		printf("Process with PID %d not found.\n", pid);
		return false;
	}
}

void Totalinfo()
{
	printf("Total Information:\n");
	printf("\nRunning Process:\n");
	if (Running != NULL)
	{
		printProcess(Running);
	}
	else
	{
		printf("No running process\n");
	}

	printf("\nPriority 0 (High):\n");
	printQueueInfo(queue0);

	printf("\nPriority 1 (Normal):\n");
	printQueueInfo(queue1);

	printf("\nPriority 2 (Low):\n");
	printQueueInfo(queue2);

	// printf("\nBlocked Queue:\n");
	// printQueueInfo(blockedQueue);
}
