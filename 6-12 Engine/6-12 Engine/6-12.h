#pragma once
#include <windows.h>

//指令中寄存器的位数。由TableA-2
#define win8 0
#define win16 1
#define win32 2
#define win64 3

typedef union {
	int  Immediate;
	char * Reg;
} DataUnion;

// 给联合体加一个标签，指示当前是哪种类型
typedef enum {
	DATA_IMMEDIATE,
	DATA_STRING
} DataType;

// 把标签和联合体包在一起
typedef struct {
	DataType    type;
	DataUnion   data;
} Data;


typedef struct _instruction
{
	const char *segment;	//操作指令，如mov
	const char *base;
	const char *index;
	int         scale;
	int         offset;
	Data        data;
} instruction, *Pinstruction;

void generate_addr(char *out, Pinstruction inst, char bit, char EFB);

char* ModR_MandSIB(char* Opcodepoint, char bit, BOOL EFB);

char* OneByteOpcode(char* Entrypoint, char Prefix);

void DisassemEngine(char*opcode, int len);

