#include "commands.h"

int main()
{
	//sETUP
	char key;
    int priority, pid, semaphore;
  

    // Initialize OS components
    // queue0 = List_create();
    // queue1 = List_create();
    // queue2 = List_create();
    // blockedQueue = List_create();
	first_time = false;
    printf("WELCOME to the OS simulator\n");
    printf("The Way it works is you have a set of commands:\n");
    printf(" C:Create\n F:Fork\n K:Kill\n E:Exit\n Q:Quatum\n S:Send\n R:Receive\n Y:Reply\n N:New Semaphore\n P:P(Sem)\n V:V(Sem)\n I:Procinfo\n T:Totalinfo\n");
    printf("The commands are case sensitive\n");
    printf("Some Commands have additonal prompts\n");
    Init(); // set up init
    initSem();
    int cycle = 1;
    while (1) {
        
        printf("\nEnter the key keyword:\n");

        scanf(" %c", &key);
        int c; // used to clear the buffer IMPORTANT
        while ((c = getchar()) != '\n' && c != EOF);
        switch (key) {
            case 'C':
                printf("Enter priority (0 = high, 1 = norm, 2 = low): ");
                scanf("%d", &priority);
                 while ((c = getchar()) != '\n' && c != EOF);
                Create(priority);
                break;
            case 'F':
                Fork();
                break;
            case 'K':
                printf("Enter PID of process to kill: ");
                scanf("%d", &pid);
                 while ((c = getchar()) != '\n' && c != EOF);
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
                 while ((c = getchar()) != '\n' && c != EOF);
                // adding malloc for the char
                char* message = (char*)malloc(40*sizeof(char));
                printf("Enter message (max 40 characters): "); // need to use malloc
                scanf("%40[^\n]s", message);
                 while ((c = getchar()) != '\n' && c != EOF);
                Send(pid, message);
                break;
            case 'R':
                Receive();
                break;
            case 'Y':
                printf("Enter PID of process to reply to: ");
                scanf("%d", &pid);
                 while ((c = getchar()) != '\n' && c != EOF);
                 char* reply = (char*)malloc(40*sizeof(char));
                printf("Enter reply message (max 40 characters): ");
                scanf("%40[^\n]s", reply);
                 while ((c = getchar()) != '\n' && c != EOF);
                Reply(pid, reply);
                break;
            case 'N':
                printf("Enter semaphore ID (0-4): ");
                scanf("%d", &semaphore);
                 while ((c = getchar()) != '\n' && c != EOF);
                printf("Enter initial value: ");
                scanf("%d", &priority);
                 while ((c = getchar()) != '\n' && c != EOF);
                newSemaphore(semaphore, priority);
                break;
            case 'P':
                printf("Enter semaphore ID (0-4): ");
                scanf("%d", &semaphore);
                 while ((c = getchar()) != '\n' && c != EOF);
                SemaphoreP(semaphore);
                break;
            case 'V':
                printf("Enter semaphore ID (0-4): ");
                scanf("%d", &semaphore);
                 while ((c = getchar()) != '\n' && c != EOF);
                SemaphoreV(semaphore);
                break;
            case 'I':
                printf("Enter PID of process to get information: ");
                scanf("%d", &pid);
                 while ((c = getchar()) != '\n' && c != EOF);
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
        print_rec();
        cycle++;
        if (cycle %7 == 0)
        {
            starvation();
        }
    }

    return 0;
}
