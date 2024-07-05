#include "header/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *currentTok;

// Moves to the next token in the list
void nextToken() {
  if (currentTok != NULL) {
    currentTok = currentTok->next;
  }
}

// Checks if current token type is the expected type
int checkToken(TokenType expected) {
  return currentTok->tok->type == expected;
}

// Checks if current lexeme is the expected lexeme
int checkLexeme(TokenType tokType, char *expected) {
  return checkToken(tokType) &&
         (strcmp(currentTok->tok->lexeme, expected) == 0);
}

// Moves to the next token if current token type matches expected
void matchToken(TokenType expected) {
  if (checkToken(expected)) {
    nextToken();
  } else {
    fprintf(stderr, "error: unexpected token type %d\n", currentTok->tok->type);
    exit(1);
  }
}

// Moves to the next token if current lexeme matches expected
void matchLexeme(TokenType tokType, char *expected) {
  if (checkLexeme(tokType, expected)) {
    nextToken();
  } else {
    fprintf(stderr, "error: expected \"%s\" found \"%s\" at line %d\n",
           expected, currentTok->tok->lexeme, currentTok->tok->line);
    exit(1);
  }
}

// Parser functions
void program(); // edit
void block(); // edit
void labelDeclaration(); // implement
void varDeclaration(); // edit
void identifierList(); // edit
void type(); // edit
void subroutines(); // implement
void procedure(); // implement
void function(); // implement
void params(); // implement
void statementList(); // edit
void statement(); // edit
void assignment(); // implement
void subroutineCall(); // implement
void deviation(); // implement
void ifStatement(); // edit
void whileStatement(); // edit
void writeStatement(); // edit
void readStatement(); // edit
void expressionList(); // implement
void simpleExpression(); // edit
void term(); // edit
void factor(); // edit

void program() {
  matchLexeme(KEYWORD, "program");
  matchToken(IDENTIFIER);
  matchLexeme(DELIMITER, ";");
  block();
  matchLexeme(DELIMITER, ".");
}

void block() {
  declarationList();
  statementList();
}

void varDeclaration() {
  while (checkLexeme(KEYWORD, "var")) {
    matchLexeme(KEYWORD, "var");
    identifierList();
    matchLexeme(DELIMITER, ":");
    type();
    matchLexeme(DELIMITER, ";");
  }
}

void identifierList() {
  matchToken(IDENTIFIER);
  while (checkLexeme(DELIMITER, ",")) {
    matchLexeme(DELIMITER, ",");
    matchToken(IDENTIFIER);
  }
}

void type() {
  if (!checkToken(IDENTIFIER)) {
    printf("error: unexpected token %d\n", currentTok->tok->type);
    exit(1);
  }

  if (checkLexeme(IDENTIFIER, "integer") ||
      checkLexeme(IDENTIFIER, "real") == 0 ||
      checkLexeme(IDENTIFIER, "boolean")) {
    matchToken(IDENTIFIER);
  } else {
    printf("error: invalid type %s\n", currentTok->tok->lexeme);
    exit(1);
  }
}

void statementList() {
  statement();
  while (checkLexeme(DELIMITER, ";")) {
    matchLexeme(DELIMITER, ";");
    statement();
  }
}

void statement() {
  if (checkToken(IDENTIFIER)) {
    // assignment
    matchToken(IDENTIFIER);
    matchLexeme(COMPOUND_OPERATOR, ":=");
    expression();
  } 
  else if (checkLexeme(KEYWORD, "if")) ifStatement();
  else if (checkLexeme(KEYWORD, "while")) whileStatement();
  else if (checkLexeme(KEYWORD, "write")) writeStatement();
  else if (checkLexeme(KEYWORD, "read")) readStatement();
  else if (checkLexeme(KEYWORD, "begin")) beginEndBlock();
  else if (checkLexeme(KEYWORD, "end"));
  else {
    printf("error: expected statement, found %s\n", currentTok->tok->lexeme);
    exit(1);
  }
}

void ifStatement() {
  matchLexeme(KEYWORD, "if");
  expression();
  matchLexeme(KEYWORD, "then");
  statement();
  if (checkLexeme(KEYWORD, "else")) {
    matchLexeme(KEYWORD, "else");
    statement();
  }
}

void whileStatement() {
  matchLexeme(KEYWORD, "while");
  expression();
  matchLexeme(KEYWORD, "do");
  statement();
}

void writeStatement() {
  matchLexeme(KEYWORD, "write");
  matchLexeme(DELIMITER, "(");
  writeParameters();
  matchLexeme(DELIMITER, ")");
}

void writeParameters() {
  expression();
  while (checkLexeme(DELIMITER, ",")) {
    matchLexeme(DELIMITER, ",");
    expression();
  }
}

void readStatement() {
  matchLexeme(KEYWORD, "read");
  matchLexeme(DELIMITER, "(");
  identifierList();
  matchLexeme(DELIMITER, ")");
}

void beginEndBlock() {
  matchLexeme(KEYWORD, "begin");
  statementList();
  matchLexeme(KEYWORD, "end");
}

void simpleExpression() {
  term();
  while (checkLexeme(OPERATOR, "+") ||
         checkLexeme(OPERATOR, "-")) {
    nextToken();
    term();
  }
}

void term() {
  factor();
  while (checkLexeme(OPERATOR, "*") ||
         checkLexeme(OPERATOR, "/")) {
    nextToken();
    factor();
  }
}

void factor() {
  if (checkToken(IDENTIFIER)) {
    matchToken(IDENTIFIER);
  } else if (checkToken(NUMBER)) {
    matchToken(NUMBER);
  } else if (checkLexeme(DELIMITER, "(")) {
    matchLexeme(DELIMITER, "(");
    expression();
    matchLexeme(DELIMITER, ")");
  } else {
    printf("error: invalid factor %s\n", currentTok->tok->lexeme);
    exit(1);
  }
}

// Main parser function
void parser(Node *tokenList) {
  currentTok = tokenList->next;
  program();

  if (currentTok == NULL) {
    printf("Aceito\n");
  } else {
    printf("Rejeito\n");
    exit(1);
  }
}

// syntax:
// <program>                 -> 'program' <IDENTIFIER> '(' <identifier-list> ')' ';' <block> '.'

// <block>                   -> [<label-declaration>] [<var-declaration>]
//                              [<subroutines>] <statement-list>

// <label-declaration>       -> 'label' <NUMBER> (',' <NUMBER>)* ';'

// <var-declaration>         -> 'var' <identifier-list> ':' <type>
//                              (';' <identifier-list> ':' <type>)* ';'

// <identifier-list>         -> <IDENTIFIER> (',' <IDENTIFIER>)*

// <type>                    -> 'integer' | 'real' | 'boolean'

// <subroutines>             -> (<procedure> | <function>)*

// <procedure>               -> 'procedure' <IDENTIFIER> [<params>] ';' <block>

// <function>                -> 'function' <IDENTIFIER> [<params>] ';' <block>

// <params>                  -> [var] <identifier-list> : <IDENTIFIER> 
//                              ([var] <identifier-list> : <IDENTIFIER>)

// <statement-list>          -> 'begin' <statement> (';' <statement>) 'end'

// <statement>               -> [<NUMBER> ':'] <assignment> | <procedure-call> | <deviation> |
//                              <statement-list> | <if-statement> | <while-statement> |
//                              <write-statement> | <read-statement>

// <assignment>              -> <IDENTIFIER> ':=' <expression>

// <subroutine-call>          -> <IDENTIFIER> ['(' <expression-list> ')']

// <deviation>               -> 'goto' <NUMBER>

// <if-statement>            -> 'if' <expression> 'then' <statement> ('else' <statement>)?

// <while-statement>         -> 'while' <expression> 'do' <statement>

// <write-statement>         -> 'write' '(' <expression-list> ')'

// <read-statement>          -> 'read' '(' <identifier-list> ')'

// <expression-list>         -> <expression> (',' <expression>)*

// <expression>              -> <simple-expression> [<relation> <simple-expression>]

// <relation>                -> '=' | '<>' | '<' | '<=' | '>=' | '>'

// <simple-expression>       -> ['+' | '-'] <term> (('+' | '-' | 'or') <term>)*

// <term>                    -> <factor> (('*' | '/' | 'div' | 'and') <factor>)

// <factor>                  -> identifier | number | <subroutine-call> | '(' expression ')' |
//                              'not' <factor>