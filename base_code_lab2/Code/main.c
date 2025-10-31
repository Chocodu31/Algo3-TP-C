// Need this to use the getline C function on Linux. Works without this on MacOs. Not tested on Windows.
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>


#include "token.h"
#include "queue.h"
#include "stack.h"


/** 
 * Utilities function to print the token queues
 */
void print_token(const void* e, void* user_param);
void print_queue(FILE* f, Queue* q);

/** 
 * Function to be written by students
 */
bool isSymbol(char c) {
	return c=='+'||c=='-'||c=='*'||c=='/'||c=='^'||c=='('||c==')';
}

// J'ai pas réussi a utilisé la fonction de ctype.h
bool isDigit(char c) {
	return c=='0'||c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7'||c=='8'||c=='9';
}

Queue* stringToTokenQueue(const char* expression) {
	Queue* q = create_queue();
	const char* curpos = expression;
	while (*curpos != '\0') {
		while (*curpos == ' ' || *curpos == '\n' || *curpos == '\r') {
			curpos++;
		}
		if (*curpos == '\0') break;
		if (isSymbol(*curpos)) {
			// CAS SYMBOLE //
			Token* t = create_token_from_string(curpos,1);
			queue_push(q,t);
			curpos++;
		} else if (isDigit(*curpos)) {
			// CAS CHIFFRE //
			int i=1;
			while(isDigit(*(curpos+i))) {
				i++;
			}
			Token* t = create_token_from_string(curpos,i);
			queue_push(q,t);
			curpos+=i;
		} else {
			// AUTRE //
			curpos++;
		}
	}
	return q;
}

Queue* shuntingYard(Queue* infix) {
	Queue* output = create_queue();
	Stack* s = create_stack(queue_size(infix));
	while(!queue_empty(infix)) {
		Token* t = (Token*)queue_top(infix);
		if (token_is_number(t)) {
			queue_push(output,t);
		}
		else if (token_is_operator(t)) {
			while (!stack_empty(s) && ((token_operator_priority((Token *)stack_top(s)) >= token_operator_priority(t)) || (token_operator_priority((Token *)stack_top(s)) == token_operator_priority(t) && token_operator_leftAssociative(t))) && (token_parenthesis((Token *)stack_top(s)) != ')') ) {
				queue_push(output,stack_top(s));
				stack_pop(s);
			}
			stack_push(s,t);
		}
		else if (token_parenthesis(t) == '(') {
			stack_push(s,t);
		}
		else if (token_parenthesis(t) == ')') {
			while (!stack_empty(s) && (!token_is_parenthesis((Token *)stack_top(s)) && token_parenthesis((Token *)stack_top(s)) != '(')) {
				queue_push(output,stack_top(s));
				stack_pop(s);
			}
			if (stack_empty(s)) {
				break;
			}
			Token *tk = (Token *) stack_top(s);
			stack_pop(s);
			delete_token(&tk);
			delete_token(&t);
		}
		queue_pop(infix);

	}

	while (!stack_empty(s)) {
		if (token_is_parenthesis((Token *)stack_top(s))) {
			Token *tk = (Token *) stack_top(s);
			delete_token(&tk);
			stack_pop(s);
		} else {
			queue_push(output,stack_top(s));
			stack_pop(s);
		}
	}

	delete_stack(&s);

	return output;
}

Token* evaluateOperator(Token* arg1, Token* op, Token* arg2) {
	switch (token_operator(op)) {
		case '+':
			return create_token_from_value(token_value(arg1) + token_value(arg2));
		case '-':
			return create_token_from_value(token_value(arg1) - token_value(arg2));
		case '*':
			return create_token_from_value(token_value(arg1) * token_value(arg2));
		case '/':
			return create_token_from_value(token_value(arg1) / token_value(arg2));
		case '^':
			return create_token_from_value(pow(token_value(arg1), token_value(arg2)));
		default:
			return NULL;
	} 
}

float evaluateExpression(Queue* postfix){
	Stack* s = create_stack(queue_size(postfix));
	while(!queue_empty(postfix)) {
		Token* t = (Token *)queue_top(postfix);
		if (token_is_operator(t)) {
			Token* operand_2 = (Token *)stack_top(s);
			stack_pop(s);
			Token* operand_1 = (Token *)stack_top(s);
			stack_pop(s);
			stack_push(s,evaluateOperator(operand_1,t,operand_2));
			queue_pop(postfix);

			delete_token(&operand_1);
			delete_token(&operand_2);
			delete_token(&t); 
		} else if (token_is_number(t)) {
			stack_push(s,t);
			queue_pop(postfix);
		}
	}
	float result = token_value(stack_top(s));
	
	while (!stack_empty(s)) {
		Token* tk = (Token *)stack_top(s);
		stack_pop(s);
		delete_token(&tk);
	}
	
	delete_stack(&s);
	return result;
}

void computeExpressions(FILE* input) {
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, input)) != -1) {
		if(read > 1) {
			printf("Input : %s", line);
			printf("Infix : ");
			Queue* q = stringToTokenQueue(line);
			print_queue(stdout,q);
			printf("\n");
			printf("Postfix : ");
			Queue* postfix = shuntingYard(q);
			print_queue(stdout,postfix);
			printf("\n");
			printf("Evaluate : %f\n\n",evaluateExpression(postfix));

			// FREE QUEUE
			while (!queue_empty(postfix)) {
				Token* t = (Token*)queue_top(postfix);
				delete_token(&t);
				queue_pop(postfix);
			}
			delete_queue(&q);
			delete_queue(&postfix);
		}
	}

	// FREE LECTURE
    if (line) free(line);

}



/** Main function for testing.
 * The main function expects one parameter that is the file where expressions to translate are
 * to be read.
 *
 * This file must contain a valid expression on each line
 *
 */
int main(int argc, char** argv){
	if (argc<2) {
		fprintf(stderr,"usage : %s filename\n", argv[0]);
		return 1;
	}
	
	FILE* input = fopen(argv[1], "r");

	if ( !input ) {
		perror(argv[1]);
		return 1;
	}

	computeExpressions(input);

	fclose(input);
	return 0;
}
 
void print_token(const void* e, void* user_param) {
	FILE* f = (FILE*)user_param;
	Token* t = (Token*)e;
	token_dump(f, t);
}

void print_queue(FILE* f, Queue* q) {
	fprintf(f, "(%d) --  ", queue_size(q));
	queue_map(q, print_token, f);
}
