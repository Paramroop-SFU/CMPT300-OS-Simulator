#include "commands.h"

int main()
{
	// for first time make init and add it to the Running
	init = (Process*)malloc(sizeof(Process));
	init->pid = 0;
	Running = init;

	while (1)
	{
		char option;
		int c;
		printf("Please choose a Command\n");
		scanf("%c", &option);

		if (option == 'C')
		{
			printf("C\n");
		}
		else if (option == 'F')
		{
			printf("F\n");
		}
		else if (option == 'K')
		{
			printf("K\n");
		}
		else if (option == 'E')
		{
			printf("E\n");
		}
		else if (option == 'Q')
		{
			printf("Q\n");
		}
		else if (option == 'S')
		{
			printf("S\n");
		}
		else if (option == 'R')
		{
			printf("R\n");
		}
		else if (option == 'Y')
		{
			printf("Y\n");
		}
		else if (option == 'N')
		{
			printf("N\n");
		}
		else if (option == 'P')
		{
			printf("P\n");
		}
		else if (option == 'V')
		{
			printf("V\n");
		}
		else if (option == 'I')
		{
			printf("I\n");
		}
		else if (option == 'T')
		{
			printf("T\n");
		}
		else
		{
			printf("Command does not exsits");
		}
		while ((c = getchar()) != '\n' && c != EOF)
			;
	}
}
