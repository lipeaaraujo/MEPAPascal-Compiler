#include <stdio.h>

// typedef enum TipoToken {
//   PALAVRA_CHAVE,
//   ID,
//   LITERAL_INTEIRO,
//   LITERAL_REAL,
//   LITERAL_CHAR,
//   LITERAL_STRING,
//   OPERADOR,
//   DELIMITADOR,
//   FIM_ARQUIVO
// } TipoToken;

// typedef struct Token {
//   TipoToken tipo;
//   char *lexema;
//   int linha, coluna;
// } Token;

// const char *palavras_chave[] = {};

// const char *operadores[] = {};

int main(int argc, char *argv[]) {
  // verifica se o número de argumentos passados é menor que 2.
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    return 1;
  }

  // tenta abrir o arquivo pascal em modo leitura.
  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  printf("%s\n", argv[1]);

  // fecha o arquivo.
  fclose(file);
  return 0;
}