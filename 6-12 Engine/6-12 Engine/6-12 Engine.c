#include <windows.h>
#include <stdio.h>
#include "tools.h"
#include "6-12.h"

//单行汇编指令长度
int CodeLen;
//判断是否是前缀嵌套调用
BOOL IsPrefixNest = 0;
//寄存器索引
char* regSeg[8] = { "ES","CS","SS","DS","FS","GS" };
char* reg8[8] = { "AL","CL","DL","BL","AH","CH","DH","BH" };
char* reg16[8] = { "AX","CX","DX","BX","SP","BP","SI","DI" };
char* reg32[8] = { "EAX","ECX","EDX","EBX","ESP","EBP","ESI","EDI" };
char* reg64[16] = { "RAX","RCX","RDX","RBX","RSP","RBP","RSI","RDI", "R8","R9","R10","R11","R12","R13","R14","R15" };

char* DataLen[4] = { "BYTE","WORD","DWORD","QWORD" };

//TableA6
char* Group1[8] = { "ADD","OR","ADC","SBB", "AND","SUB","XOR","CMP" };
char* Group1A[8] = { "POP" };
char* Group2[8] = { "ROL","ROR","RCL","RCR", "SHL","SHR"," ","SAR" };
char* Group3[8] = { "TEST"," ","NOT","NEG", "MUL","IMUL","DIV","IDIV" };
char* Group4[8] = { "INC","DEC" };
char* Group5[8] = { "INC","DEC","CALLN","CALLF","JMPN","JMPF","JMPF","PUSH" };




//解析ModR_M字节
char Get_R_M(char ModR_M)
{
	return ModR_M & 0x7;
};
char Get_Reg_Opcode(char ModR_M)
{
	return (ModR_M >> 3) & 0x7;
};
char Get_Mod(char ModR_M)
{
	return (ModR_M >> 6) & 0x3;
};

//解析SIB字节
char Get_Base(char SIB)
{
	return SIB & 7;
};
char Get_Index(char SIB)
{
	return (SIB >> 3) & 7;
};
char Get_Scale(char SIB)
{
	return (SIB >> 6) & 7;
};

//段前缀函数
char* GetSegmentforPrefix(int Prefix)
{
	char*segment = NULL;
	switch (Prefix)
	{
	case 0x26:
		segment = "ES";
		break;
	case 0x2E:
		segment = "CS";
		break;
	case 0x36:
		segment = "SS";
		break;
	case 0x3E:
		segment = "DS";
		break;
	case 0x64:
		segment = "FS";
		break;
	case 0x65:
		segment = "GS";
		break;
	}
	return segment;
};



/*
out:汇编输出地址
inst:opcode传入的结构体指针
bit:进程位数，16，32，64
EFB:Ev,Gv还是Gv,Ev。也就是内存项是不是在前面
*/
void generate_addr(char *out, Pinstruction inst, char Reg_Opcodebit, char EFB)
{
	char addr[128] = { 0 };

	//标识第一次添加字符串
	int first = 1;
	//添加base
	if (inst->base)
	{
		snprintf(addr + strlen(addr), sizeof(addr) - strlen(addr), "%s", inst->base);
		first = 0;
	}
	//添加index
	if (inst->index)
	{
		snprintf(addr + strlen(addr), sizeof(addr) - strlen(addr), "%s%s*%d", first ? "" : "+", inst->index, inst->scale);
		first = 0;
	}
	//添加offset
	if (inst->offset != 0 || first)
	{
		snprintf(addr + strlen(addr), sizeof(addr) - strlen(addr), "%s0x%X", first ? "" : "+", inst->offset);
	}

	if (inst->data.type == DATA_IMMEDIATE)
	{

		snprintf(out, 128, "%s PTR %s:[%s],%X", DataLen[Reg_Opcodebit], inst->segment, addr, inst->data.data.Immediate);
	}
	else if (inst->data.type == DATA_STRING)
	{
		if (EFB == 1)
		{
			snprintf(out, 128, "%s PTR %s:[%s],%s", DataLen[Reg_Opcodebit], inst->segment, addr, inst->data.data.Reg);
		}
		else
		{
			snprintf(out, 128, "%s,%s PTR %s:[%s]", inst->data.data.Reg, DataLen[Reg_Opcodebit], inst->segment, addr);
		};

	}

}



/*
Opcodepoint:下行指令的首地址指针
Prefix:前缀
Reg_Opcodebit:进程位数，16，32，64
EFB:Ev,Gv还是Gv,Ev。也就是内存项是不是在前面
*/
char* ModR_MandSIB(char* Opcodepoint, char Prefix, char Reg_Opcodebit, char EFB)
{
	char ModR_M;
	char SIB;

	char *buf;
	char R_M;
	char Reg_Opcode;
	char Mod;

	char Base_index;
	char Index_index;
	char Scale_index;

	char*Base;
	char*Index;
	char Scale;


	int dis = 0;				//偏移
	char* reg = NULL;				//汇编中的Gv寄存器
	char* reg_rm = NULL;		//汇编中的rm=3的寄存器
	char InstructionLen = 0;	//指令长度
	char* segment = "DS";		//默认数据段

	instruction inst = { 0 };

	//解析前缀
	switch (Prefix)
	{
	case 0x26:
		segment = "ES";
		break;
	case 0x2E:
		segment = "CS";
		break;
	case 0x36:
		segment = "SS";
		break;
	case 0x3E:
		segment = "DS";
		break;
	case 0x64:
		segment = "FS";
		break;
	case 0x65:
		segment = "GS";
		break;
	case 0x66:
		//修改为16位模式
		Reg_Opcodebit = 1;
		break;
	};

	//分配内存，存放汇编指令
	buf = malloc(128);
	if (!buf)return 0;

	//解析MOD字节
	ModR_M = *(Opcodepoint + 1);
	R_M = Get_R_M(ModR_M);
	Mod = Get_Mod(ModR_M);
	Reg_Opcode = Get_Reg_Opcode(ModR_M);


	//mod字节长度
	InstructionLen += 1;

	//Gv寄存器
	switch (Reg_Opcodebit)
	{
	case 0:
		reg = reg8[Reg_Opcode];
		break;
	case 1:
		reg = reg16[Reg_Opcode];
		break;
	case 2:
		reg = reg32[Reg_Opcode];
		break;
	case 3:
		reg = reg64[Reg_Opcode];
		break;
	};

	//获取偏移
	if ((R_M == 4) && (Mod != 3))
	{
		//sib字节
		SIB = *(Opcodepoint + 2);
		InstructionLen += 1;
		switch (Mod)
		{
		case 0:
			dis = 0;
			break;
		case 1:
			dis = *(Opcodepoint + 3);
			InstructionLen += 1;
			break;
		case 2:
			dis = *(int*)(Opcodepoint + 3);
			InstructionLen += 4;
			break;
		};
	}
	else
	{
		switch (Mod)
		{
		case 0:
			dis = 0;
			if (R_M == 5) 
			{
				dis = *(int*)(Opcodepoint + 2);
			};
			break;
		case 1:
			dis = *(Opcodepoint + 2);
			InstructionLen += 1;
			break;
		case 2:
			dis = *(int*)(Opcodepoint + 2);
			InstructionLen += 4;
			break;
		};
	};

	//含sib处理
	if ((R_M == 4) && (Mod != 3))
	{
		//解析sib字节
		Base_index = Get_Base(SIB);
		Index_index = Get_Index(SIB);
		Scale_index = Get_Scale(SIB);


		Base = reg32[Base_index];
		Index = reg32[Index_index];
		Scale = 2 ^ Scale_index;
		//前缀按照前缀走，否则正常解析
		if (((Base_index == 5) || (Base_index == 4)) && (Prefix == 0))
		{
			segment = "SS";
		};


		if ((Base_index == 5) && (Mod == 0))
		{
			//Table2-3 mod==0&&Base_index==5特殊处理
			Base = NULL;
			dis = *(int*)(Opcodepoint + 3);
		};

		//esp为none，特殊处理
		if (Index_index == 4)
		{
			Index = NULL;
		};

		//填充结构体
		inst.segment = segment;
		inst.base = Base;
		inst.index = Index;
		inst.scale = Scale;
		inst.offset = dis;
		inst.data.data.Reg = reg;
		inst.data.type = DATA_STRING;

		//形成汇编语句
		generate_addr(buf, &inst, Reg_Opcodebit, EFB);
		return buf;
	};

	//不含sib处理。寄存器单独处理
	if (Mod <= 2)
	{
		if ((Mod == 0) && (R_M == 5))
		{
			Base = NULL;
		}
		else
		{
			Base = reg32[R_M];
		};

		if ((R_M == 5) && (Prefix == 0) && (Mod != 0))
		{
			segment = "SS";
		};
		if ((Mod == 0) && (R_M == 4))
		{
			//Table2-2 mod==0&&R/M==5特殊处理
			Base = NULL;
			dis = *(int*)(Opcodepoint + 3);
		};
		inst.segment = segment;
		inst.base = Base;
		inst.offset = dis;
		inst.data.data.Reg = reg;
		inst.data.type = DATA_STRING;
		//形成汇编语句
		generate_addr(buf, &inst, Reg_Opcodebit, EFB);
	}
	else
	{
		switch (Reg_Opcodebit)
		{
		case 0:
			reg_rm = reg8[R_M];
			break;
		case 1:
			reg_rm = reg16[R_M];
			break;
		case 2:
			reg_rm = reg32[R_M];
			break;
		case 3:
			reg_rm = reg64[R_M];
			break;
		};
		if (EFB == 1)
		{
			snprintf(buf, 128, "%s,%s", reg_rm, reg);
		}
		else
		{
			snprintf(buf, 128, "%s,%s", reg, reg_rm);
		};
	};
	return buf;
};

//这两个函数单独获取Ev
void generate_addr_Ev(char *out, Pinstruction inst, char Reg_Opcodebit)
{
	char addr[128] = { 0 };

	//标识第一次添加字符串
	int first = 1;
	//添加base
	if (inst->base)
	{
		snprintf(addr + strlen(addr), sizeof(addr) - strlen(addr), "%s", inst->base);
		first = 0;
	}
	//添加index
	if (inst->index)
	{
		snprintf(addr + strlen(addr), sizeof(addr) - strlen(addr), "%s%s*%d", first ? "" : "+", inst->index, inst->scale);
		first = 0;
	}
	//添加offset
	if (inst->offset != 0 || first)
	{
		snprintf(addr + strlen(addr), sizeof(addr) - strlen(addr), "%s0x%X", first ? "" : "+", inst->offset);
	}

	if (inst->data.type == DATA_IMMEDIATE)
	{

		snprintf(out, 128, "%s PTR %s:[%s],%X", DataLen[Reg_Opcodebit], inst->segment, addr, inst->data.data.Immediate);
	}
	else if (inst->data.type == DATA_STRING)
	{
		snprintf(out, 128, "%s PTR %s:[%s]", DataLen[Reg_Opcodebit], inst->segment, addr);
	}

}
char* ModR_MandSIB_Ev(char* Opcodepoint, char Prefix, char Reg_Opcodebit)
{
	char ModR_M;
	char SIB;

	char *buf;
	char R_M;
	char Reg_Opcode;
	char Mod;

	char Base_index;
	char Index_index;
	char Scale_index;

	char*Base;
	char*Index;
	char Scale;


	int dis = 0;				//偏移
	char* reg = NULL;				//汇编中的Gv寄存器
	char* reg_rm = NULL;		//汇编中的rm=3的寄存器
	char InstructionLen = 0;	//指令长度
	char* segment = "DS";		//默认数据段

	instruction inst = { 0 };

	//解析前缀
	switch (Prefix)
	{
	case 0x26:
		segment = "ES";
		break;
	case 0x2E:
		segment = "CS";
		break;
	case 0x36:
		segment = "SS";
		break;
	case 0x3E:
		segment = "DS";
		break;
	case 0x64:
		segment = "FS";
		break;
	case 0x65:
		segment = "GS";
		break;
	case 0x66:
		//修改为16位模式
		Reg_Opcodebit = 1;
		break;
	};
	if (Prefix)
	{
		InstructionLen += 1;
	};
	//分配内存，存放汇编指令
	buf = malloc(128);
	if (!buf)return 0;

	//解析MOD字节
	ModR_M = *(Opcodepoint + 1);
	R_M = Get_R_M(ModR_M);
	Mod = Get_Mod(ModR_M);
	Reg_Opcode = Get_Reg_Opcode(ModR_M);


	//mod字节长度
	InstructionLen += 1;

	//Gv寄存器
	switch (Reg_Opcodebit)
	{
	case 0:
		reg = reg8[Reg_Opcode];
		break;
	case 1:
		reg = reg16[Reg_Opcode];
		break;
	case 2:
		reg = reg32[Reg_Opcode];
		break;
	case 3:
		reg = reg64[Reg_Opcode];
		break;
	};

	//获取偏移
	if ((R_M == 4) && (Mod != 3))
	{
		//sib字节
		SIB = *(Opcodepoint + 2);
		InstructionLen += 1;
		switch (Mod)
		{
		case 0:
			dis = 0;
			break;
		case 1:
			dis = *(Opcodepoint + 3);
			InstructionLen += 1;
			break;
		case 2:
			dis = *(int*)(Opcodepoint + 3);
			InstructionLen += 4;
			break;
		};
	}
	else
	{
		switch (Mod)
		{
		case 0:
			dis = 0;
			if (R_M == 5)
			{
				dis = *(int*)(Opcodepoint + 2);
			};
			break;
		case 1:
			dis = *(Opcodepoint + 2);
			InstructionLen += 1;
			break;
		case 2:
			dis = *(int*)(Opcodepoint + 2);
			InstructionLen += 4;
			break;
		};
	};

	//含sib处理
	if ((R_M == 4) && (Mod != 3))
	{
		//解析sib字节
		Base_index = Get_Base(SIB);
		Index_index = Get_Index(SIB);
		Scale_index = Get_Scale(SIB);


		Base = reg32[Base_index];
		Index = reg32[Index_index];
		Scale = 2 ^ Scale_index;
		//前缀按照前缀走，否则正常解析
		if (((Base_index == 5) || (Base_index == 4)) && (Prefix == 0))
		{
			segment = "SS";
		};


		if ((Base_index == 5) && (Mod == 0))
		{
			//Table2-3 mod==0&&Base_index==5特殊处理
			Base = NULL;
			segment = "DS";
			dis = *(int*)(Opcodepoint + 3);
		};

		//esp为none，特殊处理
		if (Index_index == 4)
		{
			Index = NULL;
		};

		//填充结构体
		inst.segment = segment;
		inst.base = Base;
		inst.index = Index;
		inst.scale = Scale;
		inst.offset = dis;
		inst.data.data.Reg = reg;
		inst.data.type = DATA_STRING;

		//形成汇编语句
		generate_addr_Ev(buf, &inst, Reg_Opcodebit);

	};

	//不含sib处理。寄存器单独处理
	if (Mod <= 2)
	{
		if ((Mod == 0) && (R_M == 5))
		{
			Base = NULL;
		}
		else
		{
			Base = reg32[R_M];
		};
		if ((R_M == 5) && (Prefix == 0)&&(Mod != 0))
		{
			segment = "SS";
		};
		if ((Mod == 0) && (R_M == 4))
		{
			//Table2-2 mod==0&&R/M==5特殊处理
			Base = NULL;
			dis = *(int*)(Opcodepoint + 3);
		};
		inst.segment = segment;
		inst.base = Base;
		inst.offset = dis;
		inst.data.data.Reg = reg;
		inst.data.type = DATA_STRING;
		//形成汇编语句
		generate_addr_Ev(buf, &inst, Reg_Opcodebit);
	}
	else
	{
		switch (Reg_Opcodebit)
		{
		case 0:
			reg_rm = reg8[R_M];
			break;
		case 1:
			reg_rm = reg16[R_M];
			break;
		case 2:
			reg_rm = reg32[R_M];
			break;
		case 3:
			reg_rm = reg64[R_M];
			break;
		};
		snprintf(buf, 128, "%s", reg_rm);
	};

	return buf;
};

//获取TableA_6中的操作符
char* GetOpcodeTableA_6(char group, char Reg_Opcodebit)
{
	switch (group)
	{
	case 1:
	{
		return Group1[Reg_Opcodebit];
		break;
	};
	case 0x1A:
	{
		return Group1A[0];
		break;
	};
	case 2:
	{
		return Group2[Reg_Opcodebit];
		break;
	};
	case 3:
	{
		return Group3[Reg_Opcodebit];
		break;
	};
	case 4:
	{
		return Group4[Reg_Opcodebit];
		break;
	};
	case 5:
	{
		return Group5[Reg_Opcodebit];
		break;
	};
	case 6:
	{
		break;
	};
	case 7:
	{
		break;
	};
	case 8:
	{
		break;
	};
	case 9:
	{
		break;
	};
	case 10:
	{
		break;
	};
	case 11:
	{
		break;
	};


	};
};

//获取包含mod字节的指令长度，也即Ev部分。不包括立即数。Opcodepoint是语句开始的地址，如果有前缀，包括前缀
int GetModLen(char* Opcodepoint, char Prefix)
{
	char ModR_M;
	char SIB;
	char R_M;
	char Reg_Opcode;
	char Mod;
	char Base_index=0;
	int dis = 0;				//偏移

	char InstructionLen = 0;	//指令长度


	//opcode字节
	InstructionLen += 1;
	////前缀
	//if (Prefix)
	//{
	//	//前缀字节
	//	InstructionLen += 1;
	//}
	//解析MOD字节
	ModR_M = *(Opcodepoint + 1);
	R_M = Get_R_M(ModR_M);
	Mod = Get_Mod(ModR_M);

	//mod字节长度
	InstructionLen += 1;

	//}
	//else
	//{
	//	//解析MOD字节
	//	ModR_M = *(Opcodepoint + 1);
	//	R_M = Get_R_M(ModR_M);
	//	Mod = Get_Mod(ModR_M);

	//	//mod字节长度
	//	InstructionLen += 1;
	//};

	//获取偏移和计算指令长度,R_M为4表明有SIB字节，否则没有

	if ((R_M == 4) && (Mod != 3))
	{
		//sib字节
		InstructionLen += 1;
		SIB = *(Opcodepoint + 2);
		//解析sib字节
		Base_index = Get_Base(SIB);
	}
	switch (Mod)
	{
	case 0:
		if (R_M == 5)InstructionLen += 4;
		if (Base_index == 5)InstructionLen += 4;
		break;
	case 1:
		InstructionLen += 1;
		break;
	case 2:
		InstructionLen += 4;
		break;
	};
	//}
	//else
	//{
	//	switch (Mod)
	//	{
	//	case 0:
	//		break;
	//	case 1:
	//		InstructionLen += 1;
	//		break;
	//	case 2:
	//		InstructionLen += 4;
	//		break;
	//	};
	//};
	return InstructionLen;
};


/*
Entrypoint:指令的首地址指针
Prefix:前缀
Processbit:进程位数，16，32，64。默认32。
*/
char* ColOpcode_0(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);

	char* instruct = NULL;

	switch (RowOpcode)
	{
	case 0:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);

		snprintf(buf, 128, "ADD %s", instruct);
		break;
	};
	case 1:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);

		snprintf(buf, 128, "ADD %s", instruct);
		break;
	};
	case 2:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, FALSE);
		snprintf(buf, 128, "ADD %s", instruct);
		break;
	};
	case 3:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
		snprintf(buf, 128, "ADD %s", instruct);
		break;
	};
	case 4:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "ADD AL,%X", Ib);
		break;
	};
	case 5:
	{
		switch (Processbit)
		{
		case 16:
		{
			short Iw = *(short*)(Entrypoint + 1);
			snprintf(buf, 128, "ADD AX,%X", Iw);
			break;
		};

		case 32:
		{
			int Id = *(int*)(Entrypoint + 1);
			snprintf(buf, 128, "ADD EAX,%X", Id);
			break;
		};

		case 64:
		{
			DWORD64 Iq = *(DWORD64*)(Entrypoint + 1);
			snprintf(buf, 128, "ADD RAX,%llx", Iq);
			break;
		};

		};
		break;
	};
	case 6:
	{
		snprintf(buf, 128, "PUSH ES");
		CodeLen = 1;
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "POP ES");
		CodeLen = 1;
		break;
	};
	case 8:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "OR %s", instruct);
		break;
	};
	case 9:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "OR %s", instruct);
		break;
	};
	case 0xa:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, FALSE);
		snprintf(buf, 128, "OR %s", instruct);
		break;
	};
	case 0xb:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
		snprintf(buf, 128, "OR %s", instruct);
		break;
	};
	case 0xc:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "OR AL,%X", Ib);
		break;
	};
	case 0xd:
	{
		switch (Processbit)
		{
		case 16:
		{
			short Iw = *(short*)(Entrypoint + 1);
			snprintf(buf, 128, "OR AX,%X", Iw);
			break;
		};

		case 32:
		{
			int Id = *(int*)(Entrypoint + 1);
			snprintf(buf, 128, "OR EAX,%X", Id);
			break;
		};

		case 64:
		{
			DWORD64 Iq = *(DWORD64*)(Entrypoint + 1);
			snprintf(buf, 128, "OR RAX,%llx", Iq);
			break;
		};

		};
		break;
	};
	case 0xe:
	{
		snprintf(buf, 128, "PUSH CS");
		CodeLen = 1;
		break;
	};
	case 0xf:
	{
		//snprintf(buf, 128, "POP CS");
		//2字节opcode
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_1(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char RowOpcode = Opcode & 0xF;



	char* buf = malloc(128);
	if (!buf)return 0;

	char* instruct = NULL;
	switch (RowOpcode)
	{
	case 0:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "ADC %s", instruct);
		break;
	};
	case 1:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "ADC %s", instruct);
		break;
	};
	case 2:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, FALSE);
		snprintf(buf, 128, "ADC %s", instruct);
		break;
	};
	case 3:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
		snprintf(buf, 128, "ADC %s", instruct);
		break;
	};
	case 4:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "ADC AL,%X", Ib);
		break;
	};
	case 5:
	{
		switch (Processbit)
		{
		case 16:
		{
			short Iw = *(short*)(Entrypoint + 1);
			snprintf(buf, 128, "ADC AX,%X", Iw);
			break;
		};

		case 32:
		{
			int Id = *(int*)(Entrypoint + 1);
			snprintf(buf, 128, "ADC EAX,%X", Id);
			break;
		};

		case 64:
		{
			DWORD64 Iq = *(DWORD64*)(Entrypoint + 1);
			snprintf(buf, 128, "ADC RAX,%llx", Iq);
			break;
		};

		};
		break;
	};
	case 6:
	{
		snprintf(buf, 128, "PUSH SS");
		CodeLen = 1;
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "POP SS");
		CodeLen = 1;
		break;
	};
	case 8:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "SBB %s", instruct);
		break;
	};
	case 9:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "SBB %s", instruct);
		break;
	};
	case 0xa:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, FALSE);
		snprintf(buf, 128, "SBB %s", instruct);
		break;
	};
	case 0xb:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
		snprintf(buf, 128, "SBB %s", instruct);
		break;
	};
	case 0xc:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "SBB AL,%X", Ib);
		break;
	};
	case 0xd:
	{
		switch (Processbit)
		{
		case 16:
		{
			short Iw = *(short*)(Entrypoint + 1);
			snprintf(buf, 128, "SBB AX,%X", Iw);
			break;
		};

		case 32:
		{
			int Id = *(int*)(Entrypoint + 1);
			snprintf(buf, 128, "SBB EAX,%X", Id);
			break;
		};

		case 64:
		{
			DWORD64 Iq = *(DWORD64*)(Entrypoint + 1);
			snprintf(buf, 128, "SBB RAX,%llx", Iq);
			break;
		};

		};
		break;
	};
	case 0xe:
	{
		snprintf(buf, 128, "PUSH DS");
		CodeLen = 1;
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "POP DS");
		CodeLen = 1;
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_2(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);

	char* instruct = NULL;
	switch (RowOpcode)
	{
	case 0:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "AND %s", instruct);
		break;
	};
	case 1:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "AND %s", instruct);
		break;
	};
	case 2:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, FALSE);
		snprintf(buf, 128, "AND %s", instruct);
		break;
	};
	case 3:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
		snprintf(buf, 128, "AND %s", instruct);
		break;
	};
	case 4:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "AND AL,%X", Ib);
		break;
	};
	case 5:
	{
		switch (Processbit)
		{
		case 16:
		{
			short Iw = *(short*)(Entrypoint + 1);
			snprintf(buf, 128, "AND AX,%X", Iw);
			break;
		};

		case 32:
		{
			int Id = *(int*)(Entrypoint + 1);
			snprintf(buf, 128, "AND EAX,%X", Id);
			break;
		};

		case 64:
		{
			DWORD64 Iq = *(DWORD64*)(Entrypoint + 1);
			snprintf(buf, 128, "AND RAX,%llx", Iq);
			break;
		};

		};
		break;
	};
	case 6:
	{
		//有段前缀，一定有mod。Entrypoint + 1，把前缀偏移掉
		OneByteOpcode(Entrypoint + 1, 0x26);
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "DAA");
		CodeLen = 1;
		break;
	};
	case 8:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "SUB %s", instruct);

		break;
	};
	case 9:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "SUB %s", instruct);
		break;
	};
	case 0xa:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, FALSE);
		snprintf(buf, 128, "SUB %s", instruct);
		break;
	};
	case 0xb:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
		snprintf(buf, 128, "SUB %s", instruct);
		break;
	};
	case 0xc:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "SUB AL,%X", Ib);
		break;
	};
	case 0xd:
	{
		switch (Processbit)
		{
		case 16:
		{
			short Iw = *(short*)(Entrypoint + 1);
			snprintf(buf, 128, "SUB AX,%X", Iw);
			break;
		};

		case 32:
		{
			int Id = *(int*)(Entrypoint + 1);
			snprintf(buf, 128, "SUB EAX,%X", Id);
			break;
		};

		case 64:
		{
			DWORD64 Iq = *(DWORD64*)(Entrypoint + 1);
			snprintf(buf, 128, "SUB RAX,%llx", Iq);
			break;
		};

		};
		break;
	};
	case 0xe:
	{
		OneByteOpcode(Entrypoint + 1, 0x2E);
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "DAS");
		CodeLen = 1;

		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_3(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char RowOpcode = Opcode & 0xF;



	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);

	char* instruct = NULL;
	switch (RowOpcode)
	{
	case 0:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "XOR %s", instruct);
		break;
	};
	case 1:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "XOR %s", instruct);
		break;
	};
	case 2:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, FALSE);
		snprintf(buf, 128, "XOR %s", instruct);
		break;
	};
	case 3:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
		snprintf(buf, 128, "XOR %s", instruct);
		break;
	};
	case 4:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "XOR AL,%X", Ib);
		break;
	};
	case 5:
	{
		switch (Processbit)
		{
		case 16:
		{
			short Iw = *(short*)(Entrypoint + 1);
			snprintf(buf, 128, "XOR AX,%X", Iw);
			break;
		};

		case 32:
		{
			int Id = *(int*)(Entrypoint + 1);
			snprintf(buf, 128, "XOR EAX,%X", Id);
			break;
		};

		case 64:
		{
			DWORD64 Iq = *(DWORD64*)(Entrypoint + 1);
			snprintf(buf, 128, "XOR RAX,%llx", Iq);
			break;
		};

		};
		break;
	};
	case 6:
	{
		//有段前缀，一定有mod。Entrypoint + 1，把前缀偏移掉
		OneByteOpcode(Entrypoint + 1, 0x36);
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "AAA");
		CodeLen = 1;
		break;
	};
	case 8:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "CMP %s", instruct);
		break;
	};
	case 9:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "CMP %s", instruct);
		break;
	};
	case 0xa:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, FALSE);
		snprintf(buf, 128, "CMP %s", instruct);
		break;
	};
	case 0xb:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
		snprintf(buf, 128, "CMP %s", instruct);
		break;
	};
	case 0xc:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "CMP AL,%X", Ib);
		break;
	};
	case 0xd:
	{
		switch (Processbit)
		{
		case 16:
		{
			short Iw = *(short*)(Entrypoint + 1);
			snprintf(buf, 128, "CMP AX,%X", Iw);
			break;
		};

		case 32:
		{
			int Id = *(int*)(Entrypoint + 1);
			snprintf(buf, 128, "CMP EAX,%X", Id);
			break;
		};

		case 64:
		{
			DWORD64 Iq = *(DWORD64*)(Entrypoint + 1);
			snprintf(buf, 128, "CMP RAX,%llx", Iq);
			break;
		};

		};
		break;
	};
	case 0xe:
	{
		OneByteOpcode(Entrypoint + 1, 0x3E);
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "AAS");
		CodeLen = 1;
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_4(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);

	char* instruct = NULL;

	CodeLen = 1;

	switch (RowOpcode)
	{
	case 0:
	{
		snprintf(buf, 128, "INC EAX");
		break;
	};
	case 1:
	{
		snprintf(buf, 128, "INC ECX");
		break;
	};
	case 2:
	{
		snprintf(buf, 128, "INC EDX");
		break;
	};
	case 3:
	{
		snprintf(buf, 128, "INC EBX");
		break;
	};
	case 4:
	{
		snprintf(buf, 128, "INC ESP");
		break;
	};
	case 5:
	{
		snprintf(buf, 128, "INC EBP");
		break;
	};
	case 6:
	{
		snprintf(buf, 128, "INC ESI");
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "INC EDI");
		break;
	};
	case 8:
	{
		snprintf(buf, 128, "DEC EAX");
		break;
	};
	case 9:
	{
		snprintf(buf, 128, "DEC ECX");
		break;
	};
	case 0xa:
	{
		snprintf(buf, 128, "DEC EDX");
		break;
	};
	case 0xb:
	{
		snprintf(buf, 128, "DEC EBX");
		break;
	};
	case 0xc:
	{
		snprintf(buf, 128, "DEC ESP");
		break;
	};
	case 0xd:
	{
		snprintf(buf, 128, "DEC EBP");
		break;
	};
	case 0xe:
	{
		snprintf(buf, 128, "DEC ESI");
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "DEC EDI");
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_5(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char RowOpcode = Opcode & 0xF;

	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);

	char* instruct = NULL;

	CodeLen = 1;

	switch (RowOpcode)
	{
	case 0:
	{
		snprintf(buf, 128, "PUSH EAX");
		break;
	};
	case 1:
	{
		snprintf(buf, 128, "PUSH ECX");
		break;
	};
	case 2:
	{
		snprintf(buf, 128, "PUSH EDX");
		break;
	};
	case 3:
	{
		snprintf(buf, 128, "PUSH EBX");
		break;
	};
	case 4:
	{
		snprintf(buf, 128, "PUSH ESP");
		break;
	};
	case 5:
	{
		snprintf(buf, 128, "PUSH EBP");
		break;
	};
	case 6:
	{
		snprintf(buf, 128, "PUSH ESI");
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "PUSH EDI");
		break;
	};
	case 8:
	{
		snprintf(buf, 128, "POP EAX");
		break;
	};
	case 9:
	{
		snprintf(buf, 128, "POP ECX");
		break;
	};
	case 0xa:
	{
		snprintf(buf, 128, "POP EDX");
		break;
	};
	case 0xb:
	{
		snprintf(buf, 128, "POP EBX");
		break;
	};
	case 0xc:
	{
		snprintf(buf, 128, "POP ESP");
		break;
	};
	case 0xd:
	{
		snprintf(buf, 128, "POP EBP");
		break;
	};
	case 0xe:
	{
		snprintf(buf, 128, "POP ESI");
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "POP EDI");
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_6(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);

	char* instruct = NULL;
	switch (RowOpcode)
	{
	case 0:
	{
		if (Prefix == 0x66)
		{

			snprintf(buf, 128, "PUSHAW");
		}
		else
		{
			snprintf(buf, 128, "PUSHAD");
		}
		CodeLen = 1;
		break;
	};
	case 1:
	{
		if (Prefix == 0x66)
		{

			snprintf(buf, 128, "POPAW");
		}
		else
		{
			snprintf(buf, 128, "POPAD");
		}
		CodeLen = 1;
		break;
	};
	case 2:
	{
		if (Prefix == 0x66)
		{
			instruct = ModR_MandSIB(Entrypoint, Prefix, win16, FALSE);
			snprintf(buf, 128, "BOUND %s", instruct);
		}
		else
		{
			instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
			snprintf(buf, 128, "BOUND %s", instruct);
		}

		break;
	};
	case 3:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win16, TRUE);
		snprintf(buf, 128, "ARPL %s", instruct);
		break;
	};
	case 4:
	{
		OneByteOpcode(Entrypoint + 1, 0x64);
		break;
	};
	case 5:
	{
		OneByteOpcode(Entrypoint + 1, 0x65);
		break;
	};
	case 6:
	{
		OneByteOpcode(Entrypoint + 1, 0x66);
		break;
	};
	case 7:
	{
		OneByteOpcode(Entrypoint + 1, 0x67);
		break;
	};
	case 8:
	{
		switch (Processbit)
		{
		case 16:
		{
			short Iw = *(short*)(Entrypoint + 1);
			snprintf(buf, 128, "PUSH %X", Iw);
			CodeLen = 3;
			break;
		};

		case 32:
		{
			int Id = *(int*)(Entrypoint + 1);
			snprintf(buf, 128, "PUSH %X", Id);
			CodeLen = 5;
			break;
		};

		case 64:
		{
			DWORD64 Iq = *(DWORD64*)(Entrypoint + 1);
			snprintf(buf, 128, "PUSH %llx", Iq);
			CodeLen = 9;
			break;
		};
		}
		break;
	};
	case 9:
	{
		if (Prefix == 0x66)
		{
			instruct = ModR_MandSIB(Entrypoint, Prefix, win16, FALSE);
			snprintf(buf, 128, "IMUL %s", instruct);
		}
		else
		{
			instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
			snprintf(buf, 128, "IMUL %s", instruct);
		}
		break;
	};
	case 0xa:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "PUSH %X", Ib);
		CodeLen = 2;
		break;
	};
	case 0xb:
	{
		//这个和9没有改
		snprintf(buf, 128, "INS BYTE PTR ES:[EDI],DX");
		break;
	};
	case 0xc:
	{
		snprintf(buf, 128, "INS BYTE PTR ES:[EDI],DX");
		CodeLen = 1;
		break;
	};
	case 0xd:
	{
		snprintf(buf, 128, "INS DWORD PTR ES:[EDI],DX");
		CodeLen = 1;
		break;
	};
	case 0xe:
	{
		snprintf(buf, 128, "OUTS DX,BYTE PTR ES:[EDI]");
		CodeLen = 1;
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "OUTS DX,DWORD PTR ES:[EDI]");
		CodeLen = 1;
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_7(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char offset = *(Entrypoint + 1);

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);

	char* instruct = NULL;

	CodeLen = 2;

	switch (RowOpcode)
	{
	case 0:
	{
		snprintf(buf, 128, "JO %X", offset+ Entrypoint);
		break;
	};
	case 1:
	{
		snprintf(buf, 128, "JNO %X", offset + Entrypoint);
		break;
	};
	case 2:
	{
		snprintf(buf, 128, "JB %X", offset + Entrypoint);
		break;
	};
	case 3:
	{
		snprintf(buf, 128, "JNB %X", offset + Entrypoint);
		break;
	};
	case 4:
	{
		snprintf(buf, 128, "JZ %X", offset + Entrypoint);
		break;
	};
	case 5:
	{
		snprintf(buf, 128, "JNZ %X", offset + Entrypoint);
		break;
	};
	case 6:
	{
		snprintf(buf, 128, "JNA %X", offset + Entrypoint);
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "JA %X", offset + Entrypoint);
		break;
	};
	case 8:
	{
		snprintf(buf, 128, "JS %X", offset + Entrypoint);
		break;
	};
	case 9:
	{
		snprintf(buf, 128, "JNS %X", offset + Entrypoint);
		break;
	};
	case 0xa:
	{
		snprintf(buf, 128, "JP %X", offset + Entrypoint);
		break;
	};
	case 0xb:
	{
		snprintf(buf, 128, "JNP %X", offset + Entrypoint);
		break;
	};
	case 0xc:
	{
		snprintf(buf, 128, "JL %X", offset + Entrypoint);
		break;
	};
	case 0xd:
	{
		snprintf(buf, 128, "JNL %X", offset + Entrypoint);
		break;
	};
	case 0xe:
	{
		snprintf(buf, 128, "JNG %X", offset + Entrypoint);
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "JG %X", offset + Entrypoint);
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_8(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char offset = *(Entrypoint + 1);

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);


	char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));

	char* instruct = NULL;
	switch (RowOpcode)
	{
	case 0:
	{
		int len = GetModLen(Entrypoint, Prefix, win8);
		char Ib = *(Entrypoint + len);
		char* instruction = GetOpcodeTableA_6(1, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);

		snprintf(buf, 128, "%s %s,%X", instruction, instruct, Ib);
		CodeLen += 1;
		break;
	};
	case 1:
	{
		int len = GetModLen(Entrypoint, Prefix, win32);
		int Id = *(Entrypoint + len);
		char* instruction = GetOpcodeTableA_6(1, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);

		snprintf(buf, 128, "%s %s,%X", instruction, instruct, Id);
		CodeLen += 4;
		break;
	};
	case 2:
	{
		int len = GetModLen(Entrypoint, Prefix, win8);
		char Ib = *(Entrypoint + len );
		char* instruction = GetOpcodeTableA_6(1, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);

		snprintf(buf, 128, "%s %s,%X", instruction, instruct, Ib);
		CodeLen += 1;
		break;
	};
	case 3:
	{
		int len = GetModLen(Entrypoint, Prefix, win32);
		char Ib = *(Entrypoint + len);
		char* instruction = GetOpcodeTableA_6(1, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);

		snprintf(buf, 128, "%s %s,%X", instruction, instruct, Ib);
		CodeLen += 1;
		break;
	};
	case 4:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "TEST %s", instruct);
		break;
	};
	case 5:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "TEST %s", instruct);
		break;
	};
	case 6:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "XCHG %s", instruct);
		break;
	};
	case 7:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "XCHG %s", instruct);
		break;
	};
	case 8:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, TRUE);
		snprintf(buf, 128, "MOV %s", instruct);
		break;
	};
	case 9:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, TRUE);
		snprintf(buf, 128, "MOV %s", instruct);
		break;
	};
	case 0xa:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win8, FALSE);
		snprintf(buf, 128, "MOV %s", instruct);
		break;
	};
	case 0xb:
	{
		instruct = ModR_MandSIB(Entrypoint, Prefix, win32, FALSE);
		snprintf(buf, 128, "MOV %s", instruct);
		break;
	};
	case 0xc:
	{
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
		snprintf(buf, 128, "MOV %s,%s", instruct, regSeg[Reg_Opcode]);
		break;
	};
	case 0xd:
	{
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
		snprintf(buf, 128, "LEA %s,%s", reg32[Reg_Opcode], instruct);
		break;
	};
	case 0xe:
	{
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
		snprintf(buf, 128, "MOV %s,%s", regSeg[Reg_Opcode], instruct);
		break;
	};
	case 0xf:
	{
		char* instruction = GetOpcodeTableA_6(0x1A, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);

		snprintf(buf, 128, "%s %s", instruction, instruct);
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_9(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	char offset = *(Entrypoint + 1);

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);


	char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));

	char* instruct = NULL;

	CodeLen = 1;

	switch (RowOpcode)
	{
	case 0:
	{
		snprintf(buf, 128, "NOP");
		break;
	};
	case 1:
	{
		snprintf(buf, 128, "XCHG EAX,ECX");
		break;
	};
	case 2:
	{
		snprintf(buf, 128, "XCHG EAX,EDX");
		break;
	};
	case 3:
	{
		snprintf(buf, 128, "XCHG EAX,EBX");
		break;
	};
	case 4:
	{
		snprintf(buf, 128, "XCHG EAX,ESP");
		break;
	};
	case 5:
	{
		snprintf(buf, 128, "XCHG EAX,EBP");
		break;
	};
	case 6:
	{
		snprintf(buf, 128, "XCHG EAX,ESI");
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "XCHG EAX,EDI");
		break;
	};
	case 8:
	{
		snprintf(buf, 128, "CWDE");
		break;
	};
	case 9:
	{
		snprintf(buf, 128, "CDQ");
		break;
	};
	case 0xa:
	{
		int offse1 = *(int*)(Entrypoint + 1);
		short offse2 = *(short*)(Entrypoint + 5);
		snprintf(buf, 128, "CALL FAR %X,%X", offse1, offse2);
		CodeLen = 7;
		break;
	};
	case 0xb:
	{
		snprintf(buf, 128, "WAIT");
		break;
	};
	case 0xc:
	{
		snprintf(buf, 128, "PUSHFD");
		break;
	};
	case 0xd:
	{
		snprintf(buf, 128, "POPFD");
		break;
	};
	case 0xe:
	{
		//把寄存器 AH中的值，存入标志寄存器低 8 位中
		snprintf(buf, 128, "SAHF");
		break;
	};
	case 0xf:
	{
		//把 FLAGS 寄存器中的低 8 位状态标志加载到 AH 寄存器中
		snprintf(buf, 128, "LAHF");
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_A(char* Entrypoint, char Processbit, char Prefix)
{
	char* Segment = "DS";

	char Opcode = *Entrypoint;

	char offset = *(Entrypoint + 1);

	char RowOpcode = Opcode & 0xF;



	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);


	char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));
	if (Prefix)
	{
		Segment = GetSegmentforPrefix(Prefix);
	};
	
	char* instruct = NULL;

	CodeLen = 5;
	switch (RowOpcode)
	{
	case 0:
	{
		int offse1 = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV AL,BYTE PTR %s:[%X]", Segment, offse1);
		break;
	};
	case 1:
	{
		int offse1 = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV EAX,DYTE PTR %s:[%X]", Segment, offse1);
		break;
	};
	case 2:
	{
		int offse1 = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV BYTE PTR %s:[%X],AL", Segment, offse1);
		break;
	};
	case 3:
	{
		int offse1 = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV DYTE PTR %s:[%X],EAX", Segment, offse1);
		break;
	};
	case 4:
	{
		snprintf(buf, 128, "MOVS BYTE PTR ES:[EDI],BYTE PTR DS:[ESI]");
		CodeLen = 1;
		break;
	};
	case 5:
	{
		snprintf(buf, 128, "MOVS DWORD PTR ES:[EDI],DWORD PTR DS:[ESI]");
		CodeLen = 1;
		break;
	};
	case 6:
	{
		snprintf(buf, 128, "CMPS BYTE PTR DS:[ESI],BYTE PTR ES:[EDI]");
		CodeLen = 1;
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "CMPS DWORD PTR DS:[ESI],DWORD PTR ES:[EDI]");
		CodeLen = 1;
		break;
	};
	case 8:
	{
		offset = *(Entrypoint + 1);
		snprintf(buf, 128, "TEST AL,%X", offset);
		CodeLen = 2;
		break;
	};
	case 9:
	{
		int offse1 = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "TEST EAX,%X", offse1);
		break;
	};
	case 0xa:
	{
		snprintf(buf, 128, "STOS BYTE PTR ES:[EDI]");
		CodeLen = 1;
		break;
	};
	case 0xb:
	{
		snprintf(buf, 128, "STOS DWORD PTR ES:[EDI]");
		CodeLen = 1;
		break;
	};
	case 0xc:
	{
		snprintf(buf, 128, "LODS BYTE PTR DS:[ESI]");
		CodeLen = 1;
		break;
	};
	case 0xd:
	{
		snprintf(buf, 128, "LODS DWORD PTR DS:[ESI]");
		CodeLen = 1;
		break;
	};
	case 0xe:
	{
		snprintf(buf, 128, "SCAS BYTE PTR ES:[EDI]");
		CodeLen = 1;
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "SCAS DWORD PTR ES:[EDI]");
		CodeLen = 1;
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_B(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	int offset = *(Entrypoint + 1);

	char RowOpcode = Opcode & 0xF;



	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);


	char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));

	char* instruct = NULL;

	if (RowOpcode < 8)
	{
		CodeLen = 2;
	}
	else
	{
		CodeLen = 5;
	};

	switch (RowOpcode)
	{
	case 0:
	{
		snprintf(buf, 128, "MOV AL,%X", offset);
		break;
	};
	case 1:
	{
		snprintf(buf, 128, "MOV CL,%X", offset);
		break;
	};
	case 2:
	{
		snprintf(buf, 128, "MOV DL,%X", offset);
		break;
	};
	case 3:
	{
		snprintf(buf, 128, "MOV BL,%X", offset);
		break;
	};
	case 4:
	{
		snprintf(buf, 128, "MOV AH,%X", offset);
		break;
	};
	case 5:
	{
		snprintf(buf, 128, "MOV CH,%X", offset);
		break;
	};
	case 6:
	{
		snprintf(buf, 128, "MOV DH,%X", offset);
		break;
	};
	case 7:
	{
		snprintf(buf, 128, "MOV BH,%X", offset);
		break;
	};
	case 8:
	{
		offset = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV EAX,%X", offset);
		break;
	};
	case 9:
	{
		offset = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV ECX,%X", offset);
		break;
	};
	case 0xa:
	{
		offset = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV EDX,%X", offset);
		break;
	};
	case 0xb:
	{
		offset = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV EBX,%X", offset);
		break;
	};
	case 0xc:
	{
		offset = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV ESP,%X", offset);
		break;
	};
	case 0xd:
	{
		offset = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV EBP,%X", offset);
		break;
	};
	case 0xe:
	{
		offset = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV ESI,%X", offset);
		break;
	};
	case 0xf:
	{
		offset = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "MOV EDI,%X", offset);
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_C(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	int offset = *(Entrypoint + 1);

	char RowOpcode = Opcode & 0xF;



	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);



	char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));

	char* instruct = NULL;
	switch (RowOpcode)
	{
	case 0:
	{
		int len = GetModLen(Entrypoint, Prefix, win8);
		char Ib = *(Entrypoint + len);
		char* instruction = GetOpcodeTableA_6(2, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);

		snprintf(buf, 128, "%s %s,%X", instruction, instruct, Ib);
		CodeLen = len + 1;
		break;
	};
	case 1:
	{
		int len = GetModLen(Entrypoint, Prefix, win32);
		char Ib = *(Entrypoint + len );
		char* instruction = GetOpcodeTableA_6(2, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);

		snprintf(buf, 128, "%s %s,%X", instruction, instruct, Ib);
		CodeLen = len + 1;
		break;
	};
	case 2:
	{
		short Iw = *(short*)(Entrypoint + 1);
		snprintf(buf, 128, "RETN %X", Iw);
		CodeLen = 3;
		break;
	};
	case 3:
	{
		snprintf(buf, 128, "RETN");
		CodeLen = 1;
		break;
	};
	case 4:
	{
		char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
		snprintf(buf, 128, "LES %s,%s", reg32[Reg_Opcode], instruct);
		break;
	};
	case 5:
	{
		char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
		snprintf(buf, 128, "LDS %s,%s", reg32[Reg_Opcode], instruct);
		break;
	};
	case 6:
	{
		switch (Reg_Opcode)
		{
		case 0:
		{
			int len = GetModLen(Entrypoint, Prefix, win8);
			char Ib = *(Entrypoint + len);
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);
			snprintf(buf, 128, "MOV %s,%X", instruct, Ib);
			CodeLen = len + 1;
			break;
		};
		default:
		{
			snprintf(buf, 128, "NULL");
		};

		};

		break;
	};
	case 7:
	{
		switch (Reg_Opcode)
		{
		case 0:
		{
			int len = GetModLen(Entrypoint, Prefix, win32);
			int Id = *(int*)(Entrypoint + len);
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "MOV %s,%X", instruct, Id);
			CodeLen = len + 4;
			break;
		};
		default:
		{
			snprintf(buf, 128, "NULL");
		};
		break;
		}
		break;
	};
	case 8:
	{
		offset = *(short*)(Entrypoint + 1);
		int offset1 = *(short*)(Entrypoint + 3);
		snprintf(buf, 128, "ENTER %X,%X", offset, offset1);
		CodeLen = 5;
		break;
	};
	case 9:
	{
		snprintf(buf, 128, "LEAVE");
		CodeLen = 1;
		break;
	};
	case 0xa:
	{
		short Iw = *(short*)(Entrypoint + 1);
		snprintf(buf, 128, "RETF %X", Iw);
		CodeLen = 3;
		break;
	};
	case 0xb:
	{
		snprintf(buf, 128, "RETF");
		CodeLen = 1;
		break;
	};
	case 0xc:
	{
		snprintf(buf, 128, "INT 3");
		CodeLen = 1;
		break;
	};
	case 0xd:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "INT %X", Ib);
		CodeLen = 2;
		break;
	};
	case 0xe:
	{
		snprintf(buf, 128, "INTO");
		CodeLen = 1;
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "IRETD");
		CodeLen = 1;
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_D(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	int offset = *(Entrypoint + 1);

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);


	char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));

	char* instruct = NULL;
	switch (RowOpcode)
	{
	case 0:
	{

		char* instruction = GetOpcodeTableA_6(2, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);

		snprintf(buf, 128, "%s %s,1", instruction, instruct);

		break;
	};
	case 1:
	{
		char* instruction = GetOpcodeTableA_6(2, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);

		snprintf(buf, 128, "%s %s,1", instruction, instruct);
		break;
	};
	case 2:
	{
		char* instruction = GetOpcodeTableA_6(2, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);

		snprintf(buf, 128, "%s %s,CL", instruction, instruct);
		break;
	};
	case 3:
	{
		char* instruction = GetOpcodeTableA_6(2, Reg_Opcode);
		instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);

		snprintf(buf, 128, "%s %s,CL", instruction, instruct);
		break;
	};
	case 4:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "AMM %X", Ib);
		CodeLen = 2;
		break;
	};
	case 5:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "AMD %X", Ib);
		CodeLen = 2;
		break;
	};
	case 6:
	{

		snprintf(buf, 128, "SALC");
		CodeLen = 1;
		break;
	};
	case 7:
	{
		//它将内存中位于 [BX + AL]（16位模式）或者 [RBX + AL]地址处的字节加载到 AL 寄存器中。
		snprintf(buf, 128, "XLAT BYTE PTR DS:[EBX+AL]");
		CodeLen = 1;
		break;
	};
	case 8:
	{

		break;
	};
	case 9:
	{

		break;
	};
	case 0xa:
	{

		break;
	};
	case 0xb:
	{

		break;
	};
	case 0xc:
	{

		break;
	};
	case 0xd:
	{

		break;
	};
	case 0xe:
	{

		break;
	};
	case 0xf:
	{

		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_E(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	int offset = *(Entrypoint + 1);

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);


	char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));

	char* instruct = NULL;

	CodeLen = 2;

	switch (RowOpcode)
	{
	case 0:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "LOOPNE %X", Ib);
		break;
	};
	case 1:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "LOOPE %X", Ib);
		break;
	};
	case 2:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "LOOP %X", Ib);
		break;
	};
	case 3:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "JECXZ %X", Ib);
		break;
	};
	case 4:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "IN AL,%X", Ib);
		break;
	};
	case 5:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "IN EAX,%X", Ib);
		break;
	};
	case 6:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "OUT %X,AL", Ib);
		break;
	};
	case 7:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "OUT %X,EAX", Ib);
		break;
	};
	case 8:
	{
		int Id = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "CALL %X", Id+ Entrypoint);

		CodeLen = 5;
		break;
	};
	case 9:
	{
		int Id = *(int*)(Entrypoint + 1);
		snprintf(buf, 128, "JMP NEAR %X", Id + Entrypoint + 5);

		CodeLen = 5;
		break;
	};
	case 0xa:
	{
		int offse1 = *(int*)(Entrypoint + 1);
		short offse2 = *(short*)(Entrypoint + 5);
		snprintf(buf, 128, "JMP FAR %X,%X", offse1, offse2);
		CodeLen = 7;
		break;
	};
	case 0xb:
	{
		char Ib = *(Entrypoint + 1);
		snprintf(buf, 128, "JMP SHORT %X", Ib + Entrypoint + 5);
		CodeLen = 2;
		break;
	};
	case 0xc:
	{
		snprintf(buf, 128, "IN AL,DX");
		CodeLen = 1;
		break;
	};
	case 0xd:
	{
		snprintf(buf, 128, "IN EAX,DX");
		CodeLen = 1;
		break;
	};
	case 0xe:
	{
		snprintf(buf, 128, "OUT DX,AL");
		CodeLen = 1;
		break;
	};
	case 0xf:
	{
		snprintf(buf, 128, "OUT DX,EAX");
		CodeLen = 1;
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* ColOpcode_F(char* Entrypoint, char Processbit, char Prefix)
{
	char Opcode = *Entrypoint;

	int offset = *(Entrypoint + 1);

	char RowOpcode = Opcode & 0xF;


	char* buf = malloc(128);
	if (!buf)return 0;
	memset(buf, 0, 128);


	char Reg_Opcode = Get_Reg_Opcode(*(Entrypoint + 1));

	char* instruct = NULL;

	

	switch (RowOpcode)
	{
	case 0:
	{
		char* NonePrefix = OneByteOpcode(Entrypoint + 1, NULL);
		snprintf(buf, 128, "LOCK %s", NonePrefix);
		break;
	};
	case 1:
	{
		snprintf(buf, 128, "INT1");
		CodeLen = 1;
		break;
	};
	case 2:
	{
		char* NonePrefix = OneByteOpcode(Entrypoint + 1, NULL);
		snprintf(buf, 128, "REPNE %s", NonePrefix);
		break;
	};
	case 3:
	{
		char* NonePrefix = OneByteOpcode(Entrypoint + 1, NULL);
		snprintf(buf, 128, "REP %s", NonePrefix);
		break;
	};
	case 4:
	{
		snprintf(buf, 128, "HLT");
		CodeLen = 1;
		break;
	};
	case 5:
	{
		snprintf(buf, 128, "CMC");
		CodeLen = 1;
		break;
	};
	case 6:
	{
		switch (Reg_Opcode)
		{
		case 0:
		{
			int len = GetModLen(Entrypoint, Prefix, win8);
			char Ib = *(Entrypoint + len);
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);
			snprintf(buf, 128, "TEST %s,%X", instruct, Ib);
			CodeLen += 1;
			break;
		};
		case 1:
		{
			snprintf(buf, 128, "UNKNOWN COMMAND");
			break;
		};
		case 2:
		{
			char* instruction = GetOpcodeTableA_6(3, Reg_Opcode);
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);

			snprintf(buf, 128, "%s %s", instruction, instruct);
			break;
		};
		case 3:
		{
			char* instruction = GetOpcodeTableA_6(3, Reg_Opcode);
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);

			snprintf(buf, 128, "%s %s", instruction, instruct);
			break;
		};
		case 4:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);
			snprintf(buf, 128, "MUL %s", instruct);
			break;
		};
		case 5:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);
			snprintf(buf, 128, "IMUL %s", instruct);
			break;
		};
		case 6:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);
			snprintf(buf, 128, "DIV %s", instruct);
			break;
		};
		case 7:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);
			snprintf(buf, 128, "IDIV %s", instruct);
			break;
		};

		};

		break;
	};
	case 7:
	{
		switch (Reg_Opcode)
		{
		case 0:
		{
			int len = GetModLen(Entrypoint, Prefix, win32);
			int Id = *(int*)(Entrypoint + len);
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "TEST %s,%X", instruct, Id);
			CodeLen += 4;
			break;
		};
		case 1:
		{
			snprintf(buf, 128, "UNKNOWN COMMAND");
			break;
		};
		case 2:
		{
			char* instruction = GetOpcodeTableA_6(3, Reg_Opcode);
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);

			snprintf(buf, 128, "%s %s", instruction, instruct);
			break;
		};
		case 3:
		{
			char* instruction = GetOpcodeTableA_6(3, Reg_Opcode);
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);

			snprintf(buf, 128, "%s %s", instruction, instruct);
			break;
		};
		case 4:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "MUL %s", instruct);
			break;
		};
		case 5:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "IMUL %s", instruct);
			break;
		};
		case 6:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "DIV %s", instruct);
			break;
		};
		case 7:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "IDIV %s", instruct);
			break;
		};

		};
		break;
	};
	case 8:
	{
		snprintf(buf, 128, "CLC");
		CodeLen = 1;
		break;
	};
	case 9:
	{
		snprintf(buf, 128, "STC");
		CodeLen = 1;
		break;
	};
	case 0xa:
	{
		snprintf(buf, 128, "CLI");
		CodeLen = 1;
		break;
	};
	case 0xb:
	{
		snprintf(buf, 128, "STI");
		CodeLen = 1;
		break;
	};
	case 0xc:
	{
		snprintf(buf, 128, "CLD");
		CodeLen = 1;
		break;
	};
	case 0xd:
	{
		snprintf(buf, 128, "STD");
		CodeLen = 1;
		break;
	};
	case 0xe:
	{
		switch (Reg_Opcode)
		{
		case 0:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);
			snprintf(buf, 128, "INC %s", instruct);
			break;
		};
		case 1:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win8);
			snprintf(buf, 128, "DEC %s", instruct);
			break;
		};

		}
		break;
	};
	case 0xf:
	{
		switch (Reg_Opcode)
		{
		case 0:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "INC %s", instruct);
			break;
		};
		case 1:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "DEC %s", instruct);
			break;
		};
		case 2:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "CALL NEAR %s", instruct);
			break;
		};
		case 3:
		{
			char* instruction = GetOpcodeTableA_6(3, Reg_Opcode);
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);

			snprintf(buf, 128, "CALL FAR %s", instruction, instruct);
			break;
		};
		case 4:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "JUMP NEAR %s", instruct);
			break;
		};
		case 5:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "JUMP FAR %s", instruct);
			break;
		};
		case 6:
		{
			instruct = ModR_MandSIB_Ev(Entrypoint, Prefix, win32);
			snprintf(buf, 128, "PUSH %s", instruct);
			break;
		};
		case 7:
		{
			break;
		};

		};
		break;
	};
	default:
		break;
	}

	free(instruct);
	return buf;
};

char* OneByteOpcode(char* Entrypoint, char Prefix)
{
	char* buf = NULL;
	char Opcode = 0;		//总体8位
	char ColOpcode = 0;		//纵向4位
	char RowOpcode = 0;		//横向4位
	BOOL IsSuccess = 0;


	//判断入口点是否有效
	IsSuccess = SafeReadChar(Entrypoint);
	if (!IsSuccess)
	{
		return FALSE;
	};

	//获取opcode，并计算横向和纵向的值
	Opcode = *Entrypoint;
	RowOpcode = Opcode & 0xF;
	ColOpcode = (Opcode >> 4) & 0xF;


	//获取Mod指令长度
	CodeLen = GetModLen(Entrypoint, Prefix);

	//处理
	switch (ColOpcode)
	{
	case 0:
	{
		buf = ColOpcode_0(Entrypoint, 32, Prefix);
		break;
	};
	case 1:
	{
		buf = ColOpcode_1(Entrypoint, 32, Prefix);
		break;
	};
	case 2:
	{
		buf = ColOpcode_2(Entrypoint, 32, Prefix);
		break;
	};
	case 3:
	{
		buf = ColOpcode_3(Entrypoint, 32, Prefix);
		break;
	};
	case 4:
	{
		buf = ColOpcode_4(Entrypoint, 32, Prefix);
		break;
	};
	case 5:
	{
		buf = ColOpcode_5(Entrypoint, 32, Prefix);
		break;
	};
	case 6:
	{
		buf = ColOpcode_6(Entrypoint, 32, Prefix);
		break;
	};
	case 7:
	{
		buf = ColOpcode_7(Entrypoint, 32, Prefix);
		break;
	};
	case 8:
	{
		buf = ColOpcode_8(Entrypoint, 32, Prefix);
		break;
	};
	case 9:
	{
		buf = ColOpcode_9(Entrypoint, 32, Prefix);
		break;
	};
	case 0xa:
	{
		buf = ColOpcode_A(Entrypoint, 32, Prefix);
		break;
	};
	case 0xb:
	{
		buf = ColOpcode_B(Entrypoint, 32, Prefix);
		break;
	};
	case 0xc:
	{
		buf = ColOpcode_C(Entrypoint, 32, Prefix);
		break;
	};
	case 0xd:
	{
		buf = ColOpcode_D(Entrypoint, 32, Prefix);
		break;
	};
	case 0xe:
	{
		buf = ColOpcode_E(Entrypoint, 32, Prefix);
		break;
	};
	case 0xf:
	{
		buf = ColOpcode_F(Entrypoint, 32, Prefix);
		break;
	};
	default:
		break;
	}

	//有前缀，指令长度加1。
	if (Prefix)CodeLen++;

	//有前缀，不让外层调用输出空行
	if (buf[0]!=0)puts(buf);
	return buf;
};


//输入想要反汇编的代码和长度
void DisassemEngine(char*opcode, int len)
{
	while (len > 0)
	{
		OneByteOpcode(opcode, NULL);
		len -= CodeLen;
		opcode += CodeLen;
	};
	return;
};