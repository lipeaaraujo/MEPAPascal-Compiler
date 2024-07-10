#ifndef GENERATOR_H
#define GENERATOR_H

#include "common.h";

typedef struct CodeNode {
  char instruction[BUFFER_SIZE];
  struct CodeNode *next;
} CodeNode;

extern CodeNode *codeList;

void addCode(char *instruction);
void initCodeGenerator();
void printCode();

#endif // GENERATOR_H