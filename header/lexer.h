#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define BUFFER_SIZE 512

typedef enum TokenType {
  KEYWORD,
  IDENTIFIER,
  NUMBER,
  OPERATOR,
  COMPOUND_OPERATOR,
  DELIMITER,
  COMMENTS,
  UNKNOWN
} TokenType;

typedef struct Token {
  TokenType type;
  char lexeme[BUFFER_SIZE];
  int line, column;
} Token;

typedef struct Node {
  Token *tok;
  struct Node *next;
} Node;

Node *lexer(FILE *sourceFile);
void printTokenList(Node *tokenList);
void printTokensCount(Node *list);

#endif // LEXER_H