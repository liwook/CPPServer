#include"util.h"
#include<stdio.h>
#include<stdlib.h>
void perror_if(bool condtion, const char* errorMessage)
{
	if (condtion) {
		perror(errorMessage);
		exit(1);
	}
}