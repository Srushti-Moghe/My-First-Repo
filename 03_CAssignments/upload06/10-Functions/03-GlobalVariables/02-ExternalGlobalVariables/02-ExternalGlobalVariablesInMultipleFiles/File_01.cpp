//*** GLOBAL SCOPE ***

//global_count is a global variable declared in source code file ExternalGlobalVariablesInMultipleFiles.cpp

//To access it in this file, it must be first be re-declared as an external variable in the global scope of this file along with the 'extern' keyword and its proper data type

//Then, it can be used as any ordinary global variable throughout this file as well

#include <stdio.h> 


extern int global_count;

void change_count_one(void)
{
	//Code
	global_count = global_count + 1;
	printf("change_count_one() : Value of global_count in File_01 = %d\n", global_count);
}

