#include <stdio.h>
#include <stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(void)
{
	// Variable Declarations
	int S_iArray[NUM_ROWS][NUM_COLUMNS];
	int SUM_i, SUM_j;

	// Code

	// Every Row of a 2D Array is an integer array itself comprising of 'NUM_COLUMNS' integer elements
	// There are 5 ROES and 3 COLUMNS in a 2D Integer Array. Each of the 5 Rows has 1D Array of 3 integers
	// Hence each of the 5 ROWS themselves being arrays, will be the Base Addresses of their respective Rows

	for (SUM_i = 0; SUM_i < NUM_ROWS; SUM_i++)
	{
		for (SUM_j = 0; SUM_j < NUM_COLUMNS; SUM_j++)
		{
			*(S_iArray[SUM_i] + SUM_j) = (SUM_i + 1) * (SUM_j + 1); // S_iArray can be treated as 1D Array with pointers
		}
	}
	printf("\n\n");
	printf("2D Integer Array Elements along with addresses : \n\n");
	for (SUM_i = 0; SUM_i < NUM_ROWS; SUM_i++)
	{
		for (SUM_j = 0; SUM_j < NUM_COLUMNS; SUM_j++)
		{
			printf("*(iArray[%d] + %d) = %d\t\t At Address : %p\n", SUM_i, SUM_j, *(S_iArray[SUM_i] + SUM_j), (S_iArray[SUM_i] + SUM_j));
		}
		printf("\n\n");
	}
	return(0);
}



