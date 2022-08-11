//cpu.h
#ifndef __CPU_H__
#define __CPU_H__
#define my_sizeof(type) ((char *)(&type+1)-(char*)(&type))

typedef enum bool{false,true}bool;

typedef unsigned char byte;
typedef unsigned short word;
// our cpu is a 8-bit CPU has 6 registers
// register A is acumulator
// IP,BP,SP Instruction, Base, Stack Pointer are word size
// wehave only carry ->    CY
//			   Overflow -> O
//			   Sign	->     S
//			   Zero ->     Z
//			   Parity ->   P
// flags

typedef struct CPU{

	byte regs[7];		//rsrv,A,B,C,D,H,L general purpose registers

	word IP;			// Instruction pointer
	word BP;			// base pointer of current memory
	word SP;			// Stack pointer
	byte CY:1;			// carry flag
	byte O:1;			// over flow
	byte S:1;			// signed
	byte Z:1;			// zero flag
	byte LZ:1;			// less than zero
	byte GZ:1;			// greater than zero
	byte P:1;			// parity
	byte ER:1;			// error
}CPU;

// enumerate registers
enum regs{ A=0x01,B,C,D,H,L};
//supported instructions
enum instr{
			NOP=(byte)0xD0,
			MVI,MOV,MVM,
			STO,
			ADI,ADD,INC,
			SUB,SBI,DEC,
			PUSH,PUSF,
			POP,POPF,
			CMP,CMPI,
			JMP,JZ,JNZ,JLE,JGE,
			HLT
		};


// flag manipulation routines
void set_flag(char f);
void reset_flag(char f);
byte read_flag(char f);
void reset_all_flags();
void write_all_flags(byte val);

// register manipulation routines
void write_reg(char reg,byte value);
byte read_reg(char reg);

//memory manipulation routines
void write_memory_b(word location, byte value);
byte read_memory_b(word location);
void write_memory_w(word location, word value);
word read_memory_w(word location);



// stack management stack top at 0 and stack bottom at 31 total = 32 bytes
bool is_stack_empty();
bool is_stack_full();
void push(byte value);
byte pop();
void pushw(word value);
word popw();

// debug and display routines
//reset cpu
void cpu_reset();
void dump_cpu(); //prints contents of our CPU
byte fetch();	// reads a byte from program memory
void execute(char *pgm);
// utils
byte twos_comp(byte val);
void hexdump(void *ptr, int buflen);
void dump_memory(char *start, char *end);


#endif /*__CPU_H__*/
