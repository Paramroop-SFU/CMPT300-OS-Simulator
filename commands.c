#include "commands.h"

List*queue1,*queue0,*queue2;
bool first_time;
static int pid_valueGenerator = 1;
//Running to NULL for setup????
Process* Running;
List* blockedQueue;

//0 to 4 sem ID
Semaphore sem[5];
Process*init;
// Note since im using queue
// insert with prepend
// remove with last list_trim
// only one process can be running at a time so I will just make a Variable that that holds the running process
// Need to make a init processes, most likly best in main

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

void printQueueInfo(List *queue) {
    Node *current = queue->pFirstNode;

    if (current == NULL) {
        printf("No processes in the queue.\n");
        return;
    }

    while (current != NULL) {
        printProcess((Process*) current->pItem);
        current = current->pNext;
    }
}


bool COMPARATOR_FN(void* pItem, void* pComparisonArg)
{
	Process*temp = (Process*)pItem;
	int* pidCompare = (int*)pComparisonArg;
	if (temp->pid == *pidCompare)
		return true;

	return false;

}

//looks for the first aviable process in queue
//from highest priority to lowest
Process* getfromQueue() // helper functions
{
	Process* availProcess = NULL;
	if (List_count(queue0) > 0)
	{
		availProcess = (Process*)List_trim(queue0);
	}
	// void* value = List_trim(queue1);
	else if (List_count(queue1) > 0)
	{
		availProcess = (Process*)List_trim(queue1);
	}
	else if (List_count(queue2) > 0)
	{
		availProcess = (Process*)List_trim(queue2);
	}

	return availProcess;
}

// Proccess functions

bool Create(int priority)
{
	if (!first_time)
	{
		queue0 = List_create();
		queue1 = List_create();
		queue2 = List_create();
		blockedQueue = List_create();
		//Set it to to true, for after first time
		first_time = true;
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
		//the init process should fail
		if (Running->pid == 0)
		{
			return false;
		}

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
	if (Running->pid == pid)
	{
		Exit();
	}

// 	List_first(queue0); // make sure the queue searching from the start 
// 	void* value = List_search(queue0,COMPARATOR_FN,pid); // if not null than found
// 	if (value != NULL)
// 	{
// 		free(List_remove(queue0)); // remove and free
// 		return true; // return
// 	}
// 	List_first(queue1);
// value = List_search(queue1,COMPARATOR_FN,pid);
// 	if (value != NULL)
// 	{
// 		free(List_remove(queue1));
// 		return true;
// 	}
// 	List_first(queue2);
// 	 value = List_search(queue2,COMPARATOR_FN,pid);
// 	if (value != NULL)
// 	{
// 		free(List_remove(queue2));
// 		return true;
// 	}
// 	List_first(blockedQueue);
// 	 value = List_search(blockedQueue,COMPARATOR_FN,pid);
// 	if (value != NULL)
// 	{
// 		free(List_remove(blockedQueue));
// 		return true;
// 	}
	
// 	return false; // does not exsits
	//more concise
	bool found = false;
    List* queues[] = {queue0, queue1, queue2};
    for (int i = 0; i < 3; ++i) {
        List_first(queues[i]);
        int* value = List_search(queues[i], COMPARATOR_FN, &pid);
        if (value != NULL) {
            free(List_remove(queues[i]));
            found = true;
            break;
        }
    }
    return found;
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
		//What does this mean????
		Running = init;
		return true;
	}
	else
	{
		// Running = (List*)getfromQueue;
		Running = (Process*)getfromQueue();
		//added return bool
		return true;
	}

}

bool Quantum() // simulates a timer running out, so put it back to the queue
{
	if(Running != NULL) {
		const int i = Running->curPriority;

		List *currQueue = NULL;
		if (i==0)
		{
			// List_prepend(queue0,Running);
			currQueue = queue0;
		}
		else if(i==1)
		{
			// List_prepend(queue1,Running);
			currQueue = queue1;
		}
		else
		{
			// List_prepend(queue2,Running);
			currQueue = queue2;
		}
		// Running = NULL;
		// Running = (Process*)getfromQueue(); // send to the running
		// if (Running == NULL)
		// {
		// 	Running = init;
		// }
		//The process added to the bak of ready queue
		//and next available process ready to strat
		if(List_append(currQueue, Running) == LIST_SUCCESS) {
			Running = (Process*)getfromQueue();
			return true;
		}
	}
	return false;
}

bool Send(int pid, char * msg) // assume data already allocated
{
	if(Running != NULL) {
		Process *recv = NULL;
		List* queues[] = {queue0, queue1, queue2};
		for(int i = 0; i < 3; i++){
			//look for the process to be sent to
			List_first(queues[i]);
			recv = List_search(queues[i], COMPARATOR_FN, &pid);
			
			if(recv != NULL) {
				break;
			}
		//if pid not found
		if(recv != NULL) {
			strncpy(recv->message, msg, MAX_MESSAGE_LENGTH-1);
			recv->messagestatus = waiting_for_response;
			Running->status = blocked;
			List_append(blockedQueue, Running);

			Running = (Process*)getfromQueue();
			return true;
		}

		}
		return false;


	}
	

}

bool Receive()
{
	if(Running != NULL) {
		if (Running->messagestatus == waiting_for_response) {
			printf("Message received; %s \n", Running->message);
			Running->messagestatus = none;
			Process* send = (Process*)List_trim(blockedQueue);
			send->status = queued;
			Running = send;
			return true;
		}


	}
	return false;

}

bool Reply(int pid,char* msg)
{
	//Seach for the pid process
	Process* send = NULL;
	List_first(blockedQueue);
	send = List_search(blockedQueue, COMPARATOR_FN, &pid);
	if(send != NULL) {
		strncpy(send->reply, msg, MAX_MESSAGE_LENGTH-1);
		send->messagestatus = needs_to_reply;
		return true;

	}
	return false;

}

bool newSemaphore(int semaphore, int initial)
{
	//0 to 4 sem ID
	if(semaphore >= 0 && semaphore < 5) {
		sem[semaphore].val = initial;
		return true;
	}
	return false;

}

bool SempahoreP(int semaphore)
{//0 to 4 sem ID
	if(semaphore >= 0 && semaphore < 5) {
		//deccrement by 1
		sem[semaphore].val-- ;
		return true;
	}
	return false;

}

bool SempahoreV(int semaphore)
{
	//0 to 4 sem ID
	if(semaphore >= 0 && semaphore < 5) {
		//increment by 1
		sem[semaphore].val++ ;
		return true;
	}
	return false;



}

bool Procinfo(int pid)
{Process* process = NULL;
	//look for the pid from all the queues in List
    List* queues[] = {queue0, queue1, queue2, blockedQueue};
    for (int i = 0; i < 4; ++i) {
        List_first(queues[i]);
        process = List_search(queues[i], COMPARATOR_FN, &pid);
        if (process != NULL)
            break;
    }

    // If the process is found
    if (process != NULL) {
        printf("Process Information for PID %d:\n", process->pid);
        printf("Original Priority: %d\n", process->orgPriority);
        printf("Current Priority: %d\n", process->curPriority);
        printf("Status: ");
        switch (process->status) {
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
        if (process->messagestatus != none) {
            printf("Message Status: ");
            switch (process->messagestatus) {
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
    } else {
        printf("Process with PID %d not found.\n", pid);
        return false;
    }

	
}

void Totalinfo() {
    printf("Total Information:\n");
    printf("\nRunning Process:\n");
    if (Running != NULL) {
        printProcess(Running);
    } else {
        printf("No running process\n");
    }

    printf("\nPriority 0 (High):\n");
    printQueueInfo(queue0);

    printf("\nPriority 1 (Normal):\n");
    printQueueInfo(queue1);

    printf("\nPriority 2 (Low):\n");
    printQueueInfo(queue2);

    printf("\nBlocked Queue:\n");
    printQueueInfo(blockedQueue);
}


