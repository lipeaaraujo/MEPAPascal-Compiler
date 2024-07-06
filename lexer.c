#include "header/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

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

Token *createToken(TokenType type, char *lexeme, int line, int column) {
  Token *newToken = (Token *)malloc(sizeof(Token));
  newToken->type = type;
  strcpy(newToken->lexeme, lexeme);
  newToken->line = line;
  newToken->column = column;
  return newToken;
}

void pushList(Node *head, Token *tok) {
  if (head->next == NULL) {
    Node *new = (Node *)calloc(1, sizeof(Node));
    new->tok = tok;
    head->next = new;
    return;
  }
  pushList(head->next, tok);
}

// Receives a pattern and a string, and verifies if the string matches the regex.
int matchRegex(const char *pattern, const char *text) {   
  regex_t regex;
  int ret;
  
  regcomp(&regex, pattern, REG_EXTENDED);
  ret = regexec(&regex, text, 0, NULL, 0);
  regfree(&regex);
  if (ret == REG_NOMATCH) return 0;
  return 1;
}

int compare(const void *a, const void *b) {
  return strcmp(*(const char **)a, *(const char **)b);
}

// Verifies if a sorted array contains the argument string.
int contains(char *str, const char **arr, int arrSize) {
  char **pos = bsearch(&str, arr, arrSize, sizeof(char *), compare);
  if (pos != NULL) return 1;
  return 0;
}

// Peeks into the next character of the file, without reading it.
char fpeek(FILE *source) {
  char c =  fgetc(source);
  if (c != EOF) ungetc(c, source);
  return c;
}

// Processes a whitespace character read by the lexer.
void processWhitespace(char c, int *line, int *column) {
  if (c == '\n') {
    (*line)++;
    *column = 1;
  } else {
    (*column)++;
  }
}

// Processes a alphanumeric character and reads the rest of the lexeme.
TokenType processAlnum(FILE *src, char *buffer, char c, int *column) {  
  // read the lexeme.
  int i=0;
  do {
    buffer[i++] = c;
    c = fgetc(src);
    (*column)++;
  } while (isalnum(c));
  ungetc(c, src);
  (*column)--;
  buffer[i] = '\0';

  // return the token type
  if (contains(buffer, keywords, sizeKeywords)) return KEYWORD;
  if (matchRegex("^[a-zA-Z_][a-zA-Z0-9_]*$", buffer)) return IDENTIFIER;
  return UNKNOWN;
}

TokenType processDigit(FILE *src, char *buffer, char c, int *column) {
  // read the lexeme.
  int i=0;
  do {
    buffer[i++] = c;
    c = fgetc(src);
    (*column)++;
  } while (isdigit(c) || c == '.');
  ungetc(c, src);
  (*column)--;
  buffer[i] = '\0';

  if (matchRegex("^[0-9]+$|^[0-9]+\\.?[0-9]+$", buffer)) return NUMBER;
  return UNKNOWN;
}

// Processes punction characters.
TokenType processPunct(FILE *src, char *buffer, char c, int *column) {
  // check if it's a comment, compound, normal operator or a delimiter
  if (c == '(' && fpeek(src) == '*') {
    // read the lexeme
    int i=0;
    do {
      buffer[i++] = c;
      c = fgetc(src);
      (*column)++;
    } while (!(buffer[i-1] == '*' && c == ')') && c != EOF);
    if (c != EOF) buffer[i++] = c;
    buffer[i] = '\0';

    if (matchRegex("\\(\\*.*?\\*\\)", buffer)) return COMMENTS;
    else return UNKNOWN;
  }

  buffer[0] = c;
  (*column)++;

  if (fpeek(src) != EOF) {
    buffer[1] = fgetc(src);
    (*column)++;

    if (contains(buffer, compoundOperators, sizeCompoundOperators)) return COMPOUND_OPERATOR;

    ungetc(buffer[1], src);
    buffer[1] = '\0';
    (*column)--;
  }
  
  if (contains(buffer, operators, sizeOperators)) return OPERATOR;
  if (contains(buffer, delimiters, sizeDelimiters)) return DELIMITER;

  return UNKNOWN;
}

// Main lexer function.
Node *lexer(FILE *sourceFile) {
  // initialize the list of tokens with a initial size of 8.
  Node *tokenList = (Node *)calloc(1, sizeof(Node));

  // start reading character by character.
  char c;
  int line = 1, column = 1;
  TokenType type;

  while ((c = fgetc(sourceFile)) != EOF) {
    char buffer[BUFFER_SIZE] = {0};
    if (isspace(c)) {
      processWhitespace(c, &line, &column);
      continue; // skip token creation.
    } else if (isalpha(c)) {
      type = processAlnum(sourceFile, buffer, c, &column);
    } else if (isdigit(c)) {
      type = processDigit(sourceFile, buffer, c, &column);
    } else if (ispunct(c)) {
      type = processPunct(sourceFile, buffer, c, &column);
    }

    // create and store the new token
    if (type != COMMENTS) {
      Token *newToken = createToken(type, buffer, line, column);
      pushList(tokenList, newToken);
    }
  }

  // add the eof token.
  Token *newToken = createToken(END_OF_FILE, "end of file", line, column);
  pushList(tokenList, newToken);

  return tokenList;
}

// Prints the list of tokens, only for debug purposes.
void printTokenList(Node *tokenList) {
  for (Node *aux=tokenList->next; aux != NULL; aux = aux->next) {
    printf("(\"%s\", %d), ", aux->tok->lexeme, aux->tok->type);
  }
  printf("\n");
}

void printTokensCount(Node *list) {
  int keywords=0, ids=0, nums=0, operators=0, compOperators=0, 
      delims=0, comments=0, unknowns=0;
  Node *aux = list->next;

  while (aux != NULL) {
    switch(aux->tok->type) {
      case KEYWORD: keywords++; break;
      case IDENTIFIER: ids++; break;
      case NUMBER: nums++; break;
      case OPERATOR: operators++; break;
      case COMPOUND_OPERATOR: compOperators++; break;
      case DELIMITER: delims++; break;
      case COMMENTS: comments++; break;
      case UNKNOWN: unknowns++; break;
      case END_OF_FILE: break;
    }
    aux = aux->next;
  }

  printf("KEYWORD: %d\n", keywords);
  printf("IDENTIFIER: %d\n", ids);
  printf("NUMBER: %d\n", nums);
  printf("OPERATOR: %d\n", operators);
  printf("COMPOUND OPERATOR: %d\n", compOperators);
  printf("DELIMITER: %d\n", delims);
  printf("COMMENTS: %d\n", comments);
  printf("UNKNOWN: %d\n", unknowns);
}
