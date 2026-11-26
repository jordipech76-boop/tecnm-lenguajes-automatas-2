# Bitácora de Requerimientos y Auditoría IA — Sesión 22
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Generación de TAC para Expresiones  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Traductor de nodos AST a TAC  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Postorden:** primero se generan los dos hijos y al final el operador. Así en `a + b * c` sale primero `_t0 = b * c` y luego `_t1 = a + _t0`.
> - **Qué regresa cada nodo:** hice una función recursiva que regresa el **nombre** del nodo: si es hoja, su valor (`"a"`); si es operador, el temporal que se le asignó. El padre usa esos nombres en su línea.
> - **Las hojas no emiten nada:** un árbol que es solo una hoja da 0 líneas.
> - **Los límites:** nunca escribo más de `max` líneas, y `snprintf(lineas[i], 64, ...)` deja máximo 63 caracteres más el `'\0'`.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Conservación de precedencia de operadores |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion22.h ya existe y no
   se puede modificar: struct Nodo { char tipo; char op; char valor[16];
   struct Nodo *izq; struct Nodo *der; } ('H' = hoja, 'B' = operador binario).
   La plantilla ya trae Pool y nuevo_temporal(Pool *p, int *contador).
R: Ingeniero de compiladores experto en C y generación de código intermedio.
E: Implementa int generar_tac(const Nodo *raiz, char lineas[][64], int max):
   1. Recorrido en postorden: primero los hijos y después el operador.
   2. Cada operador usa un temporal nuevo de nuevo_temporal y escribe
      "_tX = a op b".
   3. Devuelve cuántas líneas emitió; nunca más de max líneas ni más de
      63 caracteres por línea.
O: Solo el .c completo (con Pool y nuevo_temporal), empezando con
   #include "api_sesion22.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion22.h"
#include <stdio.h>
#include <string.h>

/* Los temporales ya estan resueltos: usalos tal cual. */
typedef struct {
    char nombre[64][16];
    int usados;
} Pool;

const char *nuevo_temporal(Pool *p, int *contador) {
    if (p->usados >= 64) return "_t?";
    snprintf(p->nombre[p->usados], sizeof p->nombre[0], "_t%d", (*contador)++);
    return p->nombre[p->usados++];
}

typedef struct {
    char (*lineas)[64];
    int max;
    int emitidas;
    int contador;
    Pool pool;
} Generador;

static const char *generar_nodo(Generador *g, const Nodo *nodo) {
    if (nodo->tipo == 'H') {
        return nodo->valor;
    }
    const char *izq = generar_nodo(g, nodo->izq);
    const char *der = generar_nodo(g, nodo->der);
    const char *temp = nuevo_temporal(&g->pool, &g->contador);
    if (g->emitidas < g->max) {
        snprintf(g->lineas[g->emitidas], 64, "%s = %s %c %s", temp, izq, nodo->op, der);
        g->emitidas++;
    }
    return temp;
}

int generar_tac(const Nodo *raiz, char lineas[][64], int max) {
    if (raiz == NULL || lineas == NULL || max <= 0) {
        return 0;
    }
    Generador g;
    g.lineas = lineas;
    g.max = max;
    g.emitidas = 0;
    g.contador = 0;
    g.pool.usados = 0;
    generar_nodo(&g, raiz);
    return g.emitidas;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Expresiones anidadas con paréntesis

### Salida del harness
```text
=== AUDITORIA SESION 22 - Traductor de AST a TAC con precedencia ===
Caso                               esperado:              obtenido:              resultado
[ 1] a + b * c                          esperado: _t0 = b * c  obtenido: _t0 = b * c  OK
[ 2] a + b * c (2a linea)
     esperado: _t1 = a + _t0
     obtenido: _t1 = a + _t0
     -> OK
[ 3] (a + b) * c
     esperado: _t1 = _t0 * c
     obtenido: _t1 = _t0 * c
     -> OK
[ 4] x * (y - z) + w
     esperado: _t2 = _t1 + w
     obtenido: _t2 = _t1 + w
     -> OK
[ 5] hoja sin operaciones               esperado: 0            obtenido: 0            OK
[ 6] (a + b) * (c - d)
     esperado: _t2 = _t0 * _t1
     obtenido: _t2 = _t0 * _t1
     -> OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion22.md
```

* [ ] Caso base y límites superados sin *Crash*. (La versión de la IA se cae con un operador sin hijo; ver reporte.)
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. Probé dos casos que el harness no tiene:
>
> | Caso | IA | Mi artesano |
> | :--- | :--- | :--- |
> | operador con hijo derecho `NULL` | se cae (*Segmentation fault*) | 0 líneas, sin error |
> | árbol con 65 operadores | `_t? = _t63 + h65` | `_t? = _t63 + h65` |
>
> 1. **Árbol mal formado:** la IA baja a `nodo->izq` y `nodo->der` sin revisar si existen y truena. En mi versión la función regresa `NULL` cuando falta un hijo y la generación se detiene.
> 2. **Más de 64 temporales (los dos):** el `Pool` de la plantilla solo tiene 64 nombres, y a partir del 65 `nuevo_temporal` regresa `"_t?"` para todos. Si hubiera más de uno, serían la misma variable. La solución sería que el pool crezca con `malloc` o que el nombre se escriba directo en un buffer propio, pero eso cambia la plantilla.
>
> **Complejidad:** O(n) en tiempo, con n los nodos del árbol (cada uno se visita una vez), y O(h) de pila por la recursión.

---
**Firma digital:** jordipech76-boop