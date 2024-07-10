#include "header/generator.h"
#include <stdlib.h>
#include <stdio.h>

CodeNode *codeList = NULL;

// Adds a MEPA instruction to the code.
void addCode(char *instruction) {
  CodeNode *newNode = (CodeNode*)malloc(sizeof(CodeNode));
  strcpy(newNode->instruction, instruction);
  newNode->next = codeList;
  codeList = newNode;
}

// Initialises code generator.
void initCodeGenerator() {
  codeList = NULL;
}

// Prints the generated MEPA code.
void printCode() {
  CodeNode *current = codeList;
  while (current != NULL) {
    printf("%s\n", current->instruction);
    current = current->next;
  }
}