# Bitácora de Requerimientos y Auditoría IA — Sesión 01
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Encuadre, Diagnóstico de Punteros y Metodología IA  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Inversión de buffer in-place (char*)  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Lo hice con dos índices (inicio y final) y un char temporal. Me atoré con la cadena vacía: `strlen` regresa `size_t`, entonces si la longitud es 0, `len - 1` da un número gigante y se sale del buffer. Lo arreglé checando `len < 2` antes de empezar. También tuve que checar `NULL` primero, porque `strlen(NULL)` truena solito.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C11 / GCC con `-Wall -Wextra -Werror`, sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Punteros, O(n) tiempo, O(1) memoria, sin malloc, aguantar NULL y "" |
| **Output (O)** | Solo código modular (sin main) y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
Actúa como ingeniero de software de sistemas y compiladores.
En C11 con GCC (-Wall -Wextra -Werror) y sin bibliotecas no estándar, escribe
void invertir(char *s); que invierta la cadena in-place usando punteros,
en O(n) tiempo y O(1) memoria, sin malloc ni arreglos auxiliares.
Debe funcionar con NULL, cadena vacía y longitudes pares e impares.
Incluye "api_sesion01.h" y no pongas main().
Devuelve solo el código y un análisis breve de complejidad.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion01.h"
#include <stddef.h>

/*
 * invertir: invierte una cadena terminada en '\0' sobre su mismo buffer.
 * Tiempo O(n), memoria auxiliar O(1).
 */
void invertir(char *s) {
    if (s == NULL || *s == '\0') {
        return;
    }

    char *inicio = s;
    char *fin = s;

    /* avanzar fin hasta el ultimo caracter valido */
    while (*(fin + 1) != '\0') {
        fin++;
    }

    while (inicio < fin) {
        char tmp = *inicio;
        *inicio = *fin;
        *fin = tmp;
        inicio++;
        fin--;
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Valores NULL, buffers vacios y longitudes impares

### Salida del harness
Versión Artesano:
```text
=== AUDITORIA SESION 01 - Inversion de buffer in-place ===
Caso                               esperado:              obtenido:              resultado
[ 1] invertir("HOLA")                   esperado: ALOH         obtenido: ALOH         OK
[ 2] invertir("abc")                    esperado: cba          obtenido: cba          OK
[ 3] invertir("")                       esperado:              obtenido:              OK
[ 4] invertir("AB")                     esperado: BA           obtenido: BA           OK
[ 5] invertir("a")                      esperado: a            obtenido: a            OK
[ 6] invertir(NULL)                     esperado: sin crash    obtenido: sin crash    OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion01.md
```

Versión IA:
```text
=== AUDITORIA SESION 01 - Inversion de buffer in-place ===
Caso                               esperado:              obtenido:              resultado
[ 1] invertir("HOLA")                   esperado: ALOH         obtenido: ALOH         OK
[ 2] invertir("abc")                    esperado: cba          obtenido: cba          OK
[ 3] invertir("")                       esperado:              obtenido:              OK
[ 4] invertir("AB")                     esperado: BA           obtenido: BA           OK
[ 5] invertir("a")                      esperado: a            obtenido: a            OK
[ 6] invertir(NULL)                     esperado: sin crash    obtenido: sin crash    OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion01.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> La IA pasó los 6 casos, pero no dijo nada de los literales de solo lectura. Si haces `char *s = "HOLA"; invertir(s);` compila sin warnings, pero al correrlo da *Segmentation fault*, porque el literal está en `.rodata` y no se puede escribir. El harness no lo detecta porque todos sus casos usan `char a[] = "..."`. Lo probé con un `main` aparte y sí tronó. Esto no se puede arreglar dentro de la función, así que lo dejo como precondición: hay que pasarle un arreglo modificable, no un literal.

---
**Firma digital:** jordipech76-boop
