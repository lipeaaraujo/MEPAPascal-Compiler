#include "header/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *currentTok;

void nextToken()
{
  if (currentTok != NULL)
    currentTok = currentTok->next;
}

// Checks if the current token matches the expected token type
void matchToken(TokenType expected)
{
  if (currentTok->tok->type == expected)
  {
    nextToken();
  }
  else
  {
    printf("erro: unexpected token %d\n", currentTok->tok->type);
    exit(1);
  }
}

// Checks if the current token lexeme matches the expected lexeme
void matchLexeme(char *expected)
{
  if (strcmp(currentTok->tok->lexeme, expected) == 0)
  {
    nextToken();
  }
  else
  {
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
    block();
    matchLexeme(".");
}

void block() {
    declarationList();
    statementList();
}

void declarationList() {
    while (currentTok->tok->type == KEYWORD && strcmp(currentTok->tok->lexeme, "var") == 0) {
        matchLexeme("var");
        identifierList();
        matchLexeme(":");
        type();
        matchLexeme(";");
    }
}

void identifierList() {
    matchToken(IDENTIFIER);
    while (currentTok->tok->type == DELIMITER && strcmp(currentTok->tok->lexeme, ",") == 0) {
        matchLexeme(",");
        matchToken(IDENTIFIER);
    }
}

void type() {
    if (currentTok->tok->type == KEYWORD) {
        if (strcmp(currentTok->tok->lexeme, "integer") == 0 || 
            strcmp(currentTok->tok->lexeme, "real") == 0 || 
            strcmp(currentTok->tok->lexeme, "boolean") == 0) {
            nextToken();
        } else {
            printf("erro: tipo inválido %s\n", currentTok->tok->lexeme);
            exit(1);
        }
    } else {
        printf("erro: esperado tipo, encontrado %s\n", currentTok->tok->lexeme);
        exit(1);
    }
}

void statementList() {
    statement();
    while (currentTok->tok->type == DELIMITER && strcmp(currentTok->tok->lexeme, ";") == 0) {
        matchLexeme(";");
        statement();
    }
}

void statement() {
    if (currentTok->tok->type == IDENTIFIER) {
        // assignment
        matchToken(IDENTIFIER);
        matchLexeme(":=");
        expression();
    } else if (currentTok->tok->type == KEYWORD && strcmp(currentTok->tok->lexeme, "if") == 0) {
        // if-statement
        ifStatement();
    } else if (currentTok->tok->type == KEYWORD && strcmp(currentTok->tok->lexeme, "while") == 0) {
        // while-statement
        whileStatement();
    } else if (currentTok->tok->type == KEYWORD && strcmp(currentTok->tok->lexeme, "write") == 0) {
        // write-statement
        writeStatement();
    } else if (currentTok->tok->type == KEYWORD && strcmp(currentTok->tok->lexeme, "read") == 0) {
        // read-statement
        readStatement();
    } else if (currentTok->tok->type == KEYWORD && strcmp(currentTok->tok->lexeme, "begin") == 0) {
        // begin-end-block
        beginEndBlock();
    } /*else if (currentTok->tok->type == KEYWORD && strcmp(currentTok->tok->lexeme, "end") == 0) {
        // begin-end-block
        beginEndBlock();
    }*/ else {
        printf("erro: declaração inválida %s\n", currentTok->tok->lexeme);
        exit(1);
    }
}

void ifStatement() {
    matchLexeme("if");
    expression();
    matchLexeme("then");
    statement();
    if (currentTok->tok->type == KEYWORD && strcmp(currentTok->tok->lexeme, "else") == 0) {
        matchLexeme("else");
        statement();
    }
}

void whileStatement() {
    matchLexeme("while");
    expression();
    matchLexeme("do");
    statement();
}

void writeStatement() {
    matchLexeme("write");
    matchLexeme("(");
    writeParameters();
    matchLexeme(")");
}

void writeParameters() {
    expression();
    while (currentTok->tok->type == DELIMITER && strcmp(currentTok->tok->lexeme, ",") == 0) {
        matchLexeme(",");
        expression();
    }
}

void readStatement() {
    matchLexeme("read");
    matchLexeme("(");
    identifierList();
    matchLexeme(")");
}

void beginEndBlock() {
    matchLexeme("begin");
    statementList();
    matchLexeme("end");
}

void expression() {
    term();
    while (currentTok->tok->type == OPERATOR && 
           (strcmp(currentTok->tok->lexeme, "+") == 0 || strcmp(currentTok->tok->lexeme, "-") == 0)) {
        nextToken();
        term();
    }
}

void term() {
    factor();
    while (currentTok->tok->type == OPERATOR && 
           (strcmp(currentTok->tok->lexeme, "*") == 0 || strcmp(currentTok->tok->lexeme, "/") == 0)) {
        nextToken();
        factor();
    }
}

void factor()
{
  if (currentTok->tok->type == IDENTIFIER)
  {
    matchToken(IDENTIFIER);
  }
  else if (currentTok->tok->type == NUMBER)
  {
    matchToken(NUMBER);
  }
  else if (currentTok->tok->type == DELIMITER && strcmp(currentTok->tok->lexeme, "(") == 0)
  {
    matchLexeme("(");
    expression();
    matchLexeme(")");
  }
  else
  {
    printf("erro: fator inválido %s\n", currentTok->tok->lexeme);
    exit(1);
  }
}

// Main parser function
void parser(Node *tokenList)
{
  currentTok = tokenList->next;
  program();

  if (currentTok == NULL)
  {
    printf("Aceito\n");
  }
  else
  {
    printf("Rejeito\n");
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