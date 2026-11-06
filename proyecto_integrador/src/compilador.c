/* compilador.c - punto de entrada del compilador.
 *
 * Hecho (Avance 1): leer el archivo y tokenizarlo (--tokens).
 * TODO (Hito 1): construir el AST (--ast) y --check de expresiones.
 * TODO (Hito 2): analizar la semántica y reportar diagnósticos.
 * TODO (Hito 3): generar TAC y optimizarlo.
 * TODO (Hito 4): generar ensamblador, ensamblar, enlazar y ejecutar.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compilador.h"
#include "lexer.h"

/* Lee el archivo completo en memoria. Devuelve NULL si no se pudo abrir. */
static char *leer_archivo(const char *ruta) {
    FILE *f = fopen(ruta, "rb");
    if (f == NULL) {
        return NULL;
    }
    size_t cap = 4096, n = 0;
    char *buf = malloc(cap);
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (n + 1 >= cap) {
            cap *= 2;
            char *mas = realloc(buf, cap);
            if (mas == NULL) {
                free(buf);
                fclose(f);
                return NULL;
            }
            buf = mas;
        }
        buf[n++] = (char)c;
    }
    buf[n] = '\0';
    fclose(f);
    return buf;
}

int compilar(const char *ruta, const char *modo) {
    char *fuente = leer_archivo(ruta);
    if (fuente == NULL) {
        fprintf(stderr, "%s: no se pudo abrir el archivo\n", ruta);
        return 2;
    }

    ListaTokens tokens;
    tokenizar(ruta, fuente, &tokens);

    int resultado;
    if (strcmp(modo, "--tokens") == 0) {
        for (int i = 0; i < tokens.cantidad; i++) {
            const Token *t = &tokens.datos[i];
            if (t->tipo == TOK_EOF) {
                printf("%d:%d EOF\n", t->linea, t->columna);
            } else {
                printf("%d:%d %s %s\n", t->linea, t->columna,
                       nombre_tipo_token(t->tipo), t->lexema);
            }
        }
        resultado = (tokens.errores > 0) ? 1 : 0;
    } else {
        fprintf(stderr, "%s: modo %s todavia sin implementar\n", ruta, modo);
        resultado = 1;
    }

    liberar_tokens(&tokens);
    free(fuente);
    return resultado;
}
