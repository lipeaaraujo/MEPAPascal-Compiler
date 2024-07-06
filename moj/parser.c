#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define BUFFER_SIZE 512

// LEXER -----------------------------------

typedef enum TokenType {
  KEYWORD,
  IDENTIFIER,
  NUMBER,
  OPERATOR,
  COMPOUND_OPERATOR,
  DELIMITER,
  COMMENTS,
  UNKNOWN,
  END_OF_FILE
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

// PARSER -----------------------------------

typedef enum ErrorType {
  UNEXPECTED_TYPE,
  UNEXPECTED_LEXEME,
  INVALID_TYPE,
  INVALID_STATEMENT,
  INVALID_FACTOR,
  INVALID_END
} ErrorType;

Node *currentTok;

// Handles a error based on it's error type
void handleError(TokenType expectedType, char *expectedLexeme,
                 ErrorType error) {
  char *types[] = {
    "keyword", "identifier", "number", "operator", 
    "compound_operator", "delimiter", "comments", "unknown"
  };

  switch (error) {
    case UNEXPECTED_TYPE:
      fprintf(stderr, "Error: expected type %s but found %s at line %d\n",
              types[expectedType], types[currentTok->tok->type], currentTok->tok->line);
      break;
    case UNEXPECTED_LEXEME:
      fprintf(stderr, "Error: expected \"%s\" but found \"%s\" at line %d\n",
              expectedLexeme, currentTok->tok->lexeme, currentTok->tok->line);
      break;
    case INVALID_TYPE:
      fprintf(stderr, "Error: expected valid type but found \"%s\" at line %d\n",
              currentTok->tok->lexeme, currentTok->tok->line);
      break;
    case INVALID_STATEMENT:
      fprintf(stderr, "Error: expected valid statement but found \"%s\" at line %d\n",
              currentTok->tok->lexeme, currentTok->tok->line);
      break;
    case INVALID_FACTOR:
      fprintf(stderr, "Error: expected valid factor but found \"%s\" at line %d\n",
              currentTok->tok->lexeme, currentTok->tok->line);
      break;
    case INVALID_END:
      fprintf(stderr, "Error: unexpected token after end of file \"%s\"",
              currentTok->tok->lexeme);
      break;
    default:
      fprintf(stderr, "Error: unknown error");
      break;
  }

  printf("Rejeito\n");
  exit(0);
}

// Moves to the next token in the list
void nextToken() {
  if (currentTok != NULL) {
    currentTok = currentTok->next;
  }
}

// Checks if current token type is the expected type
int checkToken(TokenType expected) {
  return currentTok->tok->type == expected;
}

// Checks if current lexeme is the expected lexeme
int checkLexeme(TokenType tokType, char *expected) {
  return checkToken(tokType) &&
         (strcmp(currentTok->tok->lexeme, expected) == 0);
}

// Moves to the next token if current token type matches expected
void matchToken(TokenType expected) {
  if (checkToken(expected)) {
    nextToken();
  } else {
    handleError(expected, "", UNEXPECTED_TYPE);
  }
}

// Moves to the next token if current lexeme matches expected
void matchLexeme(TokenType tokType, char *expected) {
  if (checkLexeme(tokType, expected)) {
    nextToken();
  } else {
    handleError(tokType, expected, UNEXPECTED_LEXEME);
  }
}

// Checks if the token ahead is of expected type
int lookaheadToken(TokenType expected) {
  if (currentTok->next == NULL) return 0;  
  return currentTok->next->tok->type == expected;
}

// Checks if the lexeme of the token ahead is the expected lexeme
int lookaheadLexeme(TokenType tokType, char *expected) {
  if (currentTok->next == NULL) return 0;
  return lookaheadToken(tokType) &&
         (strcmp(currentTok->next->tok->lexeme, expected) == 0);
}

// Parser functions
void program();
void block();
void labelDeclaration();
void varDeclaration();
void identifierList();
void type();
void subroutines();
void procedure();
void function();
void params();
void statementList();
void statement();
void assignment();
void subroutineCall();
void deviation();
void ifStatement();
void whileStatement();
void writeStatement();
void readStatement();
void expressionList();
void expression();
void relation();
void simpleExpression();
void term();
void factor();

void program() {
  matchLexeme(KEYWORD, "program");
  matchToken(IDENTIFIER);
  if (checkLexeme(DELIMITER, "(")) {
    matchLexeme(DELIMITER, "(");
    identifierList();
    matchLexeme(DELIMITER, ")");    
  }
  matchLexeme(DELIMITER, ";");
  block();
  matchLexeme(DELIMITER, ".");
}

void block() {
  if (checkLexeme(KEYWORD, "label")) labelDeclaration();
  if (checkLexeme(KEYWORD, "var")) varDeclaration();
  if (checkLexeme(KEYWORD, "procedure") ||
      checkLexeme(KEYWORD, "function")) subroutines();
  statementList();
}

void labelDeclaration() {
  matchLexeme(KEYWORD, "label");
  matchToken(NUMBER);
  while (checkLexeme(DELIMITER, ",")) {
    matchLexeme(DELIMITER, ",");
    matchToken(NUMBER);
  }
  matchLexeme(DELIMITER, ";");
}

void varDeclaration() {
  matchLexeme(KEYWORD, "var");
  identifierList();
  matchLexeme(DELIMITER, ":");
  type();
  matchLexeme(DELIMITER, ";");
  while (checkToken(IDENTIFIER)) {
    identifierList();
    matchLexeme(DELIMITER, ":");
    type();
    matchLexeme(DELIMITER, ";");
  }
}

void identifierList() {
  matchToken(IDENTIFIER);
  while (checkLexeme(DELIMITER, ",")) {
    matchLexeme(DELIMITER, ",");
    matchToken(IDENTIFIER);
  }
}

void type() {
  if (!checkToken(IDENTIFIER)) {
    handleError(IDENTIFIER, "", UNEXPECTED_TYPE);
  }

  if (checkLexeme(IDENTIFIER, "integer") ||
      checkLexeme(IDENTIFIER, "real") == 0 ||
      checkLexeme(IDENTIFIER, "boolean")) {
    matchToken(IDENTIFIER);
  } else {
    handleError(IDENTIFIER, "", INVALID_TYPE);
  }
}

void subroutines() {
  while (checkLexeme(KEYWORD, "procedure") ||
      checkLexeme(KEYWORD, "function")) {
    if (checkLexeme(KEYWORD, "procedure")) procedure();
    if (checkLexeme(KEYWORD, "function")) function();
    matchLexeme(DELIMITER, ";");
  }
}

void procedure() {
  matchLexeme(KEYWORD, "procedure");
  matchToken(IDENTIFIER);
  if (checkLexeme(KEYWORD, "var") || checkToken(IDENTIFIER))
    params();
  matchLexeme(DELIMITER, ";");
  block();
}

void function() {
  matchLexeme(KEYWORD, "function");
  matchToken(IDENTIFIER);
    if (checkLexeme(KEYWORD, "var") || checkToken(IDENTIFIER))
    params();
  matchLexeme(DELIMITER, ";");
  block();  
}

void params() {
  if (checkLexeme(KEYWORD, "var")) matchLexeme(KEYWORD, "var");
  identifierList();
  matchLexeme(DELIMITER, ":");
  matchToken(IDENTIFIER);
  while (checkLexeme(KEYWORD, "var") || checkToken(IDENTIFIER)) {
    if (checkLexeme(KEYWORD, "var")) matchLexeme(KEYWORD, "var");
    identifierList();
    matchLexeme(DELIMITER, ":");
    matchToken(IDENTIFIER);
  }
}

void statementList() {
  matchLexeme(KEYWORD, "begin");
  statement();
  while (checkLexeme(DELIMITER, ";")) {
    matchLexeme(DELIMITER, ";");
    statement();
  }
  matchLexeme(KEYWORD, "end");
}

void statement() {
  if (checkToken(NUMBER)) {
    matchToken(NUMBER);
    matchLexeme(DELIMITER, ":");
  } 

  if (checkToken(IDENTIFIER)) {
    if (lookaheadLexeme(COMPOUND_OPERATOR, ":=")) assignment();
    else subroutineCall();
  } 
  else if (checkLexeme(KEYWORD, "if")) ifStatement();
  else if (checkLexeme(KEYWORD, "while")) whileStatement();
  else if (checkLexeme(KEYWORD, "write")) writeStatement();
  else if (checkLexeme(KEYWORD, "read")) readStatement();
  else if (checkLexeme(KEYWORD, "begin")) statementList();
  else if (checkLexeme(KEYWORD, "goto")) deviation();
  else if (checkLexeme(KEYWORD, "end")) return; 
  else {
    handleError(KEYWORD, "", INVALID_STATEMENT);
  }
}

void assignment() {
  matchToken(IDENTIFIER);
  matchLexeme(COMPOUND_OPERATOR, ":=");
  expression();
} 

void subroutineCall() {
  matchToken(IDENTIFIER);
  if (checkLexeme(DELIMITER, "(")) {
    matchLexeme(DELIMITER, "(");
    expressionList();
    matchLexeme(DELIMITER, ")");
  }
}

void deviation() {
  matchLexeme(KEYWORD, "goto");
  matchToken(NUMBER);
}

void ifStatement() {
  matchLexeme(KEYWORD, "if");
  expression();
  matchLexeme(KEYWORD, "then");
  statement();
  if (checkLexeme(KEYWORD, "else")) {
    matchLexeme(KEYWORD, "else");
    statement();
  }
}

void whileStatement() {
  matchLexeme(KEYWORD, "while");
  expression();
  matchLexeme(KEYWORD, "do");
  statement();
}

void writeStatement() {
  matchLexeme(KEYWORD, "write");
  matchLexeme(DELIMITER, "(");
  expressionList();
  matchLexeme(DELIMITER, ")");
}

void readStatement() {
  matchLexeme(KEYWORD, "read");
  matchLexeme(DELIMITER, "(");
  identifierList();
  matchLexeme(DELIMITER, ")");
}

void expressionList() {
  expression();
  while (checkLexeme(DELIMITER, ",")) {
    matchLexeme(DELIMITER, ",");
    expression();
  }
}

void expression() {
  simpleExpression();
  if (checkLexeme(OPERATOR, "=") ||
      checkLexeme(COMPOUND_OPERATOR, "<>") ||
      checkLexeme(OPERATOR, "<") ||
      checkLexeme(COMPOUND_OPERATOR, "<=") ||
      checkLexeme(COMPOUND_OPERATOR, ">=") ||
      checkLexeme(OPERATOR, ">")) {
    relation();
    simpleExpression();
  }
}

void relation() {
  if (checkLexeme(OPERATOR, "=") ||
      checkLexeme(COMPOUND_OPERATOR, "<>") ||
      checkLexeme(OPERATOR, "<") ||
      checkLexeme(COMPOUND_OPERATOR, "<=") ||
      checkLexeme(COMPOUND_OPERATOR, ">=") ||
      checkLexeme(OPERATOR, ">")) {
    nextToken();
  }
}

void simpleExpression() {
  if (checkLexeme(OPERATOR, "+") ||
      checkLexeme(OPERATOR, "-")) {
    matchToken(OPERATOR);
  }
  term();
  while (checkLexeme(OPERATOR, "+") ||
         checkLexeme(OPERATOR, "-") ||
         checkLexeme(KEYWORD, "or")) {
    nextToken();
    term();
  }
}

void term() {
  factor();
  while (checkLexeme(OPERATOR, "*") ||
         checkLexeme(OPERATOR, "/") ||
         checkLexeme(KEYWORD, "div") ||
         checkLexeme(KEYWORD, "and")) {
    nextToken();
    factor();
  }
}

void factor() {
  if (checkToken(IDENTIFIER)) {
    if (lookaheadLexeme(DELIMITER, "(")) subroutineCall();
    else matchToken(IDENTIFIER);
  } else if (checkToken(NUMBER)) {
    matchToken(NUMBER);
  } else if (checkLexeme(DELIMITER, "(")) {
    matchLexeme(DELIMITER, "(");
    expression();
    matchLexeme(DELIMITER, ")");
  } else if (checkLexeme(KEYWORD, "not")) {
    factor();
  } else {
    handleError(UNKNOWN, "", INVALID_FACTOR);
  }
}

// Main parser function
void parser(Node *tokenList) {
  currentTok = tokenList->next;
  program();

  if (currentTok->tok->type == END_OF_FILE) {
    printf("Aceito\n");
  } else {
    handleError(END_OF_FILE, currentTok->tok->lexeme, INVALID_END);
  }

  return;
}

// MAIN -----------------------------------

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
  // printTokenList(tokenList);
  // printTokensCount(tokenList);
  parser(tokenList);

  // close the file
  fclose(sourceFile);
  return 0;
}
