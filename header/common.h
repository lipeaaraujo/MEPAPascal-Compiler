#ifndef COMMON_H
#define COMMON_H

#define BUFFER_SIZE 512

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

#endif // COMMON_H