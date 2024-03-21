#include "commands.h"

int main()
{
	//sETUP
	char key;
    int priority, pid, semaphore;
    char message[MAX_MESSAGE_LENGTH];

    // Initialize OS components
    // queue0 = List_create();
    // queue1 = List_create();
    // queue2 = List_create();
    // blockedQueue = List_create();
	first_time = false;
   
    Init(); // set up init
    while (1) {
        printf("Enter the key keyword:\n");

        scanf(" %c", &key);
        int c; // used to clear the buffer IMPORTANT
        while ((c = getchar()) != '\n' && c != EOF);
        switch (key) {
            case 'C':
                printf("Enter priority (0 = high, 1 = norm, 2 = low): ");
                scanf("%d", &priority);
                Create(priority);
                break;
            case 'F':
                Fork();
                break;
            case 'K':
                printf("Enter PID of process to kill: ");
                scanf("%d", &pid);
                Kill(pid);
                break;
            case 'E':
                Exit();
                break;
            case 'Q':
                Quantum();
                break;
            case 'S':
                printf("Enter PID of receiver: ");
                scanf("%d", &pid);
                printf("Enter message (max 40 characters): ");
                scanf("%s", message);
                Send(pid, message);
                break;
            case 'R':
                Receive();
                break;
            case 'Y':
                printf("Enter PID of process to reply to: ");
                scanf("%d", &pid);
                printf("Enter reply message (max 40 characters): ");
                scanf("%s", message);
                Reply(pid, message);
                break;
            case 'N':
                printf("Enter semaphore ID (0-4): ");
                scanf("%d", &semaphore);
                printf("Enter initial value: ");
                scanf("%d", &priority);
                newSemaphore(semaphore, priority);
                break;
            case 'P':
                printf("Enter semaphore ID (0-4): ");
                scanf("%d", &semaphore);
                SemaphoreP(semaphore);
                break;
            case 'V':
                printf("Enter semaphore ID (0-4): ");
                scanf("%d", &semaphore);
                SemaphoreV(semaphore);
                break;
            case 'I':
                printf("Enter PID of process to get information: ");
                scanf("%d", &pid);
                Procinfo(pid);
                break;
            case 'T':
                Totalinfo();
                break;
            case 'X':
                printf("Exiting simulation.\n");
                return 0;
            default:
                printf("Invalid key. Please try again.\n");
        }
    }

    return 0;
}
