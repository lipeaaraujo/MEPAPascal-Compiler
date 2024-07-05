#include <stdio.h>
#include "header/lexer.h"
#include "header/parser.h"
#include "header/generator.h"

int main(int argc, char *argv[]) {
  Node *tokenList;

  // check if the number of passed arguments is less than 2
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    return 1;
  }

  // try to open the pascal file in read mode
  FILE *sourceFile = fopen(argv[1], "r");
  if (sourceFile == NULL) {
    perror("Error opening file");
    return 1;
  }

  tokenList = lexer(sourceFile);
  //printTokenList(tokenList);
  // printTokensCount(tokenList);
  parser(tokenList);

  // close the file
  fclose(sourceFile);
  return 0;
}
