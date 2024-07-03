#include "header/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *currentTok;

void nextToken() {
  if (currentTok != NULL)
    currentTok = currentTok->next;
}

// Checks if the current token matches the expected token type
void matchToken(TokenType expected) {
  if (currentTok->tok->type == expected) {
    nextToken();
  } else {
    printf("erro: unexpected token %d\n", currentTok->tok->type);
    exit(1);
  }
}

// Checks if the current token lexeme matches the expected lexeme
void matchLexeme(char *expected) {
  if (strcmp(currentTok->tok->lexeme, expected) == 0) {
    nextToken();
  } else {
    printf("erro: expected %s found %s\n", expected, currentTok->tok->lexeme);
    exit(1);
  }
}

// Parser functions
void program();
void block();
void declarationList();
void identifierList();
void type();
void statementList();
void statement();
void ifStatement();
void whileStatement();
void writeStatement();
void writeParameters();
void readStatement();
void beginEndBlock();
void expression();
void term();
void factor();

void program() {
  matchLexeme("program");
  matchToken(IDENTIFIER);
  matchLexeme(";");
}

// Main parser function
void parser(Node *tokenList) {
  currentTok = tokenList->next;
  program();

  if (currentTok == NULL) {
    printf("aceito\n");
  } else {
    printf("erro\n");
    exit(1);
  }
}

// Simplified Pascal Syntax
// program         -> 'program' identifier ';' block '.'

// block           -> declaration-list statement-list

// declaration-list -> ('var' identifier-list ':' type ';')*

// identifier-list -> identifier (',' identifier)*

// type            -> 'integer' | 'real' | 'boolean'

// statement-list  -> statement (';' statement)*

// statement       -> assignment | if-statement | while-statement | write-statement | read-statement | begin-end-block

// assignment      -> identifier ':=' expression

// if-statement    -> 'if' expression 'then' statement ('else' statement)?

// while-statement -> 'while' expression 'do' statement

// write-statement -> 'write' '(' write-parameters ')'

// write-parameters -> expression (',' expression)*

// read-statement  -> 'read' '(' identifier-list ')'

// begin-end-block -> 'begin' statement-list 'end'

// expression      -> term (('+' | '-') term)*

// term            -> factor (('' | '/') factor)

// factor          -> identifier | number | '(' expression ')'