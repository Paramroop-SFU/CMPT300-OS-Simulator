#include "commands.h"

List *queue1, *queue0, *queue2;
List *blockQueue, *blockedALLsem;
bool first_time;
static int pid_valueGenerator = 1;

Process *init;
Process *Running;

// 0 to 4 sem ID
Semaphore sem[5];

void print_rec()
{
	if (Running->recieveStatus == prints)
	{
		printf("The Message is: %s",Running->message);
		free(Running->message);
		Running->message = NULL;
		Running->recieveStatus = none;
	}
}
// Note since im using queue
void starvation()
{
	// move everything in queue 1 to queue 0
	while(List_count(queue1) > 0)
	{
		List_first(queue1);
		Process* temp = List_remove(queue1);
		temp->curPriority--;
		List_append(queue0,temp);
	}
	while(List_count(queue2) > 0)
	{
		List_first(queue2);
		Process* temp = List_remove(queue2);
		temp->curPriority--;
		List_append(queue1,temp);
	}
}
// only one process can be running at a time so I will just make a Variable that that holds the running process
// Need to make a init processes, most likly best in main

void printProcess(Process *item)
{
	if (item == NULL)
		return;

	char *message = item->message;

	printf("  PID: %d\n  Orignal Priority: %d\n  Current Priority: %d \n", item->pid, item->orgPriority, item->curPriority);

	if (item->recieveStatus == none)
		printf("  message status: None\n");
	else if (item->recieveStatus == waiting_for_response)
		printf("  message status: Waiting and blocked (excluding init its not blocked)\n");
	else
		printf("  message status: Message in the inbox\n");

	if (item->replystatus == nones)
		printf("  message reply status: None\n");
	else if (item->replystatus == needs_to_reply)
		printf("  message reply status: Can reply \n");

	printf("  Message: %s\n", message);

	if (item->status == queued)
		printf("  Status: queued\n");
	else if (item->status == running)
		printf("  Status: running\n");
	else
		printf("  Status: blocked\n");

	printf("\n");
}

void printQueueInfo(List *queue)
{
	Node *current = queue->pFirstNode;

	if (current == NULL)
	{
		printf("  No processes in the queue.\n");
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
	temp->curPriority = 3;
	temp->orgPriority = 3;
	temp->status = running;
	temp->message = NULL;
	temp->recieveStatus = none;
	temp->replystatus = nones;
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

	if (List_count(queue0) > 0)
	{
		List_first(queue0);
		
			availProcess = List_remove(queue0);
		
	}
	else if (List_count(queue1) > 0)
	{
		List_first(queue1);
		
			availProcess = List_remove(queue1);
		
	}
	else if (List_count(queue2) > 0)
	{
		List_first(queue2);
		
		availProcess = List_remove(queue2);
		
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
	if (priority < 0 || 2 < priority)
	{
		printf("The value of the priorty provided is to big or small\nit needs to be either 0,1,2\n");
		return false;
	}

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
		process->status = running;
		init->status = queued;
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
	if(pid == 0)
	{
		printf("Cannot kill init while other processes exists!\n");
		return false;
	}
		

	List *queues[] = {queue0, queue1, queue2, blockQueue, blockedALLsem};
	for (int i = 0; i < 5; ++i)
	{
		List_first(queues[i]);
		void *value = List_search(queues[i], compareFunct, &pid);
		if (value != NULL)
		{
			
			Process* temp = (List_remove(queues[i]));
			if (temp->message != NULL)
			{
				free(temp->message);
			}
			free(temp);
			return true;
		}
	}
	printf("PID provided does not exist\n");
	return false;
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
	if (Running->message != NULL)
	{
		free(Running->message);
	}
	free(Running);
	Running = NULL;
	// find a unblock process
	Process *value = getUnblockfromQueue();
	if (value != NULL)
	{
		value->status = running;
		Running = value; // set the new Running
		return true;
	}
	else
	{
		Running = init;
		init->status = running;
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
	Running->status = queued;
	if (priorty == 0)
		List_append(queue0, Running);

	else if (priorty == 1)
		List_append(queue1, Running);
	else if (priorty == 1)
		List_append(queue2, Running);
	ChosenOne->status = running;
	Running = ChosenOne;
	return true;
}

bool Send(int pid, char *msg) // assume data already allocated, also need to figure out multiple sends and then free mem
{							  // need to add check for init

	Process *sendingTO = NULL;
	List *queues[] = {queue0, queue1, queue2, blockQueue, blockedALLsem};
	for (int i = 0; i < 5; i++)
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
	if (pid == Running->pid)
	{
		sendingTO = Running;
	}
	if (pid == 0)
	{
		sendingTO = init;
	}
	// if pid not found after all the queues
	if (sendingTO == NULL)
	{
		// print statement if not found then does not block
		printf("PID does not exist in the system\n");
		free(msg);
		return false;
	}
	
	// check if proccess already has a pending message
	if (sendingTO->message != NULL || sendingTO->recieveStatus == waiting_for_response)
	{
		
		printf("Process %d already has a pending message or will only recieve from a reply\n", pid);
		free(msg);
		Running->status = blocked;
		Running->recieveStatus = waiting_for_response;
		Running->replystatus = nones;
		List_append(blockQueue, Running); // message blocked queue
		Running = NULL;
		Process *tempvalue = getUnblockfromQueue();
		if (tempvalue == NULL)
		{

			Running = init;
			Running->status = running;
			return true;
		}
		Running = (Process *)tempvalue;
		Running->status = running;
		return false;
	}

	// change the message it sending to data
	if (sendingTO->recieveStatus == receive_Wout_message)
	{
		sendingTO->message = msg;
		sendingTO->replystatus = nones;
		sendingTO->recieveStatus = prints;
		List_first(blockQueue);
		List_search(blockQueue, compareFunct, &(sendingTO->pid));
		List_remove(blockQueue); // remove block process from the blocked queue
		if (Running->pid == 0)
		{
			Running = sendingTO;
			Running->status = running;
			printf("The message is: %s\n",msg);
			free(msg);
			
		}
		else
		{
			int priorty = sendingTO->curPriority;
		sendingTO->status = queued;
		if (priorty == 0)
			List_append(queue0, sendingTO);

		else if (priorty == 1)
			List_append(queue1, sendingTO);
		else if (priorty == 1)
			List_append(queue2, sendingTO);
		}
		
			

	}
	else
	{
		sendingTO->message = msg;
	sendingTO->replystatus = needs_to_reply;
	sendingTO->recieveStatus = message_in_inbox;
	}
	
	

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
	Running->status = running;
	return true;
}

bool Receive()
{

	if (Running->recieveStatus == message_in_inbox)
	{
		printf("Message received %s \n", Running->message);
		Running->recieveStatus = none;
		free(Running->message);
		Running->message = NULL;
		return true;
	}
	else
	{
		if (Running->pid == 0)
		{
			Running->recieveStatus = waiting_for_response;
			return false;
		}
		Running->status = blocked;
		Running->recieveStatus = receive_Wout_message;
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

bool Reply(int pid, char *msg) // if unblocked check if init is running
{
	Process *replyTO = NULL;
	List *queues[] = {queue0, queue1, queue2, blockQueue, blockedALLsem};
	for (int i = 0; i < 5; i++)
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
	if (pid == Running->pid)
	{
		replyTO = Running;
	}
	if (pid == 0)
	{
		replyTO = init;
	}
	// if pid not found after all the queues
	if (replyTO == NULL)
	{
		free(msg);
		// print statement if not found then does not block
		printf("PID does not exist in the system\n");
		return false;
	}
	if (replyTO->message != NULL) // this case should never occur if implemented correctly
	{
		printf("Process %d already has a pending message\n", pid);
		free(msg);
		return false;
	}

	if (replyTO->recieveStatus == waiting_for_response)
	{
		List_first(blockQueue);
		List_search(blockQueue, compareFunct, &(replyTO->pid));
		List_remove(blockQueue); // remove block process from the blocked queue
		if (Running->pid == 0)
		{
			Running = replyTO;
			Running->status = running;
			replyTO->message = msg;
			replyTO->recieveStatus = message_in_inbox;
			replyTO->replystatus = nones;
			return true;
		}
		int priorty = replyTO->curPriority;
		replyTO->status = queued;
		if (priorty == 0)
			List_append(queue0, replyTO);

		else if (priorty == 1)
			List_append(queue1, replyTO);
		else if (priorty == 1)
			List_append(queue2, replyTO);
	}
	else if (replyTO->recieveStatus == receive_Wout_message) // if can only get block from send
	{
		free(msg);
		printf("Can only receive from a send\n");
		return false;
	}	
	else
	{
		free(msg);
		printf("The Process that you're replying is currently not waiting for a response\n");
		return false;
	}

	replyTO->message = msg; // give the ex-blocked process its data
	replyTO->recieveStatus = message_in_inbox;
	replyTO->replystatus = nones;
	return true;
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
			printf("Sem has already been made!\n");
		}
	}
	else
	{
		printf("Sem ID not found!\n");
	}
	return false;
}

bool SemaphoreP(int semaphore) // blocks the process if S<=0
{							   // 0 to 4 sem ID

	// check
	if (!(semaphore >= 0 && semaphore < 5))
	{
		printf("Semphore ID not in bounds\n");
		return false;
	}
	if (sem[semaphore].created == false)
	{
		printf("Semaphore has not been intilized\n");
		return false;
	}
	if (Running->pid == 0)
	{
		printf("Init cannot be blocked so it cannot use Semaphores\n");
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
		printf("Semphore ID not in bounds\n");
		return false;
	}
	if (sem[semaphore].created == false)
	{
		printf("Semaphore has not been intilized\n");
		return false;
	}

	sem[semaphore].val++;

	if (sem[semaphore].val > 0) // two cases now
	{

		Process *temp = (Process *)List_trim(sem[semaphore].processesWaiting);
		if (temp != NULL)
		{
			List_first(blockedALLsem);								// make first on list
			List_search(blockedALLsem, compareFunct, &(temp->pid)); // find pid from this
			List_remove(blockedALLsem);
			if (Running->pid == 0)
			{
				Running = temp;
				Running->status = running;
				sem[semaphore].val--;
				return true;
			}

			const int priority = temp->curPriority;
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
	List *queues[] = {queue0, queue1, queue2, blockQueue, blockedALLsem};
	for (int i = 0; i < 5; ++i)
	{
		List_first(queues[i]);
		process = List_search(queues[i], compareFunct, &pid);
		if (process != NULL)
			break;
	}
	if (pid == Running->pid)
	{
		process = Running;
	}
	// When process pid found in the queues
	if (process != NULL)
	{
		printProcess(process);

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
	printf("\n Running Process:\n");
	if (Running != NULL)
	{
		printProcess(Running);
	}
	else
	{
		printf("No running process\n");
	}

	printf("\n Priority 0 (High):\n");
	printQueueInfo(queue0);

	printf("\n Priority 1 (Normal):\n");
	printQueueInfo(queue1);

	printf("\n Priority 2 (Low):\n");
	printQueueInfo(queue2);
	printf("\n Blocked Messages:\n");
	printQueueInfo(blockQueue);
	printf("\n Blocked by semaphore:\n");
	printQueueInfo(blockedALLsem);
	if (Running->pid != 0)
	{
		printf("\n Init Process:\n");
		printProcess(init);
	}
	
}
