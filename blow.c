/*
 ============================================================================
 Name        : blow.c
 Author      : TamNguyen
 Version     :
 Copyright   : (CC)
 Description : 
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "methods.h"


int main(int argc, char *argv[]) {

	if( argc == 3 ) {

	  	int debugCode = 1;

	  	//blow (contentPath, keyPath, debugCode);
	  	blow (argv[1], argv[2],debugCode);



	   }
	   else if( argc > 2 ) {
	      printf("Too many arguments supplied.\n");
	   }
	   else {
	      printf("Command: blow (content path) (key path) [debug code] \n");
	      printf("such as: blow content.txt key txt 1 \n");
	   }

	return EXIT_SUCCESS;
}
