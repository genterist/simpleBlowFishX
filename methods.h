/*
 * methods.h
 *
 *  Created on: Jul 12, 2016
 *      Author: genterist-a
 */

#ifndef METHODS_H_
#define METHODS_H_

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/*******************
 * Include pre-generated arrays of
 * parray[18]  /  sbox0[256] to sbox3[256]
 */
#include "./blowfish_consts.c"

/*Function prototypes*/

int debugMode = 1;
unsigned long temp = 0;

int readContent (unsigned long *theHex, char thePath[100]);
void hex2intArray(int i[4], int hex);
void divideKey(unsigned long key, unsigned long *l, unsigned long *r);

/*Encryption related function prototypes*/
int blowPrep(unsigned long *K1, unsigned long *K2);
int blowPrep_Encrypt(unsigned long *L, unsigned long *R);
int blow (char theFilePath[100], char theKeyPath[16], int debug);
unsigned long F(unsigned long theBlock);


/*
 * IMPLEMENNTATION OF FUNCTIONS
 * */
int blowPrep(unsigned long *K1, unsigned long *K2){
	int count = 0;

	fprintf (stderr, "\n\n Enter Key Gen\n K1: %x  K2: %x \n", *K1, *K2);
	for (count = 0; count <18; count++){
		if (count%2 == 0) parray[count] = parray[count]^*K1;
		else parray[count] = parray[count]^*K2;

		if (debugMode == 1) fprintf(stderr,"P[%i]= %x     ", count, parray[count]);
		if (count%3 == 0) fprintf(stderr,"\n");
	}

	unsigned long blowPrep_temp1=0;
	unsigned long blowPrep_temp2=0;

	/*ITERATE P KEY*/
	for (count = 0; count<18; count=count+2){
		blowPrep_Encrypt(&blowPrep_temp1, &blowPrep_temp2);
		parray[count] = blowPrep_temp1;
		parray[count+1] = blowPrep_temp2;
	}

	fprintf(stderr,"\n");

	/*ITERATE sBOX0 KEY*/
	blowPrep_temp1=0;
	blowPrep_temp2=0;
	for (count = 0; count<256; count=count+2){
		blowPrep_Encrypt(&blowPrep_temp1, &blowPrep_temp2);
		sbox0[count] = blowPrep_temp1;
		sbox0[count+1] = blowPrep_temp2;
	}

	fprintf(stderr,"\n");

	/*ITERATE sBOX1 KEY*/
	blowPrep_temp1=0;
	blowPrep_temp2=0;
	for (count = 0; count<256; count=count+2){
		blowPrep_Encrypt(&blowPrep_temp1, &blowPrep_temp2);
		sbox1[count] = blowPrep_temp1;
		sbox1[count+1] = blowPrep_temp2;
	}
	fprintf(stderr,"\n");

	/*ITERATE sBOX2 KEY*/
	blowPrep_temp1=0;
	blowPrep_temp2=0;
	for (count = 0; count<256; count=count+2){
		blowPrep_Encrypt(&blowPrep_temp1, &blowPrep_temp2);
		sbox2[count] = blowPrep_temp1;
		sbox2[count+1] = blowPrep_temp2;
	}

	fprintf(stderr,"\n");

	/*ITERATE sBOX3 KEY*/
	blowPrep_temp1=0;
	blowPrep_temp2=0;
	for (count = 0; count<256; count=count+2){
		blowPrep_Encrypt(&blowPrep_temp1, &blowPrep_temp2);
		sbox3[count] = blowPrep_temp1;
		sbox3[count+1] = blowPrep_temp2;
	}

	fprintf(stderr,"\n");

	for (count = 0; count <18; count++){
		fprintf(stderr,"P[%i]= %x     ", count, parray[count]);
		if (count%3 == 0) fprintf(stderr,"\n");
	}
	for (count = 0; count <256; count++){
		fprintf(stderr,"sbox0[%i]= %x     ", count, sbox0[count]);
		if (count%3 == 0) fprintf(stderr,"\n");
	}
	for (count = 0; count <256; count++){
		fprintf(stderr,"sbox1[%i]= %x     ", count, sbox1[count]);
		if (count%3 == 0) fprintf(stderr,"\n");
	}
	for (count = 0; count <256; count++){
		fprintf(stderr,"sbox2[%i]= %x     ", count, sbox2[count]);
		if (count%3 == 0) fprintf(stderr,"\n");
	}
	for (count = 0; count <256; count++){
		fprintf(stderr,"sbox3[%i]= %x     ", count, sbox3[count]);
		if (count%3 == 0) fprintf(stderr,"\n");
	}


	return 1;
}

int blowPrep_Encrypt(unsigned long *L, unsigned long *R){

	unsigned long this_message_Left = *L;
	unsigned long this_message_Right = *R;
	unsigned long this_message_Left_NextRound = 0;
	unsigned long this_message_Right_NextRound = 0;
	int count = 0;

	for (count = 0; count <16; count++) {
		if (count!=0){
			this_message_Right=this_message_Right_NextRound;
			this_message_Left=this_message_Left_NextRound;
		}
		if (debugMode == 2) fprintf(stderr,"\n count %i- p: %x",count, parray[count]);
		this_message_Right_NextRound = this_message_Left^parray[count];
		this_message_Left_NextRound = F(this_message_Right_NextRound)^this_message_Right;
		if (debugMode == 2) fprintf(stderr,"\n     %x-%x >> %x-%x", this_message_Left, this_message_Right,this_message_Left_NextRound, this_message_Right_NextRound);

	}
	this_message_Right_NextRound = this_message_Left^parray[16];
	this_message_Left_NextRound = this_message_Right^parray[17];

	this_message_Right=this_message_Right_NextRound;
	this_message_Left=this_message_Left_NextRound;

	*L = this_message_Left;
	*R = this_message_Right;

	return 1;

}
int blow (char theMessagePath[100], char theKeyPath[100], int debug){

	unsigned long  theKey = 0;
	unsigned long  theMessage = 0;
	unsigned long key_Left = 0;
	unsigned long key_Right = 0;
	unsigned long message_Left = 0;
	unsigned long message_Right = 0;
	unsigned long message_Left_NextRound = 0;
	unsigned long message_Right_NextRound = 0;
	unsigned long current_key = 0;
	int count = 0;

	debugMode = debug;

	readContent (&theMessage, theMessagePath);
	/* SPLIT THE MESSAGE TO LEFT AND RIGHT MESSAGES*/
	divideKey(theMessage,&message_Left, &message_Right);


	readContent (&theKey, theKeyPath);
	/* SPLIT THE KEY TO LEFT AND RIGHT KEY */
	divideKey(theKey,&key_Left, &key_Right);
	blowPrep (&key_Left,&key_Right);

	/*
	 * NOW ENTERING THE MAIN ENCRYPTION ROUTINE
	 * */
	fprintf(stderr,"\n------------------\nEnter encryption routine\n");
	for (count = 0; count <16; count++) {
		if (count!=0){
			message_Right=message_Right_NextRound;
			message_Left=message_Left_NextRound;
		}
		if (debugMode == 1) {
			fprintf(stderr,"MESSAGE: %x - %x\n", message_Left, message_Right);
		}
		message_Right_NextRound = message_Left^parray[count];
		message_Left_NextRound = F(message_Right_NextRound)^message_Right;


	}
	message_Right_NextRound = message_Left^parray[16];
	message_Left_NextRound = message_Right^parray[17];
	message_Right=message_Right_NextRound;
	message_Left=message_Left_NextRound;

	fprintf(stderr,"\nThe final encrypted message. \n%x%x", message_Left, message_Right);

	fprintf(stderr,"\n------------------------------------\n");
	/*If successful, return 1*/
	return 1;
}

int getCurrentKey ( int roundKey[4], int K[4], int round){
	int count = 0;

	hex2intArray(roundKey, parray[round]);
	for (count = 0; count <4; count++){
		roundKey[count] = roundKey[count]^K[count];
	}
	return 1;
}

unsigned long F(unsigned long theBlock) {
	int block[4];
	unsigned long thisTemp = 0;
	int count = 0;

	hex2intArray(block, theBlock);
	if (debugMode == 2)
		for (count =0; count <4; count++){
			fprintf(stderr," [%d]", block[count]);
		}

	thisTemp = ( ((sbox0[block[0]]+sbox1[block[1]]) ^ sbox2[block[2]]) + sbox3[block[3]] );

	if (debugMode == 2) {
		fprintf(stderr,"    F(X) = %x \n", thisTemp);
	}

	return thisTemp;
}

void hex2intArray(int i[4], int hex){
	i[3] = hex & 0xff;
	i[2] = (hex>>8) & 0xff;
	i[1] = (hex>>16) & 0xff;
	i[0] = (hex>>24) & 0xff;
}

/*For now, only extract 8 integers (16 hex characters) from the content file*/
int readContent (unsigned long *theHex, char thePath[100]) {

	    char *file_contents;
	    long input_file_size;
	    FILE *input_file;
	    if ((input_file = fopen(thePath, "rb")) == NULL){
	    	printf("\n Failed toNULL open the file\n");
	    	return 0;
	    	//file_contents = thePath;
	    } else
	    {
	    	fseek(input_file, 0, SEEK_END);
	    	input_file_size = ftell(input_file);
	    	rewind(input_file);
	    	file_contents = malloc(input_file_size * (sizeof(char)));
	    	fread(file_contents, sizeof(char), input_file_size, input_file);
	    }
	    fclose(input_file);
	    /*Text content is now in array file_content*/

	    *theHex = strtoul(file_contents, NULL, 16);


	    if (debugMode == 1 || debugMode == 2) {
	    	fprintf(stderr,"\nRead %s >> ", thePath);
	    }

	    return 1;
}

void divideKey( unsigned long key,  unsigned long *l,  unsigned long *r) {
	*r = key & 0xffffffff;
	*l = (key>>32)&0xffffffff;
	fprintf(stderr," %x-%x\n",*l,*r);
}

#endif /* METHODS_H_ */
