//cpu.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cpu.h"

// Global define our CPU here
CPU cpu;
// our memory capacity is very low only 1kb
byte memory[1024];		// 1024kb
const word stack_bottom =1023;
word stack_top =stack_bottom-32*8;  // upper 32 bytes of memory is stack

// start of executable code
word code_start = 256; // our code will be loaded at 256byte offset in memory

// flag manipulation routines
void set_flag(char f)
{
	switch(f)
	{
		case 'C':
				cpu.CY = 1;
				break;
		case 'O':
				cpu.O = 1;
				break;
		case 'S':
				cpu.S = 1;
				break;
		case 'Z':
				cpu.Z = 1;
				break;
		case 'P':
				cpu.P = 1;
				break;
		case 'L':
				cpu.LZ = 1;
				break;
		case 'G':
				cpu.GZ =1;
				break;
		case 'E':
				cpu.ER =1;
				break;
		default:
			fprintf(stderr,"Invalid flag to set\n");
			exit(1);
	}
}
void reset_flag(char f)
{
	switch(f)
	{
		case 'C':
				cpu.CY = 0;
				break;
		case 'O':
				cpu.O = 0;
				break;
		case 'S':
				cpu.S = 0;
				break;
		case 'Z':
				cpu.Z = 0;
				break;
		case 'P':
				cpu.P = 0;
				break;
		case 'L':
				cpu.LZ = 0;
				break;
		case 'G':
				cpu.GZ = 0;
				break;
		case 'E': cpu.ER =0;
				break;
		default:
			fprintf(stderr,"Invalid flag to reset\n");
			exit(1);
	}
}
byte read_flag(char f)
{
	switch(f)
	{
		case 'C': return(cpu.CY);
		case 'O': return(cpu.O);
		case 'S': return(cpu.S);
		case 'Z': return(cpu.Z);
		case 'P': return(cpu.P);
		case 'L': return(cpu.LZ);
		case 'G': return(cpu.GZ);
		case 'E': return(cpu.ER);
		default:
			fprintf(stderr,"Invalid flag to read\n");
			exit(1);
	}
}
void reset_all_flags()
{
	cpu.CY = 0;
	cpu.O = 0;
	cpu.S = 0;
	cpu.Z = 0;
	cpu.LZ = 0;
	cpu.GZ = 0;
	cpu.P = 0;
	cpu.ER = 0;
}
byte read_all_flags()
{
	byte res;
	res = cpu.CY<<7|cpu.O<<6|cpu.S<<5|cpu.Z<<4|cpu.LZ<<3|cpu.GZ<<2|cpu.P<<1|cpu.ER<<0;
	return(res);
}
void write_all_flags(byte val)
{
	cpu.CY=(0<(val & 0x01<<7)?1:0);
	cpu.O =(0<(val & 0x01<<6)?1:0);
	cpu.S =(0<(val & 0x01<<5)?1:0);
	cpu.Z =(0<(val & 0x01<<4)?1:0);
	cpu.LZ=(0<(val & 0x01<<3)?1:0);
	cpu.GZ=(0<(val & 0x01<<2)?1:0);
	cpu.P =(0<(val & 0x01<<1)?1:0);
	cpu.ER=(0<(val & 0x01<<0)?1:0);
}
// register manipulation routines
void write_reg(char reg,byte value)
{
	switch(reg)
	{
		case 'A': cpu.regs[1] = value;
					break;
		case 'B': cpu.regs[2] = value;
					break;
		case 'C': cpu.regs[3] = value;
					break;
		case 'D': cpu.regs[4] = value;
					break;
		case 'H': cpu.regs[5] = value;
					break;
		case 'L': cpu.regs[6] = value;
					break;
		default:
			fprintf(stderr,"Invalid regiter to write\n");
			exit(1);
	}
}
byte read_reg(char reg)
{
	switch(reg)
	{
		case 'A': return(cpu.regs[1]);

		case 'B': return(cpu.regs[2]);

		case 'C': return(cpu.regs[3]);

		case 'D': return(cpu.regs[4]);

		case 'H': return(cpu.regs[5]);

		case 'L': return(cpu.regs[6]);
		default:
			fprintf(stderr,"Invalid regiter to read\n");
			exit(1);
	}
	return 0;
}

//memory manipulation routines
void write_memory_b(word location, byte value)
{
	if( location < 0 || location>1023)
	{
		fprintf(stderr,"Writing outside memory\n");
		exit(1);
	}
	memory[location]= value;
}
byte read_memory_b(word location)
{
	if( location < 0 || location>1023)
	{
		fprintf(stderr,"Writing outside memory\n");
		exit(1);
	}
	return(memory[location]);
}

void write_memory_w(word location, word value)
{
	if( location < 0 || location>1022)
	{
		fprintf(stderr,"Writing outside memory\n");
		exit(1);
	}
	memory[location] = (byte)(value>>8);
	memory[++location] = (byte)(value&0x00ff);
}
word read_memory_w(word location)
{
	word val=0;
	if( location < 0 || location>1022)
	{
		fprintf(stderr,"Writing outside memory\n");
		exit(1);
	}
	val = read_memory_b(location);
	val = val<<8;
	val += read_memory_b(++location);
	return(val);
}
//reset cpu
void cpu_reset()
{
	cpu.SP = stack_top;
	write_reg('A',0);
	write_reg('B',0);
	write_reg('C',0);
	write_reg('D',0);
	write_reg('H',0);
	write_reg('L',0);
	cpu.IP = code_start;
	cpu.BP = 0;
	reset_flag('C');
	reset_flag('O');
	reset_flag('S');
	reset_flag('P');
	set_flag('Z');

}
// stack management stack top at 0 and stack bottom at 31 total = 32 bytes
bool is_stack_empty()
{
	if(cpu.SP > 1023)
		return true;
	else
		return false;
}
bool is_stack_full()
{
	if(cpu.SP < stack_top)
		return true;
	else
		return false;
}
void push(byte value)
{
	//push a value to stack if stack is not full
	if(is_stack_full())
	{
		fprintf(stderr,"Stack is full can not push\n");
		return;
	}
	else
		write_memory_b(++cpu.SP,value);
}

byte pop()
{
	if(is_stack_empty())
	{
		fprintf(stderr,"Stack is empty can not pop\n");
		return -1;
	}
	else
		return(read_memory_b(cpu.SP--));
}
void pushw(word value)
{
	byte hi,lo;
	hi = (byte)(value>>8);
	lo = (byte)(value &0x00ff);
	//printf("input = %0X hi = %0X lo = %0X\n",value,hi,lo);
	push(hi);
	push(lo);
}
word popw()
{
	byte hi,lo;
	lo = pop();
	hi = pop();
	word val = (hi<<8)+lo;
	return(val);
}
// utils
byte twos_comp(byte val)
{
	return(~val+1);
}

// debug and display routines
void dump_cpu()
{
	printf("==========================================================\n");
	printf(" A = 0x%X \n B = 0x%X C = 0x%X D = 0x%X \n H = 0x%X L = 0x%X \n\n", \
			cpu.regs[1],cpu.regs[2],cpu.regs[3],cpu.regs[4],cpu.regs[5],cpu.regs[6]);
	printf(" IP = 0x%X BP = 0x%X SP = 0x%X\n",cpu.IP,cpu.BP,cpu.SP);
	printf(" Flags CY = %d O = %d S = %d Z = %d LZ = %d GZ = %d P = %d ER = %d\n", \
			cpu.CY,cpu.O, cpu.S, cpu.Z, cpu.LZ, cpu.GZ, cpu.P, cpu.ER);
	printf("==========================================================\n");

}
void hexdump(void *ptr, int buflen) 
{
	unsigned char *buf = (unsigned char*)ptr;
	int i, j;
	for (i=0; i<buflen; i+=16) 
	{
		printf("%06x: ", i);
		for (j=0; j<16; j++){
			if (i+j < buflen)
				printf("%02x ", buf[i+j]);
			else
				printf("   ");
		}
		printf(" ");
		
		for (j=0; j<16; j++){
			if (i+j < buflen)
				printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
		}
		printf("\n");
		
	}
}
void dump_memory(char *start, char *end)
{

}
byte fetch()
{
	//static int ip;
	byte val = memory[cpu.IP++];	// get byte advance ip
	//byte val = pgm[ip++];
	//printf("0x%X\n",val);
	return val;
}
void execute(char *pgm)
{
	byte reg1,reg2,val;
	word m_loc;
	word res;
	int size = sizeof(pgm)* 2;
	//int size = *(&pgm + 1) - pgm; //hack from https://www.geeksforgeeks.org/how-to-find-size-of-array-in-cc-without-using-sizeof-operator/
	//printf("%d %d\n",*(&pgm+1),pgm);
	//printf("%d \n",size);
	memcpy(memory+ code_start, pgm,size*sizeof(char));
	unsigned int  mnem = fetch();
	while(1){
		if(mnem == HLT)
		{
			printf("Got HLT\n");
			break;
		}else if(mnem== NOP){
					printf("Got Nop\n");
		}else if(mnem==MVI){
					printf("Got MVI\n");
					reg1 = fetch();// register
					val = fetch(); // value
					cpu.regs[reg1] = val;
					reset_all_flags();
					if(cpu.regs[reg1]==0)
						set_flag('Z');
					dump_cpu();
		}else if(mnem==MOV){
					printf("Got MOV\n");
					reg1 = fetch(); //register1
					reg2 = fetch(); // register2
					cpu.regs[reg1] = cpu.regs[reg2];
					reset_all_flags();
					if(cpu.regs[reg1]==0)
						set_flag('Z');
					dump_cpu();
		}else if(mnem== MVM){
					printf("Got MVM\n");
					reg1 = fetch();// register
					m_loc=fetch(); // memory location
					cpu.regs[reg1]=memory[m_loc];
					reset_all_flags();
					if(cpu.regs[reg1]==0)
						set_flag('Z');
					dump_cpu();
		}else if(mnem == STO){
					printf("Got STO\n");
					m_loc=fetch();//memorylocation
					reg1 = fetch(); //register
					memory[m_loc] = cpu.regs[reg1];
					dump_cpu();
		}else if(mnem == ADI){
					printf("Got ADI\n");
					reg1 = fetch();
					val = fetch();// value
					res = cpu.regs[reg1];
					res+=val;
					cpu.regs[reg1] = res;
					reset_all_flags();
					if(res>0xff)
						set_flag('C');
					if(cpu.regs[reg1]&(0x01<<7))
						set_flag('S');
					if(cpu.regs[reg1]==0)
						set_flag('Z');
					dump_cpu();
		}else if(mnem == ADD){
					printf("Got ADD\n");
					reg1 =fetch();//register
					reg2 = fetch();
					res = cpu.regs[reg1];
					res+= cpu.regs[reg2];
					cpu.regs[reg1] = res;
					reset_all_flags();
					if(res>0xff)
						set_flag('C');
					if(cpu.regs[reg1]&(0x01<<7))
						set_flag('S');
					if(cpu.regs[reg1]==0)
						set_flag('Z');
					dump_cpu();
		}else if(mnem == INC){
					printf("Got INC\n");
					reg1 =fetch();//register
					res = cpu.regs[reg1];
					res += 1;
					cpu.regs[reg1] = res;
					reset_all_flags();
					if(res>0xff)
						set_flag('C');
					if(cpu.regs[reg1]&(0x01<<7))
						set_flag('S');
					if(cpu.regs[reg1]==0)
						set_flag('Z');
					dump_cpu();
		}else if(mnem == SUB){
					printf("Got SUB\n");
					reg1 =fetch();//register
					reg2 = fetch();
					cpu.regs[reg1] += twos_comp(cpu.regs[reg2]);
					reset_all_flags();
					if(cpu.regs[reg1]&(0x01<<7))
						set_flag('S');
					if(cpu.regs[reg1]==0)
						set_flag('Z');
					dump_cpu();
		}else if(mnem == SBI){
					printf("Got SBI\n");
					reg1 = fetch();
					val = fetch();// value
					reset_all_flags();
					cpu.regs[reg1] += twos_comp(val);
					if(cpu.regs[reg1]&(0x01<<7))
						set_flag('S');
					if(cpu.regs[reg1]==0)
						set_flag('Z');
					dump_cpu();
		}else if(mnem == DEC){
					printf("Got DEC\n");
					reg1 = fetch();
					cpu.regs[reg1] -= 1;
					reset_all_flags();
					if(cpu.regs[reg1]&(0x01<<7))
						set_flag('S');
					if(cpu.regs[reg1]==0)
						set_flag('Z');
					dump_cpu();
		}else if(mnem == CMP){
					printf("Got CMP\n");
					// compare reg1 with reg A
					// and set appropriate flag
					reg1 = fetch();// reg1
					reset_all_flags();
					if(cpu.regs[reg1]==cpu.regs[1])
						set_flag('Z');
					if(cpu.regs[reg1]<cpu.regs[1])
						set_flag('L');
					if(cpu.regs[reg1]>cpu.regs[1])
						set_flag('G');

					dump_cpu();
		}else if(mnem == CMPI){
					printf("Got CMPI\n");
					// compare reg1 with reg A
					// and set appropriate flag
					reg1 = fetch();
					val = fetch();
					reset_all_flags();
					if(cpu.regs[reg1] == val)
						set_flag('Z');
					if(cpu.regs[reg1] < val)
						set_flag('L');
					if(cpu.regs[reg1] > val)
						set_flag('G');
					dump_cpu();
		}else if(mnem == JMP){
					printf("Got JMP\n");
					// fetch the offset to jump to.
					//For simplicity the offset is from begining of
					// the address whre the code is loaded i.e code_start
					// and always +ve
					val = fetch();// value
					cpu.IP = code_start+val;

					dump_cpu();
		}else if(mnem == JZ){
					printf("Got JZ\n");
					// fetch the offset to jump to.
					//For simplicity the offset is from begining of
					// the address whre the code is loaded i.e code_start
					// and always +ve
					// read the Zero flag if set then jump
					val = fetch();// jump location offset
					if(read_flag('Z'))
					{
						cpu.IP = code_start+val;
					}
					dump_cpu();
		}else if(mnem == JNZ){
					printf("Got JNZ\n");
					// fetch the offset to jump to.
					//For simplicity the offset is from begining of
					// the address whre the code is loaded i.e code_start
					// and always +ve
					// read the Zero flag if set then jump
					val = fetch();// jump location offset
					if(read_flag('Z')!=1)
					{
						cpu.IP = code_start+val;
					}
					dump_cpu();
		}else if(mnem == JLE){
					printf("Got JLE\n");
					// fetch the offset to jump to.
					//For simplicity the offset is from begining of
					// the address whre the code is loaded i.e code_start
					// and always +ve
					// read the Zero flag if set then jump
					val = fetch();// jump location offset
					if(read_flag('Z')!=1 || read_flag('L')!=1)
					{
						cpu.IP = code_start+val;
					}
					dump_cpu();
		}else if(mnem == JGE){
					printf("Got JGE\n");
					// fetch the offset to jump to.
					//For simplicity the offset is from begining of
					// the address whre the code is loaded i.e code_start
					// and always +ve
					// read the Zero flag if set then jump
					val = fetch();// jump location offset
					if(read_flag('Z')!=1 || read_flag('G')!=1)
					{
						cpu.IP = code_start+val;
					}
					dump_cpu();
		}else if(mnem == PUSH){
					printf("Got PUSH\n");
					//push the register to stack
					reg1 = fetch();
					push(cpu.regs[reg1]);
					dump_cpu();
		}else if(mnem == POP){
					printf("Got POP\n");
					//push the register to stack

					cpu.regs[1]=pop();
					dump_cpu();
		}else if(mnem == PUSF){
					printf("Got PUSF\n");
					//push the flags to stack

					push(read_all_flags());
					dump_cpu();
		}else if(mnem == POPF){
					printf("Got POPF\n");
					//pop the flags from stack

					write_all_flags(pop());
					dump_cpu();
		}else {
					printf("Got Invalid\n");
					break;
		}
		mnem=fetch();
	}
}

