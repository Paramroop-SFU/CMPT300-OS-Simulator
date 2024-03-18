#include "commands.h"
List*queue1,*queue0,*queue2;
bool first_time = false;
static int pid_valueGenerator = 1;
Process* Running;
List* blockedQueue;
//Note since im using queue
// insert with prepend
// remove with last list_trim
// only one process can be running at a time so I will just make a Variable that that holds the running process
// Need to make a init processes, most likly best in main

bool COMPARATOR_FN(void* pItem, void* pComparisonArg)
{
	Process*temp = (Process*)pItem;
	int* pidCompare = (int*)pComparisonArg;
	if (temp->pid == *pidCompare)
		return true;

	return false;

}

void* getfromQueue()
{
	void* value = List_trim(queue0);
	if (value != NULL)
	{
		return value;
	}
	void* value = List_trim(queue1);
	if (value != NULL)
	{
		return value;
	}
	void* value = List_trim(queue2);

	return value;
}


bool Create(int priority)
{
	if (first_time == false)
	{
		queue0 = List_create();
		queue1 = List_create();
		queue2 = List_create();
		blockedQueue = List_create();

	}

	Process* process = (Process*)malloc(sizeof(Process));
	process->curPriority = priority;
	process->orgPriority = priority;
	process->status = queued;
	process->pid = pid_valueGenerator++;
	process->message = NULL;
	process->reply=NULL;
	process->messagestatus = none;
	int success;
	if (priority == 0)
		 success = List_prepend(queue0,process); // highest priorty
	else if(priority == 1)
		success = List_prepend(queue1,process);
	else
		success = List_prepend(queue2,process);

	if (success == -1)
		return false;
	else
		return true;

}

bool Fork()
{
		Process* process = (Process*)malloc(sizeof(Process));
		process->curPriority = Running->orgPriority;
		process->orgPriority = Running->orgPriority;
		process->status = queued;
		process->pid = pid_valueGenerator++;
		process->message = NULL;
		process->reply=NULL;
		process->messagestatus = none;
			int success;
	if (Running->orgPriority == 0)
		 success = List_prepend(queue0,process); // highest priorty
	else if(Running->orgPriority == 1)
		success = List_prepend(queue1,process);
	else
		success = List_prepend(queue2,process);

	if (success == -1)
		return false;
	else
		return true;

}

bool Kill(int pid) // kills no matter what excluding init, rn does not kill running
{
	List_first(queue0);
	int* value = List_search(queue0,COMPARATOR_FN,pid);
	if (value != NULL)
	{
		free(List_remove(queue0));
		return true;
	}
	List_first(queue1);
	int* value = List_search(queue0,COMPARATOR_FN,pid);
	if (value != NULL)
	{
		free(List_remove(queue1));
		return true;
	}
	List_first(queue2);
	int* value = List_search(queue0,COMPARATOR_FN,pid);
	if (value != NULL)
	{
		free(List_remove(queue2));
		return true;
	}
	return false;

}

bool Exit() // removes running, not freeing init
{
	if (Running == NULL  ||Running->pid == 0)
	{
		Running == NULL;
		return false;
	}
	free(Running);
	void* tempvalue = getfromQueue();
	if (tempvalue == NULL)
	{
		// set init
		return true;
	}
	else
	{
		Running = (List*)getfromQueue;
	}

}

bool Quantum() // simulates a timer running out, so put it back to the queue
{
	const int i = Running->curPriority;

	
}

bool Send(int pid, char * msg)
{

}

bool Receive()
{

}

bool Reply(int pid,char* msg)
{

}

bool newSemaphore(int semaphore, int initial)
{

}

bool SempahoreP(int semaphore)
{

}

bool SempahoreV(int semaphore)
{

}

bool Procinfo(int pid)
{

}




void printProcess(Process* item)
{
	if (item == NULL)
		return;

	char*message = item->message;
	char*reply = item->reply;
	printf("PID: %d\nOrignal Priority: %d\nCurrent Priority: %d \n",item->pid,item->orgPriority,item->curPriority);
	if (item->messagestatus == none)
		printf("message status: None\n");
	else if(item->messagestatus == waiting_for_response)
		printf("message status: Waiting\n");
	else
		printf("message status: Need to Reply\n");

	printf("Message: %s\n",message);
	printf("reply: %s\n",reply);
	if(item->status == queued)
		printf("Status: queued\n");
	else if(item->status == running)
		printf("Status: running\n");
	else
		printf("Status: blocked\n");
}
void Totalinfo() // prints all of the processes currently in the OS
{
	printf("Total Info\n");
	printf("Running Process\n\n");
	printProcess(Running);
	prinf("Priority 0:\n");
	Node*temp = queue0->pFirstNode;
	while(temp!=NULL)
	{
		printProcess((Process*)temp->pItem);
		temp = temp->pNext;
	}
	temp = queue1->pFirstNode;
	while(temp!=NULL)
	{
		printProcess((Process*)temp->pItem);
		temp = temp->pNext;
	}
	temp = queue2->pFirstNode;
	while(temp!=NULL)
	{
		printProcess((Process*)temp->pItem);
		temp = temp->pNext;
	}


}


