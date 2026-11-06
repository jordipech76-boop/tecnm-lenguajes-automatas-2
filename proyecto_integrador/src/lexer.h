/* lexer.h - analizador lexico de Lenguajito.
 *
 * Convierte el texto de un programa .lj en una lista de tokens,
 * cada uno con su linea y columna (empiezan en 1).
 */
#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOK_KW,      /* int, if, else, while, print */
    TOK_ID,      /* identificadores */
    TOK_NUM,     /* enteros sin signo */
    TOK_OP,      /* + - * / < <= > >= == != = */
    TOK_DELIM,   /* ( ) { } ; */
    TOK_EOF      /* fin de archivo */
} TipoToken;

typedef struct {
    TipoToken tipo;
    char lexema[64];
    int linea;
    int columna;
} Token;

typedef struct {
    Token *datos;     /* arreglo dinamico de tokens */
    int cantidad;
    int capacidad;
    int errores;      /* cuantos errores S01 se encontraron */
} ListaTokens;

/* Tokeniza `fuente`. Los errores lexicos se imprimen en stdout con el
 * formato archivo:linea:columna: error S01: mensaje */
void tokenizar(const char *ruta, const char *fuente, ListaTokens *lista);

/* Nombre del tipo para --tokens: "KW", "ID", "NUM", "OP", "DELIM", "EOF" */
const char *nombre_tipo_token(TipoToken t);

/* Libera la memoria de la lista */
void liberar_tokens(ListaTokens *lista);

#endif /* LEXER_H */
