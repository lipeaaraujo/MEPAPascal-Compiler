#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define BUFFER_SIZE 256

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
  char *lexeme;
  int line, column;
} Token;

const char *keywords[] = {
  "and", "array", "begin", "div", "do",
  "else", "end", "function", "goto", "if", "label", "not", "of", "or",
  "procedure", "program", "read", "then", "type", "var", "while", "write"
};
int sizeKeywords = sizeof(keywords) / sizeof(keywords[0]);

const char *operators[] = {
  "*", "+", "-", "/", "<", "=", ">"
};
int sizeOperators = sizeof(operators) / sizeof(operators[0]);

const char *compoundOperators[] = {
  ":=", "<=", "<>", ">="
};
int sizeCompoundOperators = sizeof(compoundOperators) / sizeof(compoundOperators[0]);

const char *delimiters[] = {
  "(", ")", ",", ".", ":", ";", "[", "]"
};
int sizeDelimiters = sizeof(delimiters) / sizeof(delimiters[0]);

char* strdup (const char* s) {
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

int notspace(int c) {
  return !isspace(c);
}

// Gets the next lexeme in the source file and stores it in the buffer based on the condition set.
void readLexeme(FILE *sourceFile, char buffer[][BUFFER_SIZE], char c,
                int *column, int (*condition)(int)) {
  int i = strlen(*buffer);
  do {
    (*buffer)[i++] = c;
    c = fgetc(sourceFile);
    (*column)++;
  } while (condition(c));

  ungetc(c, sourceFile);
  (*column)--;

  (*buffer)[i] = '\0';
}

int matchRegex(const char *pattern, const char *text) {
  // receives a pattern and a string, and verifies if the string matches the regex. 
  regex_t regex;
  int ret;
  
  regcomp(&regex, pattern, REG_EXTENDED);
  ret = regexec(&regex, text, 0, NULL, 0);
  regfree(&regex);
  if (ret == REG_NOMATCH) return 0;
  return 1;
}

// Verifies if a sorted array contains the argument string.
int contains(char *str, const char **arr, int arrSize) {
  char **pos = bsearch(&str, arr, arrSize, sizeof(char *), compare);
  if (pos != NULL) return 1;
  return 0;
}

TokenType getTokenType(char *lexeme) {
  // verify if token is a keyword
  int sizeKeywords = sizeof(keywords) / sizeof(keywords[0]);
  if (contains(lexeme, keywords, sizeKeywords)) return KEYWORD;

  // verify if token is a identifier
  if (matchRegex("^[a-zA-Z_][a-zA-Z0-9_]*$", lexeme)) return IDENTIFIER;

  // verify if token is a number
  if (matchRegex("^-[0-9]*[1-9][0-9]*$|^[0-9]*$", lexeme)) return NUMBER;

  // verify if token is a operator
  int sizeOperators = sizeof(operators) / sizeof(operators[0]);
  char **posOperators = bsearch(&lexeme, operators, sizeOperators, sizeof(char *), compare);
  if (posOperators != NULL) return OPERATOR;

  // verify if token is a compound operator
  int sizeCompoundOper = sizeof(compoundOperators) / sizeof(compoundOperators[0]);
  char **posCompoundOper = bsearch(&lexeme, compoundOperators, sizeCompoundOper, sizeof(char *), compare);
  if (posCompoundOper != NULL) return COMPOUND_OPERATOR;

  // verify if token is a delimiter
  int sizeDelimiters = sizeof(delimiters) / sizeof(delimiters[0]);
  char **posDelimiter = bsearch(&lexeme, delimiters, sizeDelimiters, sizeof(char *), compare);
  if (posDelimiter != NULL) return DELIMITER;

  // verify if token is a comment
  if (matchRegex("\\(\\*.*?\\*\\)", lexeme)) return COMMENTS;

  // if it's none, return as UNKNOWN
  return UNKNOWN;
}

// Peeks into the next character of the file, without reading it.
char fpeek(FILE *source) {
  char c =  fgetc(source);
  if (c != EOF) ungetc(c, source);
  return c;
}

Token **lexer(FILE *sourceFile) {
  // initialize the list of tokens with a initial size of 8.
  int count = 0, listSize = 256;
  Token **tokenList = (Token **)malloc(listSize * sizeof(Token *));

  // start reading character by character.
  char c;
  int line = 1, column = 1;
  TokenType type;

  while ((c = fgetc(sourceFile)) != EOF) {
    char buffer[BUFFER_SIZE] = {0};
    if (isspace(c)) {
      if (c == '\n') {
        line++;
        column = 1;
      } else {
        column++;
      }
      continue; // skip everything
    } else if (isalnum(c)) {
      readLexeme(sourceFile, &buffer, c, &column, isalnum);
      if (contains(buffer, keywords, sizeKeywords)) type = KEYWORD;
      else if (matchRegex("^[a-zA-Z_][a-zA-Z0-9_]*$", buffer)) type = IDENTIFIER;
      else if (matchRegex("^-[0-9]*[1-9][0-9]*$|^[0-9]*$", buffer)) type = NUMBER;
      else type = UNKNOWN;
    } else if (ispunct(c)) {
      if (c == '(' && fpeek(sourceFile) == '*') {
        // verify if token is a comment
        readLexeme(sourceFile, &buffer, c, &column, notspace);
        if (matchRegex("\\(\\*.*?\\*\\)", buffer)) type = COMMENTS;
        else type = UNKNOWN;
      } else if (c == '\'') {
        // verify if token is a literal string
        readLexeme(sourceFile, &buffer, c, &column, notspace);
        type = UNKNOWN;
      } else {
        buffer[0] = c;
        if (contains(buffer, operators, sizeOperators)) {
          type = OPERATOR;
        } else if (contains(buffer, delimiters, sizeDelimiters)) {
          type = DELIMITER;
        } else {
          buffer[1] = fgetc(sourceFile);
          if (contains(buffer, compoundOperators, sizeCompoundOperators)) {
            type = COMPOUND_OPERATOR;
          } else {
            type = UNKNOWN;
          }
        }
      }
    }

    // create and store the new token
    Token *newToken = createToken(type, buffer, line, column);
    tokenList[count++] = newToken;
    if (count == listSize) 
      tokenList = (Token **)realloc(tokenList, listSize * 2 * sizeof(Token *));
      listSize *= 2;
  }

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
