#include "commands.h"

List *queue1, *queue0, *queue2;
List *blockQueue, *blockedALLsem;
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

	printf("PID: %d\nOrignal Priority: %d\nCurrent Priority: %d \n", item->pid, item->orgPriority, item->curPriority);

	if (item->recieveStatus == none)
		printf("message status: None\n");
	else if (item->recieveStatus == waiting_for_response)
		printf("message status: Waiting and blocked\n");
	else
		printf("message status: Message in the inbox\n");

	if (item->replystatus == nones)
		printf("message reply  status: None\n");
	else if (item->replystatus == needs_to_reply)
		printf("message reply status: Can reply \n");

	printf("Message: %s\n", message);

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
		blockQueue = List_create();
		blockedALLsem = List_create();
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

// checks if there is a process
bool getfromQueue() // helper functions
{

	if (List_count(queue0) > 0)
	{
		return true;
	}

	else if (List_count(queue1) > 0)
	{
		return true;
	}
	else if (List_count(queue2) > 0)
	{
		return true;
	}
	else if (List_count(blockQueue) > 0)
	{
		return true;
	}
	if (List_count(blockedALLsem) > 0)
		return true;

	return false;
}

Process *getUnblockfromQueue() // helper functions
{
	Process *availProcess = NULL;
	int m = 0;
	if (List_count(queue0) > 0)
	{
		List_first(queue0);
		if ((Process *)List_search(queue0, UnblockcompareFunct, &m) != NULL)
		{
			availProcess = List_remove(queue0);
		}
	}
	else if (List_count(queue1) > 0)
	{
		List_first(queue1);
		if ((Process *)List_search(queue1, UnblockcompareFunct, &m) != NULL)
		{
			availProcess = List_remove(queue1);
		}
	}
	else if (List_count(queue2) > 0)
	{
		List_first(queue2);
		if ((Process *)List_search(queue2, UnblockcompareFunct, &m) != NULL)
		{
			availProcess = List_remove(queue2);
		}
	}

	return availProcess;
}

bool PIDexists(int pid)
{
	bool found = false;
	Process *exist = NULL;
	if (pid == 0)
	{
		return true;
	}
	if (List_count(queue0) > 0)
	{
		List_first(queue0);
		exist = List_search(queue0, compareFunct, &pid);
		if (exist != NULL)
		{
			found = true;
			return found;
		}
	}

	if (List_count(queue1) > 0)
	{
		List_first(queue1);
		exist = List_search(queue1, compareFunct, &pid);
		if (exist)
		{
			found = true;
			return found;
		}
	}
	if (List_count(queue2) > 0)
	{
		List_first(queue2);
		exist = List_search(queue2, compareFunct, &pid);
		if (exist != NULL)
		{
			found = true;
			return found;
		}
	}

	if (List_count(blockQueue) > 0)
	{
		List_first(blockQueue);
		exist = List_search(blockQueue, compareFunct, &pid);
		if (exist != NULL)
		{
			found = true;
			return found;
		}
	}

	if (List_count(blockedALLsem) > 0)
	{
		List_first(blockedALLsem);
		exist = List_search(blockedALLsem, compareFunct, &pid);
		if (exist != NULL)
		{
			found = true;
			return found;
		}
	}
	return false;
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
	process->replystatus = nones;
	process->recieveStatus = none;
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
	process->replystatus = nones;
	process->recieveStatus = none;
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
	List *queues[] = {queue0, queue1, queue2, blockQueue, blockedALLsem};
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
		bool exist = getfromQueue();
		// if tempvalue = null then init is the only process remaining and kill simulation
		if (!exist)
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
		Running = init;
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

bool Send(int pid, char *msg) // assume data already allocated, also need to figure out multiple sends and then free mem
{							  // need to add check for init

	Process *sendingTO = NULL;
	List *queues[] = {queue0, queue1, queue2, blockQueue, blockedALLsem};
	for (int i = 0; i < 3; i++)
	{
		// look for the process to be sent to
		List_first(queues[i]);
		sendingTO = List_search(queues[i], compareFunct, &pid);
		// found
		if (sendingTO != NULL)
		{
			break;
		}
	}
	// if pid not found after all the queues
	if (sendingTO == NULL)
	{
		// print statement if not found then does not block
		printf("PID does not exist in the system\n");
		return false;
	}
	// check if proccess already has a pending message
	if (sendingTO->message != NULL)
	{
		printf("Process %d already has a pending message\n",pid);
		free(msg);
		return false;
	}



	// change the message it sending to data
	sendingTO->message = msg;
	sendingTO->replystatus = needs_to_reply;
	sendingTO->replyPID = Running->pid;
	sendingTO->recieveStatus = message_in_inbox;
	
	// check if Running is init since it cant get blocked
	if (Running->pid == 0)
	{
		Running->recieveStatus = waiting_for_response;
		Running->replystatus = nones;
		return true;
	}

	Running->status = blocked;
	Running->recieveStatus = waiting_for_response;
	Running->replystatus = nones;
	List_append(blockQueue, Running); // message blocked queue
	Running = NULL;
	Process *tempvalue = getUnblockfromQueue();
	if (tempvalue == NULL)
	{
		Running = init;
		return true;
	}
	Running = (Process *)tempvalue;
	return true;
}

bool Receive()
{

	if (Running->recieveStatus == message_in_inbox)
	{
		printf("Message received; %s \n", Running->message);
		Running->recieveStatus = none;
		free(Running->message);
		Running->message = NULL;
		return true;
	}
	else
	{
		Running->status = blocked;
		Running->recieveStatus = waiting_for_response;
		List_append(blockQueue, Running);
		Running = NULL;

		Process *temp = getUnblockfromQueue();
		if (temp == NULL)
		{
			Running = init;
			return false;
		}
		else
		{
			Running = temp;
			return false;
		}
	}
	return true;
}

bool Reply(int pid, char *msg)
{
	Process *replyTO = NULL;
	List *queues[] = {queue0, queue1, queue2, blockQueue, blockedALLsem};
	for (int i = 0; i < 3; i++)
	{
		// look for the process to be sent to
		List_first(queues[i]);
		replyTO = List_search(queues[i], compareFunct, &pid);
		// found
		if (replyTO != NULL)
		{
			break;
		}
	}
	// if pid not found after all the queues
	if (replyTO == NULL)
	{
		// print statement if not found then does not block
		printf("PID does not exist in the system\n");
		return false;
	}
	if (replyTO->message != NULL) // this case should never occur if implemented correctly
	{
		printf("Process %d already has a pending message\n",pid);
		free(msg);
		return false;
	}
	
	if (replyTO->recieveStatus == waiting_for_response)
	{
		List_first(blockQueue);
		List_search(blockQueue,compareFunct,&(replyTO->pid));
		List_remove(blockQueue);
		int priorty = replyTO->pid;
		if (priorty == 0)
		List_append(queue0,replyTO );

		else if (priorty == 1)
			List_append(queue1,replyTO);
		else if (priorty == 1)
			List_append(queue2, replyTO);
		
	}

	replyTO->message = msg;
	replyTO->recieveStatus = message_in_inbox;
	replyTO->replystatus = nones;
	return true;
	
	// if (Running->messagestatus != needs_to_reply)
	// 	return false;
	// // Seach for the pid process
	// Process *replyTo = NULL;
	// List *queues[] = {queue0, queue1, queue2};
	// for (int i = 0; i < 3; i++)
	// {
	// 	List_first(queues[i]);
	// 	replyTo = (Process *)List_search(queues[i], compareFunct, &pid);

	// 	if (replyTo != NULL)
	// 	{
	// 		break;
	// 	}
	// }

	// if (replyTo != NULL && replyTo->messagestatus == waiting_for_response)
	// {
	// 	replyTo;

	// 	return true;
	// }
	// return false; // in main make a printf
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
{							   // 0 to 4 sem ID

	// check
	if (!(semaphore >= 0 && semaphore < 5))
	{

		return false;
	}
	if (sem[semaphore].created == false)
	{
		return false;
	}

	if (sem[semaphore].val > 0)
	{
		// decrement by 1 but do not block
		sem[semaphore].val--;
	}
	else
	{
		// need to block process
		Running->status = blocked;
		List_prepend(sem[semaphore].processesWaiting, Running); // add to the p block queue
		List_prepend(blockedALLsem, Running);
		// add old value to the queue
		Process *temp = getUnblockfromQueue();
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
	if (sem[semaphore].created == false)
	{
		return false;
	}

	sem[semaphore].val++;

	if (sem[semaphore].val > 0) // two cases now
	{

		Process *temp = (Process *)List_trim(sem[semaphore].processesWaiting);
		if (temp != NULL)
		{
			List_first(blockedALLsem);							 // make first on list
			List_search(blockedALLsem, compareFunct, &(temp->pid)); // find pid from this
			List_remove(blockedALLsem);
			int priority = temp->curPriority;
			if (priority == 0)
				List_append(queue0, temp); // highest priorty
			else if (priority == 1)
				List_append(queue1, temp);
			else
				List_append(queue2, temp);
			temp->status = queued;
			sem[semaphore].val--;
		}
	}
	return true;
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

		if (process->recieveStatus == none)
		{
			printf("has not recievied a message\n");
		}
		else if (process->recieveStatus == message_in_inbox)
		{
			printf("message pending and waiting to be recieved\n");
		}
		else
		{
			printf("Waiting for a reply");
		}

		if (process->replystatus == needs_to_reply)
		{
			printf("needs to reply to process to process %d\n", process->pid);
		}

		printf("Message: %s\n", process->message);

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
