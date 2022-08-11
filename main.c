#include <stdio.h>
#include "cpu.h"
//test.c

// in this unit we have to make an 
// array named pgm of type char

// we will try to execute it in our hypothetical cpu defined above
/*char pgm[]={
			MVI, A,5,
			MVI, B,3,
			ADD, A,B,
			ADI, A,2,
			STO, 10,A,
			MVM, C,10,
			SUB, C,B,
			SBI, C,12,
			ADI, A,1,
			CMP, C,
			JZ, 23,
			HLT,
			'\0'
		};
*/
/* while loop 0 to 10 times
char pgm[]={
	MVI,C,0,
	CMPI,C,10,
	JZ,12,
	INC,C,
	JLE,3,
	HLT,
	'\0'

};
*/
/* while loop 0 to 10 times*/
char pgm[]={
	MVI,C,10,
	CMPI,C,0,
	JZ,12,
	DEC,C,
	JGE,3,
	HLT,
	'\0'

};
/**/
/*
char pgm[]={
	MVI,A,10,
	MVI,C,0,
	INC,C,
	CMP,C,
	HLT,
	'\0'
};
*/
/*char pgm[]={
	MVI, A,5,
	MVI, B,5,
	CMP,B,
	PUSF,
	POP,
	MVI,A,0x0F,
	PUSH, A,
	POPF,
	HLT,
	'\0'
};*/
int main(int argc, char **argv)
{
	// reset cpu
	cpu_reset();

	printf("CPU Reset Successfull\n");
	printf("Executing => ");
	hexdump(pgm,16);
	printf("\n\n");
	execute(pgm);

	return 0;
}
