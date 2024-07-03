#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "common.h"

Node *lexer(FILE *sourceFile);
void printTokenList(Node *tokenList);
void printTokensCount(Node *list);

#endif // LEXER_H