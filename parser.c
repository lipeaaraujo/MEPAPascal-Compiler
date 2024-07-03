#include "header/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *currentTok;

void nextToken() {
  if (currentTok != NULL)
    currentTok = currentTok->next;
}

void matchToken(TokenType expected) {
  if (currentTok->tok->type == expected) {
    nextToken();
  } else {
    printf("erro: unexpected token %d\n", currentTok->tok->type);
    exit(1);
  }
}

void matchLexeme(char *expected) {
  if (strcmp(currentTok->tok->lexeme, expected) == 0) {
    nextToken();
  } else {
    printf("erro: expected %s found %s\n", expected, currentTok->tok->lexeme);
    exit(1);
  }
}

void program() {
  matchLexeme("program");
  matchToken(IDENTIFIER);
  matchLexeme(";");
}

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
