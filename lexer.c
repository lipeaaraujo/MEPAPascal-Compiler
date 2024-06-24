#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define BUFFER_SIZE 256

typedef enum TokenType {
  KEYWORD,
  ID,
  INTEGER_LITERAL,
  REAL_LITERAL,
  CHAR_LITERAL,
  STRING_LITERAL,
  OPERATOR,
  DELIMITER,
  END_OF_FILE,
  INVALID
} TokenType;

typedef struct Token {
  TokenType type;
  char *lexeme;
  int line, column;
} Token;

const char *keywords[] = {
  "and", "array", "begin", "div", "do", "else", "end", "function", "if",
  "label", "not", "of", "or", "procedure", "program", "then", "type", "var", "while"
};

const char *operators[] = {
  "(", "(*", ")", "*", "*)", "+", ",", "-", ".", "..", ":", ":=", ";", "<", "=", ">", "[", "]"
};


char* strdup (const char* s){
  size_t slen = strlen(s);
  char* result = malloc(slen + 1);
  if(result == NULL) {
    return NULL;
  }

  memcpy(result, s, slen+1);
  return result;
}

Token *createToken(TokenType type, char *lexeme, int line, int column) {
  Token *newToken = (Token *)malloc(sizeof(Token));
  newToken->type = type;
  newToken->lexeme = strdup(lexeme);
  newToken->line = line;
  newToken->column = column;
  return newToken;
}

// Prints the list of tokens, only for debug purposes.
void printTokenList(Token **tokenList, int count) {
  for (int i=0; i<count; i++) {
    printf("(\"%s\", %d), ", tokenList[i]->lexeme, tokenList[i]->type);
  }
  printf("\n");
}

int compare(const void *a, const void *b) {
  return strcmp(*(const char **)a, *(const char **)b);
}

int matchRegex(const char *pattern, const char *text) {
  // receives a pattern and a string, and verifies if the string matches the regex. 
  regex_t regex;
  int ret;
  
  regcomp(&regex, pattern, REG_EXTENDED);
  ret = regexec(&regex, text, 0, NULL, 0);
  if (ret == REG_NOMATCH) return 0;
  return 1;
}

TokenType getTokenType(char *lexeme) {
  // verify if token is a literal integer
  if (matchRegex("^-[0-9]*[1-9][0-9]*$|^[0-9]*$", lexeme)) return INTEGER_LITERAL;

  // verify if token is a literal string
  if (matchRegex("^'[^']*'$", lexeme)) return STRING_LITERAL;

  // verify if token is a operator
  int sizeOperators = sizeof(operators) / sizeof(operators[0]);
  char **posOperators = bsearch(&lexeme, operators, sizeOperators, sizeof(char *), compare);
  if (posOperators != NULL) return OPERATOR; 

  // verify if token is a keyword or a identifier
  int sizeKeywords = sizeof(keywords) / sizeof(keywords[0]);
  char **posKeyWords = bsearch(&lexeme, keywords, sizeKeywords, sizeof(char *), compare);
  if (posKeyWords != NULL) return KEYWORD;

  // verify if token is a identifier
  if (matchRegex("^[a-zA-Z_][a-zA-Z0-9_]*$", lexeme)) return ID;

  // if it's none, return as invalid
  return INVALID;
}

Token **lexer(FILE *sourceFile) {
  // initialize the list of tokens with a initial size of 8.
  int count = 0, listSize = 8;
  Token **tokenList = (Token **)malloc(listSize * sizeof(Token *));

  // start reading character by character.
  char c;
  int line = 1, column = 1;
  char buffer[256];

  while ((c = fgetc(sourceFile)) != EOF) {
    if (isspace(c)) {
      if (c == '\n') {
        line++;
        column = 1;
      } else {
        column++;
      }
      continue;
    } else if (isalnum(c) || c == '_') {
      // get the whole word
      int i = 0;
      do {
        buffer[i++] = c;
        c = fgetc(sourceFile);
        column++;
      } while (isalnum(c) || c == '_');

      buffer[i] = '\0';
      ungetc(c, sourceFile);
      column--;

    } else if (c == '\''){
      // get the entire lexema
      int i = 0;
      do {
        buffer[i++] = c;
        c = fgetc(sourceFile);
        column++;
      } while (!isspace(c));

      buffer[i] = '\0';
      ungetc(c, sourceFile);
      column--;

    } else if (ispunct(c)) {
      // get the entire lexema
      int i = 0;
      do {
        buffer[i++] = c;
        c = fgetc(sourceFile);
        column++;
      } while (ispunct(c));

      buffer[i] = '\0';
      ungetc(c, sourceFile);
      column--;
    }

    // create and store the new token
    Token *newToken = createToken(getTokenType(buffer), buffer, line, column);
    tokenList[count++] = newToken;
    if (count == listSize) 
      tokenList = (Token **)realloc(tokenList, listSize * 2 * sizeof(Token *));
  }

  Token *endFile = createToken(END_OF_FILE, "", line, column);
  tokenList[count++] = endFile;
  printTokenList(tokenList, count);
  return tokenList;
}

int main(int argc, char *argv[]) {
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

  lexer(sourceFile);

  // close the file
  fclose(sourceFile);
  return 0;
}
