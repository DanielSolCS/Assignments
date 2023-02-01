// Daniel Solis
// COP3402 Fall 22
// 4412729
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define MAXPROGSIZE 10000
#define MAXLEXSIZE 10000

lexeme *lex_analyze(int list_flag, char *program)
{
	int fctr = strlen(program); // size of program
	//printf("%d****", fctr);
	int pctr = 0; // program array counter
	int lctr = 0; // lex array counter
	lexeme *lexarr = malloc(sizeof(lexeme) * MAXLEXSIZE); // storage of lexemes
	
	while (program[pctr] != '\0') // for entire length of file
	{
		//printf("%d: ", pctr);
		// increment through large sections of whitespace
		while (isspace(program[pctr]) || iscntrl(program[pctr]))
		{
			pctr++;
			continue;
		}
		
		// comment
		if (program[pctr] == '#')
		{
			// increment pointer past comment without saving any of it
			while(program[pctr] != '\n' && program[pctr] != '\0')
			{
				pctr++;
			}
			pctr++;
			continue;
		}
		
		// if detects a letter
		if (isalpha(program[pctr]))
		{
			int x = 0; // string counter
			lexeme temp; // initialize current lexeme
			temp.error_type = 0;
			
			while (isalpha(program[pctr]) || isdigit(program[pctr]))
			{	
				//printf("%c / %d\n", program[pctr], program[pctr]);
				// identifier length error
				if (x > 10)
				{
					temp.error_type = 1;
					printf("Lexical Analyzer Error: maximum identifier length is 11\n");
					return NULL;
					/*
					// increment till the end of the identifier
					while (isalpha(program[pctr]) || isdigit(program[pctr]))
					{
						pctr++;
					}
					
					lexarr[lctr] = temp;
					lctr++;
					break; // cut out to main while loop
					*/
				}
				// write characters to lexeme name
				temp.identifier_name[x] = program[pctr];
				x++;
				pctr++;
			}
			temp.identifier_name[x] = '\0';
			
			// identifier is null or main error
			if (!strcmp(temp.identifier_name, "null") || !strcmp(temp.identifier_name, "main"))
			{
				printf("Lexical Analyzer Error: identifiers cannot be named 'null' or 'main'\n");
				return NULL;
				/*
				printf("%s", temp.name);
				temp.error_type = 5;
				lexarr[lctr] = temp;
				lctr++;
				continue;
				*/
			}
			
			// string is a keyword check
			if (!strcmp(temp.identifier_name, "const")) 
				temp.type = keyword_const;
			else if (!strcmp(temp.identifier_name, "var")) 
				temp.type = keyword_var;
			else if (!strcmp(temp.identifier_name, "procedure")) 
				temp.type = keyword_procedure;
			else if (!strcmp(temp.identifier_name, "call")) 
				temp.type = keyword_call;
			else if (!strcmp(temp.identifier_name, "begin")) 
				temp.type = keyword_begin;
			else if (!strcmp(temp.identifier_name, "end")) 
				temp.type = keyword_end;
			else if (!strcmp(temp.identifier_name, "if")) 
				temp.type = keyword_if;
			else if (!strcmp(temp.identifier_name, "then")) 
				temp.type = keyword_then;
			else if (!strcmp(temp.identifier_name, "else")) 
				temp.type = keyword_else;
			else if (!strcmp(temp.identifier_name, "while")) 
				temp.type = keyword_while;
			else if (!strcmp(temp.identifier_name, "do")) 
				temp.type = keyword_do;
			else if (!strcmp(temp.identifier_name, "read")) 
				temp.type = keyword_read;
			else if (!strcmp(temp.identifier_name, "write")) 
				temp.type = keyword_write;
			else if (!strcmp(temp.identifier_name, "def")) 
				temp.type = keyword_def;
			else
				temp.type = identifier;
			
			if (temp.error_type == 0)
			{
				lexarr[lctr] = temp;
				lctr++;
			}
			continue;
		}
		
		if (isdigit(program[pctr]))
		{
			int x = 0; // number place counter
			lexeme temp;
			temp.error_type = 0;
			while(isdigit(program[pctr]) || isalpha(program[pctr]))
			{
				//printf("%c / %d\n", program[pctr], program[pctr]);
				// if theres a letter, number at beginning of identifier error
				if (isalpha(program[pctr]))
				{
					temp.error_type = 3;
					printf("Lexical Analyzer Error: identifiers cannot begin with digits\n");
					return NULL;
					/*
					// increment past identifier
					while (isalpha(program[pctr]) || isdigit(program[pctr]))
					{
						pctr++;
					}
					lexarr[lctr] = temp;
					lctr++;
					break;
					*/
				}
				
				// number larger that 5 digits error
				if (x > 4)
				{
					temp.error_type = 2;
					printf("Lexical Analyzer Error: maximum number length is 5\n");
					return NULL;
					/*
					// increment past large number
					while (isalpha(program[pctr]) || isdigit(program[pctr]))
					{
						pctr++;
					}
					lexarr[lctr] = temp;
					lctr++;
					break;
					*/
				}
				
				// copy character to name of lexeme
				temp.identifier_name[x] = program[pctr];
				x++;
				pctr++;
			}
			temp.identifier_name[x] = '\0';
			if (temp.error_type != 0)
			{
				continue;
			}
			// math to convert string of numbers to int value for lexeme
			int size = strlen(temp.identifier_name);
			//printf("size: %d", size);
			int power = 10;
			int sum = temp.identifier_name[size - 1] - 48;
			//printf("initial number of %s: %d -> %c\n", temp.identifier_name, temp.identifier_name[size - 1], temp.identifier_name[size - 1]);
			for (int y = size - 2; y >= 0; y--)
			{
				sum += (temp.identifier_name[y] - 48) * power;
				power = power * 10;
			}
			//printf("sum : %d\n", sum);
			temp.number_value = sum;
			temp.type = number;
			lexarr[lctr] = temp;
			lctr++;
			continue;
		}
		// reaches here if not number or identifier
		lexeme temp;
		temp.error_type = 0;
		
		//printf("%c\n", program[pctr]);
		
		//printf("%c / %d\n", program[pctr], program[pctr]);
		
		// test if lexeme is special character
		if (program[pctr] == '.')
		{
			temp.type = period;
			strcpy(temp.identifier_name, ".");
		}
		else if (program[pctr] == ':' && program[pctr + 1] == '=')
		{
			strcpy(temp.identifier_name, ":=");
			temp.type = assignment_symbol;
			pctr++;
		}
		else if (program[pctr] == '-')
		{
			temp.type = minus;
			strcpy(temp.identifier_name, "-");
		}
		else if (program[pctr] == ';')
		{
			temp.type = semicolon;
			strcpy(temp.identifier_name, ";");
		}
		else if (program[pctr] == '{')
		{
			temp.type = left_curly_brace;
			strcpy(temp.identifier_name, "{");
		}
		else if (program[pctr] == '}')
		{
			temp.type = right_curly_brace;
			strcpy(temp.identifier_name, "}");
		}
		else if (program[pctr] == '=' && program[pctr + 1] == '=')
		{
			strcpy(temp.identifier_name, "==");
			temp.type = equal_to;
			pctr++;
		}
		else if (program[pctr] == '<' && program[pctr + 1] == '>')
		{
			strcpy(temp.identifier_name, "<>");
			temp.type = not_equal_to;
			pctr++;
		}
		else if (program[pctr] == '<' && program[pctr + 1] != '=')
		{
			temp.type = less_than;
			strcpy(temp.identifier_name, "<");
		}
		else if (program[pctr] == '<' && program[pctr + 1] == '=')
		{
			strcpy(temp.identifier_name, "<=");
			temp.type = less_than_or_equal_to;
			pctr++;
		}
		else if (program[pctr] == '>' && program[pctr + 1] != '=')
		{
			temp.type = greater_than;
			strcpy(temp.identifier_name, ">");
		}
		else if (program[pctr] == '>' && program[pctr + 1] == '=')
		{
			strcpy(temp.identifier_name, ">=");
			temp.type = greater_than_or_equal_to;
			pctr++;
		}
		else if (program[pctr] == '+')
		{
			temp.type = plus;
			strcpy(temp.identifier_name, "+");
		}
		else if (program[pctr] == '*')
		{
			temp.type = times;
			strcpy(temp.identifier_name, "*");
		}
		else if (program[pctr] == '/')
		{
			temp.type = division;
			strcpy(temp.identifier_name, "/");
		}
		else if (program[pctr] == '(')
		{
			temp.type = left_parenthesis;
			strcpy(temp.identifier_name, "(");
		}
		else if (program[pctr] == ')')
		{
			temp.type = right_parenthesis;
			strcpy(temp.identifier_name, ")");
		}
		else // if not special character, is invalid character
		{
			//printf("%d %c***", pctr, program[pctr]);
			temp.identifier_name[0] = program[pctr];
			temp.error_type = 4;
			printf("Lexical Analyzer Error: invalid symbol\n");
			pctr++;
			return NULL;
		}
		
		lexarr[lctr] = temp;
		lctr++;
		pctr++;
	}
	
	lexarr[lctr].error_type = -1;
	if (list_flag == 1)
	{
		// printing the lexeme list
		printf("Lexeme List:\n");
		printf("lexeme		token type\n");
		int printctr = 0;
	
		// for length of lexeme array
		while(printctr < lctr)
		{
			// lexeme is not an error
			if (lexarr[printctr].error_type == 0)
			{
				printf("%12s	%d\n", lexarr[printctr].identifier_name, lexarr[printctr].type);
				printctr++;
				continue;
			}
			// checks which error type the lexeme is
			if (lexarr[printctr].error_type == 1)
			{
				printf("Lexical Analyzer Error: maximum identifier length is 11\n");
				printctr++;
				continue;
			}
			if (lexarr[printctr].error_type == 2)
			{
				printf("Lexical Analyzer Error: maximum number length is 5\n");
				printctr++;
				continue;
			}
			if (lexarr[printctr].error_type == 3)
			{
				printf("Lexical Analyzer Error: identifiers cannot begin with digits\n");
				printctr++;
				continue;
			}
			if (lexarr[printctr].error_type == 4)
			{
				printf("Lexical Analyzer Error: invalid symbol\n");
				printctr++;
				continue;
			}
			if (lexarr[printctr].error_type == 5)
			{
				printf("Lexical Analyzer Error: identifiers cannot be named 'null' or 'main'\n");
				printctr++;
				continue;
			}
			// lexeme is completely something unexpected
			printf("error, no output found for lexeme\n");
			printctr++;
			
		}
	}
	return lexarr;
}