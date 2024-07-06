#include "header/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum ErrorType {
  UNEXPECTED_TYPE,
  UNEXPECTED_LEXEME,
  INVALID_TYPE,
  INVALID_STATEMENT,
  INVALID_FACTOR,
  INVALID_END
} ErrorType;

Node *currentTok;

// Handles a error based on it's error type
void handleError(TokenType expectedType, char *expectedLexeme,
                 ErrorType error) {
  char *types[] = {
    "keyword", "identifier", "number", "operator", 
    "compound_operator", "delimiter", "comments", "unknown"
  };

  switch (error) {
    case UNEXPECTED_TYPE:
      fprintf(stderr, "Error: expected type %s but found %s at line %d\n",
              types[expectedType], types[currentTok->tok->type], currentTok->tok->line);
      break;
    case UNEXPECTED_LEXEME:
      fprintf(stderr, "Error: expected \"%s\" but found \"%s\" at line %d\n",
              expectedLexeme, currentTok->tok->lexeme, currentTok->tok->line);
      break;
    case INVALID_TYPE:
      fprintf(stderr, "Error: expected valid type but found \"%s\" at line %d\n",
              currentTok->tok->lexeme, currentTok->tok->line);
      break;
    case INVALID_STATEMENT:
      fprintf(stderr, "Error: expected valid statement but found \"%s\" at line %d\n",
              currentTok->tok->lexeme, currentTok->tok->line);
      break;
    case INVALID_FACTOR:
      fprintf(stderr, "Error: expected valid factor but found \"%s\" at line %d\n",
              currentTok->tok->lexeme, currentTok->tok->line);
      break;
    case INVALID_END:
      fprintf(stderr, "Error: unexpected token after end of file \"%s\"",
              currentTok->tok->lexeme);
      break;
    default:
      fprintf(stderr, "Error: unknown error");
      break;
  }

  printf("Rejeito\n");
  exit(1);
}

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
    handleError(expected, "", UNEXPECTED_TYPE);
  }
}

// Moves to the next token if current lexeme matches expected
void matchLexeme(TokenType tokType, char *expected) {
  if (checkLexeme(tokType, expected)) {
    nextToken();
  } else {
    handleError(tokType, expected, UNEXPECTED_LEXEME);
  }
}

// Checks if the token ahead is of expected type
int lookaheadToken(TokenType expected) {
  if (currentTok->next == NULL) return 0;  
  return currentTok->next->tok->type == expected;
}

// Checks if the lexeme of the token ahead is the expected lexeme
int lookaheadLexeme(TokenType tokType, char *expected) {
  if (currentTok->next == NULL) return 0;
  return lookaheadToken(tokType) &&
         (strcmp(currentTok->next->tok->lexeme, expected) == 0);
}

// Parser functions
void program();
void block();
void labelDeclaration();
void varDeclaration();
void identifierList();
void type();
void subroutines();
void procedure();
void function();
void params();
void statementList();
void statement();
void assignment();
void subroutineCall();
void deviation();
void ifStatement();
void whileStatement();
void writeStatement();
void readStatement();
void expressionList();
void expression();
void relation();
void simpleExpression();
void term();
void factor();

void program() {
  matchLexeme(KEYWORD, "program");
  matchToken(IDENTIFIER);
  if (checkLexeme(DELIMITER, "(")) {
    matchLexeme(DELIMITER, "(");
    identifierList();
    matchLexeme(DELIMITER, ")");    
  }
  matchLexeme(DELIMITER, ";");
  block();
  matchLexeme(DELIMITER, ".");
}

void block() {
  if (checkLexeme(KEYWORD, "label")) labelDeclaration();
  if (checkLexeme(KEYWORD, "var")) varDeclaration();
  if (checkLexeme(KEYWORD, "procedure") ||
      checkLexeme(KEYWORD, "function")) subroutines();
  statementList();
}

void labelDeclaration() {
  matchLexeme(KEYWORD, "label");
  matchToken(NUMBER);
  while (checkLexeme(DELIMITER, ",")) {
    matchLexeme(DELIMITER, ",");
    matchToken(NUMBER);
  }
  matchLexeme(DELIMITER, ";");
}

void varDeclaration() {
  matchLexeme(KEYWORD, "var");
  identifierList();
  matchLexeme(DELIMITER, ":");
  type();
  matchLexeme(DELIMITER, ";");
  while (checkToken(IDENTIFIER)) {
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
    handleError(IDENTIFIER, "", UNEXPECTED_TYPE);
  }

  if (checkLexeme(IDENTIFIER, "integer") ||
      checkLexeme(IDENTIFIER, "real") == 0 ||
      checkLexeme(IDENTIFIER, "boolean")) {
    matchToken(IDENTIFIER);
  } else {
    handleError(IDENTIFIER, "", INVALID_TYPE);
  }
}

void subroutines() {
  while (checkLexeme(KEYWORD, "procedure") ||
      checkLexeme(KEYWORD, "function")) {
    if (checkLexeme(KEYWORD, "procedure")) procedure();
    if (checkLexeme(KEYWORD, "function")) function();
    matchLexeme(DELIMITER, ";");
  }
}

void procedure() {
  matchLexeme(KEYWORD, "procedure");
  matchToken(IDENTIFIER);
  if (checkLexeme(KEYWORD, "var") || checkToken(IDENTIFIER))
    params();
  matchLexeme(DELIMITER, ";");
  block();
}

void function() {
  matchLexeme(KEYWORD, "function");
  matchToken(IDENTIFIER);
    if (checkLexeme(KEYWORD, "var") || checkToken(IDENTIFIER))
    params();
  matchLexeme(DELIMITER, ";");
  block();  
}

void params() {
  if (checkLexeme(KEYWORD, "var")) matchLexeme(KEYWORD, "var");
  identifierList();
  matchLexeme(DELIMITER, ":");
  matchToken(IDENTIFIER);
  while (checkLexeme(KEYWORD, "var") || checkToken(IDENTIFIER)) {
    if (checkLexeme(KEYWORD, "var")) matchLexeme(KEYWORD, "var");
    identifierList();
    matchLexeme(DELIMITER, ":");
    matchToken(IDENTIFIER);
  }
}

void statementList() {
  matchLexeme(KEYWORD, "begin");
  statement();
  while (checkLexeme(DELIMITER, ";")) {
    matchLexeme(DELIMITER, ";");
    statement();
  }
  matchLexeme(KEYWORD, "end");
}

void statement() {
  if (checkToken(NUMBER)) {
    matchToken(NUMBER);
    matchLexeme(DELIMITER, ":");
  } 

  if (checkToken(IDENTIFIER)) {
    if (lookaheadLexeme(COMPOUND_OPERATOR, ":=")) assignment();
    else subroutineCall();
  } 
  else if (checkLexeme(KEYWORD, "if")) ifStatement();
  else if (checkLexeme(KEYWORD, "while")) whileStatement();
  else if (checkLexeme(KEYWORD, "write")) writeStatement();
  else if (checkLexeme(KEYWORD, "read")) readStatement();
  else if (checkLexeme(KEYWORD, "begin")) statementList();
  else if (checkLexeme(KEYWORD, "goto")) deviation();
  else if (checkLexeme(KEYWORD, "end")) return; 
  else {
    handleError(KEYWORD, "", INVALID_STATEMENT);
  }
}

void assignment() {
  matchToken(IDENTIFIER);
  matchLexeme(COMPOUND_OPERATOR, ":=");
  expression();
} 

void subroutineCall() {
  matchToken(IDENTIFIER);
  if (checkLexeme(DELIMITER, "(")) {
    matchLexeme(DELIMITER, "(");
    expressionList();
    matchLexeme(DELIMITER, ")");
  }
}

void deviation() {
  matchLexeme(KEYWORD, "goto");
  matchToken(NUMBER);
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
  expressionList();
  matchLexeme(DELIMITER, ")");
}

void readStatement() {
  matchLexeme(KEYWORD, "read");
  matchLexeme(DELIMITER, "(");
  identifierList();
  matchLexeme(DELIMITER, ")");
}

void expressionList() {
  expression();
  while (checkLexeme(DELIMITER, ",")) {
    matchLexeme(DELIMITER, ",");
    expression();
  }
}

void expression() {
  simpleExpression();
  if (checkLexeme(OPERATOR, "=") ||
      checkLexeme(COMPOUND_OPERATOR, "<>") ||
      checkLexeme(OPERATOR, "<") ||
      checkLexeme(COMPOUND_OPERATOR, "<=") ||
      checkLexeme(COMPOUND_OPERATOR, ">=") ||
      checkLexeme(OPERATOR, ">")) {
    relation();
    simpleExpression();
  }
}

void relation() {
  if (checkLexeme(OPERATOR, "=") ||
      checkLexeme(COMPOUND_OPERATOR, "<>") ||
      checkLexeme(OPERATOR, "<") ||
      checkLexeme(COMPOUND_OPERATOR, "<=") ||
      checkLexeme(COMPOUND_OPERATOR, ">=") ||
      checkLexeme(OPERATOR, ">")) {
    nextToken();
  }
}

void simpleExpression() {
  if (checkLexeme(OPERATOR, "+") ||
      checkLexeme(OPERATOR, "-")) {
    matchToken(OPERATOR);
  }
  term();
  while (checkLexeme(OPERATOR, "+") ||
         checkLexeme(OPERATOR, "-") ||
         checkLexeme(KEYWORD, "or")) {
    nextToken();
    term();
  }
}

void term() {
  factor();
  while (checkLexeme(OPERATOR, "*") ||
         checkLexeme(OPERATOR, "/") ||
         checkLexeme(KEYWORD, "div") ||
         checkLexeme(KEYWORD, "and")) {
    nextToken();
    factor();
  }
}

void factor() {
  if (checkToken(IDENTIFIER)) {
    if (lookaheadLexeme(DELIMITER, "(")) subroutineCall();
    else matchToken(IDENTIFIER);
  } else if (checkToken(NUMBER)) {
    matchToken(NUMBER);
  } else if (checkLexeme(DELIMITER, "(")) {
    matchLexeme(DELIMITER, "(");
    expression();
    matchLexeme(DELIMITER, ")");
  } else if (checkLexeme(KEYWORD, "not")) {
    factor();
  } else {
    handleError(UNKNOWN, "", INVALID_FACTOR);
  }
}

// Main parser function
void parser(Node *tokenList) {
  currentTok = tokenList->next;
  program();

  if (currentTok->tok->type == END_OF_FILE) {
    printf("Aceito\n");
  } else {
    handleError(END_OF_FILE, currentTok->tok->lexeme, INVALID_END);
  }

  return;
}

// syntax:
// <program>                 -> 'program' <IDENTIFIER> ['(' <identifier-list> ')'] ';' <block> '.'

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

// <factor>                  -> <IDENTIFIER> | <NUMBER> | <subroutine-call> | '(' <expression> ')' |
//                              'not' <factor>