#ifndef PARSER_H
#define PARSER_H

#include "common.h"

typedef enum ErrorType {
  UNEXPECTED_TYPE,
  UNEXPECTED_LEXEME,
  INVALID_TYPE,
  INVALID_STATEMENT,
  INVALID_FACTOR,
  UNDECLARED_SYMBOL,
  INVALID_END
} ErrorType;

typedef struct SymbolNode {
  char name[BUFFER_SIZE];
  struct SymbolNode *next;
} SymbolNode;

void parser(Node *tokenList);

#endif // PARSER_H