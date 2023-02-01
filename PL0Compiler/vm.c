// Daniel Solis
// COP3402 Fall 22
// 4412729
#include <stdio.h>
#include "compiler.h"

void print_stack(int PC, int BP, int SP, int GP, int *pas, int *bars)
{
	int i;
	printf("%d\t%d\t%d\t", PC, BP, SP);
	for (i = GP; i <= SP; i++)
	{
		if (bars[i] == 1)
			printf("| %d ", pas[i]);
		else
			printf("%d ", pas[i]);
	}
	printf("\n");
}

int base(int *pas, int BP, int L)
{
	int arb = BP;	// arb = activation record base
	while ( L > 0)     //find base L levels down
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}

void execute(int trace_flag, instruction *code)
{
	//initialize array and registers
	int PAS[10000];
	int BP = 0;
	int SP = 0;
	int PC = 0;
	int IR[3];
	int stkdis;
	int HALT = 1;
	int bars[500];
	
	//populate array with instructions
	int ctr = 0;
	int codectr = 0;
	while(code[codectr].op != -1)
	{
		PAS[ctr++] = code[codectr].op;
		SP++;
		PAS[ctr++] = code[codectr].l;
		SP++;
		PAS[ctr++] = code[codectr].m;
		SP++;
		codectr++;
	}
	
	stkdis = SP;
	BP = SP + 1;
	//program
	// prints original values of CPU registers
	if (trace_flag == 1)
	{
		printf("VM Execution: \n");
		printf("			PC	BP	SP	stack\n");
		printf("	Initial values:	%d	%d	%d\n", PC, BP, SP);
	}
	
	while(HALT == 1)
	{
		//fetch
		IR[0] = PAS[PC];//OP
		IR[1] = PAS[PC + 1];//L
		IR[2] = PAS[PC + 2];//M
		PC = PC + 3;
		
		//execute
		switch(IR[0])
		{
			case 1	://LIT
				SP++;
				PAS[SP] = IR[2];
				if (trace_flag == 1)
					printf("LIT	");
				break;
			case 2	://OPR
				switch(IR[2])
				{
					case 0://return
						SP = BP - 1;
						PC = PAS[SP+3];//cpu pc = RA
						BP = PAS[SP+2];//cpubp = DL
						if (trace_flag == 1)
							printf("RTN	");
						break;
						/* please ignore these, I originally did these based off of the lecture slides
							but didnt want to delete them for studying purposes
					case 1://negative top stack
						PAS[SP] = -PAS[SP];
						printf("NEG	");
						break;
						*/
					case 1://addition
						SP--;
						PAS[SP] = PAS[SP] + PAS[SP+1];
						if (trace_flag == 1)
							printf("ADD	");
						break;
					case 2://subtraction
						SP--;
						PAS[SP] = PAS[SP] - PAS[SP+1];
						if (trace_flag == 1)
							printf("SUB	");
						break;
					case 3://multiplication
						SP--;
						PAS[SP] = PAS[SP] * PAS[SP+1];
						if (trace_flag == 1)
							printf("MUL	");
						break;
					case 4://division
						SP--;
						//printf("%d", PAS[SP]);
						PAS[SP] = PAS[SP] / PAS[SP+1];
						if (trace_flag == 1)
							printf("DIV	");
						break;
						/*
					case 6://is odd
						PAS[SP] = PAS[SP] % 2;
						printf("ODD	");
						break;
					case 7://modulous
						SP--;
						PAS[SP] = PAS[SP] % PAS[SP+1];
						printf("MOD	");
						break;
						*/
					case 5://is equal
						SP--;
						PAS[SP] = PAS[SP] == PAS[SP+1];
						if (trace_flag == 1)
							printf("EQL	");
						break;
					case 6://not equal
						SP--;
						PAS[SP] = PAS[SP] != PAS[SP+1];
						if (trace_flag == 1)
							printf("NEQ	");
						break;
					case 7://less than
						SP--;
						PAS[SP] = PAS[SP] < PAS[SP+1];
						if (trace_flag == 1)
							printf("LSS	");
						break;
					case 8://less than or equal to
						SP--;
						PAS[SP] = PAS[SP] <= PAS[SP+1];
						if (trace_flag == 1)
							printf("LEQ	");
						break;
					case 9://greater than
						SP--;
						PAS[SP] = PAS[SP] > PAS[SP+1];
						if (trace_flag == 1)
							printf("GTR	");
						break;
					case 10://greater than or equal to
						SP--;
						PAS[SP] = PAS[SP] >= PAS[SP+1];
						if (trace_flag == 1)
							printf("GEQ	");
						break;
				}
				break;
			case 3	://LOD
				SP++;
				PAS[SP] = PAS[base(PAS, BP, IR[1])+IR[2]];
				if (trace_flag == 1)
					printf("LOD	");
				break;
			case 4	://STO
				PAS[base(PAS, BP, IR[1]) + IR[2]] = PAS[SP];
				SP--;
				if (trace_flag == 1)
					printf("STO	");
				break;
			case 5	://CAL
				PAS[SP + 1] = base(PAS, BP, IR[1]);//static link(SL)
				PAS[SP + 2] = BP;//dynamc link(DL)
				PAS[SP + 3] = PC;//return adress(RA)
				BP = SP + 1;
				PC = IR[2];
				bars[SP + 1] = 1;
				if (trace_flag == 1)
					printf("CAL	");
				break;
			case 6	://INC
				SP = SP + IR[2];
				if (trace_flag == 1)
					printf("INC	");
				break;
			case 7	://JMP
				PC = IR[2];
				if (trace_flag == 1)
					printf("JMP	");
				break;
			case 8	://JPC
				if(PAS[SP] == 0)
				{
					PC = IR[2];
				}
				SP--;
				if (trace_flag == 1)
					printf("JPC	");
				break;
			case 9	://SYS
				switch(IR[2])
				{
					case 1:
						printf("%d\n", PAS[SP]);
						if (trace_flag == 1)
							printf("WRT	");
						SP--; //*****
						break;
					case 2:
						printf("Please Enter an Integer: ");
						scanf("%d", &PAS[SP+1]);//store input on top of stack
						SP++;
						if (trace_flag == 1)
							printf("RED	");
						break;
					case 3:
						HALT = 0;//stop program
						if (trace_flag == 1)
							printf("HLT	");
						break;
				}
				break;
		}
		if (trace_flag == 1)
			printf("%d	%d	", IR[1], IR[2]);// print second part of instructions
		
		if (trace_flag == 1)
			print_stack(PC, BP, SP, stkdis + 1, PAS, bars);// print stack
		
	}
}