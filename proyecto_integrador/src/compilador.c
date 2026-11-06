/* compilador.c - punto de entrada del compilador.
 *
 * TODO (Hito 1): leer el archivo, tokenizarlo y construir el AST.
 * TODO (Hito 2): analizar la semántica y reportar diagnósticos.
 * TODO (Hito 3): generar TAC y optimizarlo.
 * TODO (Hito 4): generar ensamblador, ensamblar, enlazar y ejecutar.
 */
#include <stdio.h>

#include "compilador.h"

int compilar(const char *ruta, const char *modo) {
    fprintf(stderr, "%s: compilador sin implementar (modo %s)\n", ruta, modo);
    return 1;
}
