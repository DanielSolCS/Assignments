// Daniel Solis
// COP3402 Fall 22
// 4412729
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define ARRAY_SIZE 500

// grammer
void program();
void block();
int declarations();
void constants();
void var(int numVar);
void proc();
void statement();
void factor();
void condition();
void expression();
void term();

lexeme *tokens;
int token_index = 0;
symbol *table;
int table_index = 0;
instruction *code;
int code_index = 0;

int error = 0;
int level;

void emit(int op, int l, int m);
void add_symbol(int kind, char name[], int value, int level, int address);
void mark();
int multiple_declaration_check(char name[]);
int find_symbol(char name[], int kind);

void print_parser_error(int error_code, int case_code);
void print_assembly_code();
void print_symbol_table();

instruction *parse(int code_flag, int table_flag, lexeme *list)
{
	// variable setup
	int i;
	tokens = list;
	table = calloc(ARRAY_SIZE, sizeof(symbol));
	code = calloc(ARRAY_SIZE, sizeof(instruction));
	int buffer;
	
	/*
	for(int i = 0; i < 91; i++)
	{
		printf("%d %s %d %d\n", tokens[i].type, tokens[i].identifier_name, tokens[i].number_value, tokens[i].error_type);
	}
	*/
	
	
	// YOUR CODE HERE
	program();
	if (error == 1)
	{
		return NULL;
	}
	
	if (code_flag == 1)
	{
		printf("\n");
		print_assembly_code();
	}
		
	if (table_flag == 1)
	{
		print_symbol_table();	
	}
	
	emit(-1, 0, 0);
	
	free(table);
	return code;
	
}

// adds a new instruction to the end of the code
void emit(int op, int l, int m)
{
	code[code_index].op = op;
	code[code_index].l = l;
	code[code_index].m = m;
	code_index++;
}

// adds a new symbol to the end of the table
void add_symbol(int kind, char name[], int value, int level, int address)
{
	table[table_index].kind = kind;
	strcpy(table[table_index].name, name);
	table[table_index].value = value;
	table[table_index].level = level;
	table[table_index].address = address;
	table[table_index].mark = 0;
	table_index++;
}

// marks all of the current procedure's symbols
void mark()
{
	int i;
	for (i = table_index - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

// returns -1 if there are no other symbols with the same name within this procedure
int multiple_declaration_check(char name[])
{
	int i;
	for (i = 0; i < table_index; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

// returns the index of the symbol with the desired name and kind, prioritizing 
// 		symbols with level closer to the current level
int find_symbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < table_index; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void print_parser_error(int error_code, int case_code)
{
	switch (error_code)
	{
		case 1 :
			printf("Parser Error 1: missing . \n");
			break;
		case 2 :
			switch (case_code)
			{
				case 1 :
					printf("Parser Error 2: missing identifier after keyword const\n");
					break;
				case 2 :
					printf("Parser Error 2: missing identifier after keyword var\n");
					break;
				case 3 :
					printf("Parser Error 2: missing identifier after keyword procedure\n");
					break;
				case 4 :
					printf("Parser Error 2: missing identifier after keyword call\n");
					break;
				case 5 :
					printf("Parser Error 2: missing identifier after keyword read\n");
					break;
				case 6 :
					printf("Parser Error 2: missing identifier after keyword def\n");
					break;
				default :
					printf("Implementation Error: unrecognized error code\n");
			}
			break;
		case 3 :
			printf("Parser Error 3: identifier is declared multiple times by a procedure\n");
			break;
		case 4 :
			switch (case_code)
			{
				case 1 :
					printf("Parser Error 4: missing := in constant declaration\n");
					break;
				case 2 :
					printf("Parser Error 4: missing := in assignment statement\n");
					break;
				default :				
					printf("Implementation Error: unrecognized error code\n");
			}
			break;
		case 5 :
			printf("Parser Error 5: missing number in constant declaration\n");
			break;
		case 6 :
			switch (case_code)
			{
				case 1 :
					printf("Parser Error 6: missing ; after constant declaration\n");
					break;
				case 2 :
					printf("Parser Error 6: missing ; after variable declaration\n");
					break;
				case 3 :
					printf("Parser Error 6: missing ; after statement in begin-end\n");
					break;
				default :				
					printf("Implementation Error: unrecognized error code\n");
			}
			break;
		case 7 :
			printf("Parser Error 7: procedures and constants cannot be assigned to\n");
			break;
		case 8 :
			switch (case_code)
			{
				case 1 :
					printf("Parser Error 8: undeclared identifier used in assignment statement\n");
					break;
				case 2 :
					printf("Parser Error 8: undeclared identifier used in call statement\n");
					break;
				case 3 :
					printf("Parser Error 8: undeclared identifier used in read statement\n");
					break;
				case 4 :
					printf("Parser Error 8: undeclared identifier used in arithmetic expression\n");
					break;
				default :				
					printf("Implementation Error: unrecognized error code\n");
			}
			break;
		case 9 :
			printf("Parser Error 9: variables and constants cannot be called\n");
			break;
		case 10 :
			printf("Parser Error 10: begin must be followed by end\n");
			break;
		case 11 :
			printf("Parser Error 11: if must be followed by then\n");
			break;
		case 12 :
			printf("Parser Error 12: while must be followed by do\n");
			break;
		case 13 :
			printf("Parser Error 13: procedures and constants cannot be read\n");
			break;
		case 14 :
			printf("Parser Error 14: missing {\n");
			break;
		case 15 :
			printf("Parser Error 15: { must be followed by }\n");
			break;
		case 16 :
			printf("Parser Error 16: missing relational operator\n");
			break;
		case 17 :
			printf("Parser Error 17: procedures cannot be used in arithmetic\n");
			break;
		case 18 :
			printf("Parser Error 18: ( must be followed by )\n");
			break;
		case 19 :
			printf("Parser Error 19: invalid expression\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");

	}
}

void print_assembly_code()
{
	int i;
	printf("Assembly Code:\n");
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < code_index; i++)
	{
		printf("%d\t%d\t", i, code[i].op);
		switch(code[i].op)
		{
			case LIT :
				printf("LIT\t");
				break;
			case OPR :
				switch (code[i].m)
				{
					case RTN :
						printf("RTN\t");
						break;
					case ADD : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("ADD\t");
						break;
					case SUB : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("SUB\t");
						break;
					case MUL : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("MUL\t");
						break;
					case DIV : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("DIV\t");
						break;
					case EQL : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("EQL\t");
						break;
					case NEQ : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("NEQ\t");
						break;
					case LSS : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("LSS\t");
						break;
					case LEQ : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("LEQ\t");
						break;
					case GTR : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("GTR\t");
						break;
					case GEQ : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("GEQ\t");
						break;
					default :
						printf("err\t");
						break;
				}
				break;
			case LOD :
				printf("LOD\t");
				break;
			case STO :
				printf("STO\t");
				break;
			case CAL :
				printf("CAL\t");
				break;
			case INC :
				printf("INC\t");
				break;
			case JMP :
				printf("JMP\t");
				break;
			case JPC : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
				printf("JPC\t");
				break;
			case SYS :
				switch (code[i].m)
				{
					case WRT : // DO NOT ATTEMPT TO IMPLEMENT THIS, YOU WILL GET A ZERO IF YOU DO
						printf("WRT\t");
						break;
					case RED :
						printf("RED\t");
						break;
					case HLT :
						printf("HLT\t");
						break;
					default :
						printf("err\t");
						break;
				}
				break;
			default :
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	printf("\n");
}

void print_symbol_table()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < table_index; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].value, table[i].level, table[i].address, table[i].mark); 
	printf("\n");
}

void program()
{
	add_symbol(3, "main", 0, 0, 0);
	level = -1;
	emit(7, 0, 0);
	block();
	if (error == 1)
		return;
	if (tokens[token_index].type != period)
	{
		print_parser_error(1, 0);
		error = 1;
		return;
	}
	emit(9, 0, 3);
	
	code[0].m = table[0].address;
	
	int temp;
	for (int y = 0; y < code_index; y++)
	{
		if (code[y].op == 5)
		{
			temp = code[y].m;
			code[y].m = table[temp].address;
		}
	}
}
void block()
{
	int curproc = table_index - 1;
	level++;
	int inc_m_value = declarations() + 3;
	if (error == 1)
		return;
	proc();
	if (error == 1)
		return;
	table[curproc].address = code_index * 3;
	
	emit(6, 0, inc_m_value);
	statement();
	if (error == 1)
		return;
	mark();
	level--;
}
int declarations()
{
	int numVar = 0;
	while (tokens[token_index].type == keyword_const || tokens[token_index].type == keyword_var)
	{
		if(tokens[token_index].type == keyword_const)
		{
			constants();
			if (error == 1)
				return -1;
		}
		else
		{
			var(numVar);
			if (error == 1)
				return -1;
			numVar++;
		}
	}
	return numVar;
	
}
void constants()
{
	int minus_flag = 0;
	token_index++;
	if (tokens[token_index].type != identifier)
	{
		print_parser_error(2, 1);
		error = 1;
		return;
	}
	if (multiple_declaration_check(tokens[token_index].identifier_name) != -1)
	{
		print_parser_error(3, 0);
		error = 1;
		return;
	}
	
	char name[12];
	strcpy(name, tokens[token_index].identifier_name);
	token_index++;
	
	if (tokens[token_index].type != assignment_symbol)
	{
		print_parser_error(4, 1);
		error = 1;
		return;
	}
	
	token_index++;
	if (tokens[token_index].type == minus)
	{
		minus_flag = 1;
		token_index++;
	}
	if (tokens[token_index].type != number)
	{
		print_parser_error(5, 0);
		error = 1;
		return;
	}
	
	int value = tokens[token_index].number_value;
	token_index++;
	if (minus_flag)
	{
		value *= -1;
	}
	
	if (tokens[token_index].type != semicolon)
	{
		print_parser_error(6, 1);
		error = 1;
		return;
	}
	
	add_symbol(1, name, value, level, 0);
	token_index++;
}

void var(int numVar)
{
	token_index++;
	if (tokens[token_index].type != identifier)
	{
		print_parser_error(2, 2);
		error = 1;
		return;
	}
	if (multiple_declaration_check(tokens[token_index].identifier_name) != -1)
	{
		print_parser_error(3, 0);
		error = 1;
		return;
	}

	char name[12];
	strcpy(name, tokens[token_index].identifier_name);
	token_index++;
	
	add_symbol(2, name, 0, level, numVar + 3);
	
	if (tokens[token_index].type != semicolon)
	{
		print_parser_error(6, 2);
		error = 1;
		return;
	}
	token_index++;
}
void proc()
{
	while (tokens[token_index].type == keyword_procedure)
	{
		token_index++;
		if (tokens[token_index].type != identifier)
		{
			print_parser_error(2, 3);
			error = 1;
			return;
		}
		if (multiple_declaration_check(tokens[token_index].identifier_name) != -1)
		{
			print_parser_error(3, 0);
			error = 1;
			return;
		}
		
		char name[12];
		strcpy(name, tokens[token_index].identifier_name);
		token_index++;
		
		add_symbol(3, name, 0, level, 0);
		
		if (tokens[token_index].type != left_curly_brace)
		{
			print_parser_error(14, 0);
			error = 1;
			return;
		}
		token_index++;
		block();
		if (error != 0)
			return;
		emit(2, 0, 0);
		if (tokens[token_index].type != right_curly_brace)
		{
			print_parser_error(15, 0);
			error = 1;
			return;
		}
		token_index++;
	}
}

void statement()
{
	if (tokens[token_index].type == keyword_def)
	{
		token_index++;
		if (tokens[token_index].type != identifier)
		{
			print_parser_error(2, 6);
			error = 1;
			return;
		}
		int symIndex = find_symbol(tokens[token_index].identifier_name, 2);
		if (symIndex == -1)
		{
			if (find_symbol(tokens[token_index].identifier_name, 1) == find_symbol(tokens[token_index].identifier_name, 3))
			{
				print_parser_error(8, 1);
				error = 1;
				return;
			}
			else
			{
				print_parser_error(7, 0);
				error = 1;
				return;
			}
		}
		
		token_index++;
		if (tokens[token_index].type != assignment_symbol)
		{
			print_parser_error(4, 2);
			error = 1;
			return;
		}
		
		token_index++;
		expression();
		if (error != 0)
			return;
		
		emit(4, level - table[symIndex].level, table[symIndex].address);
	}
	else if (tokens[token_index].type == keyword_call)
	{
		token_index++;
		if (tokens[token_index].type != identifier)
		{
			print_parser_error(2, 4);
			error = 1;
			return;
		}
		int symIndex = find_symbol(tokens[token_index].identifier_name, 3);
		if (symIndex == -1)
		{
			if (find_symbol(tokens[token_index].identifier_name, 1) == find_symbol(tokens[token_index].identifier_name, 2))
			{
				print_parser_error(8, 2);
				error = 1;
				return;
			}
			else
			{
				print_parser_error(9, 0);
				error = 1;
				return;
			}
		}
		
		token_index++;
		emit(5, level - table[symIndex].level, symIndex);
	}
	else if (tokens[token_index].type == keyword_begin)
	{
		do
		{
			token_index++;
			statement();
			if (error != 0)
				return;
		} while (tokens[token_index].type == semicolon);
			
		if (tokens[token_index].type != keyword_end)
		{
			if (tokens[token_index].type == identifier || tokens[token_index].type == keyword_call || tokens[token_index].type == keyword_begin || tokens[token_index].type == keyword_read || tokens[token_index].type == keyword_def || tokens[token_index].type == keyword_if || tokens[token_index].type == keyword_while || tokens[token_index].type == keyword_write)
			{
				print_parser_error(6, 3);
				error = 1;
				return;
			}
			else
			{
				print_parser_error(10, 0);
				error = 1;
				return;
			}
		}
		token_index++;
	}
	else if (tokens[token_index].type == keyword_read)
	{
		token_index++;
		if (tokens[token_index].type != identifier)
		{
			print_parser_error(2, 5);
			error = 1;
			return;
		}
		int symIndex = find_symbol(tokens[token_index].identifier_name, 2);
		if (symIndex == -1)
		{
			if (find_symbol(tokens[token_index].identifier_name, 1) == find_symbol(tokens[token_index].identifier_name, 3))
			{
				print_parser_error(8, 3);
				error = 1;
				return;
			}
			else
			{
				print_parser_error(13, 0);
				error = 1;
				return;
			}
		}
		
		token_index++;
		emit(9, 0, 2);
		emit(4, level - table[symIndex].level, table[symIndex].address);
	}
	else if (tokens[token_index].type == keyword_if)
	{
		token_index++;
		condition();
		if (error == 1)
			return;
		int jpc_index = code_index;
		emit(8, 0, 0);
		if (tokens[token_index].type != keyword_then)
		{
			print_parser_error(11, 0);
			error = 1;
			return;
		}	
		token_index++;
		statement();
		if (error == 1)
			return;
		if (tokens[token_index].type == keyword_else)
		{
			token_index++;
			int jmp_index = code_index;
			emit(7, 0, 0);
			code[jpc_index].m = code_index * 3;
			statement();
			if (error == 1)
				return;
			code[jmp_index].m = code_index * 3;
		}
		else
		{
			code[jpc_index].m = code_index * 3;
		}
	}
	else if (tokens[token_index].type == keyword_while)
	{
		token_index++;
		int cond_index = code_index;
		condition();
		if (error == 1)
			return;
		if (tokens[token_index].type != keyword_do)
		{
			print_parser_error(12, 0);
			error = 1;
			return;
		}
		token_index++;
		int while_jpc_index = code_index;
		emit(8, 0, 0);
		statement();
		if (error == 1)
			return;
		emit(7, 0, cond_index * 3);
		code[while_jpc_index].m = code_index * 3;
	}
	else if (tokens[token_index].type == keyword_write)
	{
		token_index++;
		expression();
		if (error == 1)
			return;
		emit(9, 0, 1);
	}
}

void factor()
{
	if (tokens[token_index].type == identifier)
	{
		int const_index = find_symbol(tokens[token_index].identifier_name, 1);
		int var_index = find_symbol(tokens[token_index].identifier_name, 2);
		
		if (const_index == var_index)
		{
			if (find_symbol(tokens[token_index].identifier_name, 3) != -1)
			{
				print_parser_error(17, 0);
				error = 1;
				return;
			}
			else
			{
				print_parser_error(8, 4);
				error = 1;
				return;
			}
		}
		if (const_index == -1)
		{
			emit(3, level - table[var_index].level, table[var_index].address);
		}
		else if (var_index == -1)
		{
			emit(1, 0, table[const_index].value);
		}
		else if (table[const_index].level > table[var_index].level)
		{
			emit(1, 0, table[const_index].value);
		}
		else
		{
			emit(3, level - table[var_index].level, table[var_index].address);
		}
		
		token_index++;
		
	}
	else if (tokens[token_index].type == number)
	{
		emit(1, 0, tokens[token_index].number_value);
		token_index++;
	}
	else if (tokens[token_index].type == left_parenthesis)
	{
		token_index++;
		expression();
		if (error == 1)
				return;
		if (tokens[token_index].type != right_parenthesis)
		{
			print_parser_error(0, 18);
			error = 1;
			return;
		}
		token_index++;
	}
	else
	{
		print_parser_error(19, 0);
		error = 1;
		return;
	}
}

void condition()
{
	expression();
	if (error == 1)
		return;
	if (tokens[token_index].type == equal_to)
	{
		token_index++;
		expression();
		if (error == 1)
			return;
		emit(2, 0, 5);
	}
	else if (tokens[token_index].type == not_equal_to)
	{
		token_index++;
		expression();
		if (error == 1)
			return;
		emit(2, 0, 6);
	}
	else if (tokens[token_index].type == less_than)
	{
		token_index++;
		expression();
		if (error == 1)
			return;
		emit(2, 0, 7);
	}
	else if (tokens[token_index].type == less_than_or_equal_to)
	{
		token_index++;
		expression();
		if (error == 1)
			return;
		emit(2, 0, 8);
	}
	else if (tokens[token_index].type == greater_than)
	{
		token_index++;
		expression();
		if (error == 1)
			return;
		emit(2, 0, 9);
	}
	else if (tokens[token_index].type == greater_than_or_equal_to)
	{
		token_index++;
		expression();
		if (error == 1)
			return;
		emit(2, 0, 10);
	}
	else
	{
		print_parser_error(0, 16);
		error = 1;
		return;
	}
	
}

void expression()
{
	term();
	if (error == 1)
		return;
	while (tokens[token_index].type == plus || tokens[token_index].type == minus)
	{
		if (tokens[token_index].type == plus)
		{
			token_index++;
			term();
			if (error == 1)
				return;
			emit(2, 0, 1);
		}
		else if (tokens[token_index].type == minus)
		{
			token_index++;
			term();
			if (error == 1)
				return;
			emit(2, 0, 2);
		}
	}
}

void term()
{
	factor();
	if (error == 1)
		return;
	while (tokens[token_index].type == times || tokens[token_index].type == division)
	{
		if (tokens[token_index].type == times)
		{
			token_index++;
			factor();
			if (error == 1)
				return;
			emit(2, 0, 3);
		}
		else if (tokens[token_index].type == division)
		{
			token_index++;
			factor();
			if (error == 1)
				return;
			emit(2, 0, 4);
		}
	}
	
}