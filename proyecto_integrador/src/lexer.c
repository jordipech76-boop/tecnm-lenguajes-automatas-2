/* lexer.c - analizador lexico de Lenguajito (ver docs/gramatica.md, seccion 2). */
#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *const PALABRAS_RESERVADAS[] = {
    "int", "if", "else", "while", "print"
};

/* Agrega un token al final de la lista, creciendo el arreglo si hace falta */
static void agregar(ListaTokens *lista, TipoToken tipo, const char *inicio,
                    int largo, int linea, int columna) {
    if (lista->cantidad == lista->capacidad) {
        int nueva = (lista->capacidad == 0) ? 64 : lista->capacidad * 2;
        Token *mas = realloc(lista->datos, (size_t)nueva * sizeof *mas);
        if (mas == NULL) {
            fprintf(stderr, "sin memoria para tokens\n");
            exit(1);
        }
        lista->datos = mas;
        lista->capacidad = nueva;
    }
    Token *t = &lista->datos[lista->cantidad++];
    t->tipo = tipo;
    if (largo > (int)sizeof t->lexema - 1) {
        largo = (int)sizeof t->lexema - 1;   /* lexema muy largo: se corta */
    }
    memcpy(t->lexema, inicio, (size_t)largo);
    t->lexema[largo] = '\0';
    t->linea = linea;
    t->columna = columna;
}

static int es_reservada(const char *inicio, int largo) {
    for (size_t i = 0; i < sizeof PALABRAS_RESERVADAS / sizeof PALABRAS_RESERVADAS[0]; i++) {
        const char *p = PALABRAS_RESERVADAS[i];
        if ((int)strlen(p) == largo && strncmp(p, inicio, (size_t)largo) == 0) {
            return 1;
        }
    }
    return 0;
}

void tokenizar(const char *ruta, const char *fuente, ListaTokens *lista) {
    lista->datos = NULL;
    lista->cantidad = 0;
    lista->capacidad = 0;
    lista->errores = 0;

    const char *p = fuente;
    int linea = 1, columna = 1;

    while (*p != '\0') {
        char c = *p;

        /* espacios, tabuladores y saltos de linea se ignoran */
        if (c == '\n') { linea++; columna = 1; p++; continue; }
        if (c == ' ' || c == '\t' || c == '\r') { columna++; p++; continue; }

        /* comentario // hasta el final de la linea */
        if (c == '/' && p[1] == '/') {
            while (*p != '\0' && *p != '\n') { p++; columna++; }
            continue;
        }

        const char *inicio = p;
        int col_inicio = columna;

        /* identificador o palabra reservada: [a-zA-Z_][a-zA-Z0-9_]* */
        if (isalpha((unsigned char)c) || c == '_') {
            while (isalnum((unsigned char)*p) || *p == '_') { p++; columna++; }
            int largo = (int)(p - inicio);
            agregar(lista, es_reservada(inicio, largo) ? TOK_KW : TOK_ID,
                    inicio, largo, linea, col_inicio);
            continue;
        }

        /* entero sin signo: [0-9]+ */
        if (isdigit((unsigned char)c)) {
            while (isdigit((unsigned char)*p)) { p++; columna++; }
            agregar(lista, TOK_NUM, inicio, (int)(p - inicio), linea, col_inicio);
            continue;
        }

        /* operadores de dos caracteres: <= >= == != */
        if ((c == '<' || c == '>' || c == '=' || c == '!') && p[1] == '=') {
            agregar(lista, TOK_OP, inicio, 2, linea, col_inicio);
            p += 2; columna += 2;
            continue;
        }

        /* operadores de un caracter: + - * / < > = */
        if (strchr("+-*/<>=", c) != NULL) {
            agregar(lista, TOK_OP, inicio, 1, linea, col_inicio);
            p++; columna++;
            continue;
        }

        /* delimitadores: ( ) { } ; */
        if (strchr("(){};", c) != NULL) {
            agregar(lista, TOK_DELIM, inicio, 1, linea, col_inicio);
            p++; columna++;
            continue;
        }

        /* cualquier otra cosa es un error lexico (incluye un '!' solo) */
        printf("%s:%d:%d: error S01: caracter invalido '%c'\n", ruta, linea, columna, c);
        lista->errores++;
        p++; columna++;
    }

    agregar(lista, TOK_EOF, "", 0, linea, columna);
}

const char *nombre_tipo_token(TipoToken t) {
    switch (t) {
        case TOK_KW:    return "KW";
        case TOK_ID:    return "ID";
        case TOK_NUM:   return "NUM";
        case TOK_OP:    return "OP";
        case TOK_DELIM: return "DELIM";
        case TOK_EOF:   return "EOF";
    }
    return "?";
}

void liberar_tokens(ListaTokens *lista) {
    free(lista->datos);
    lista->datos = NULL;
    lista->cantidad = 0;
    lista->capacidad = 0;
}
