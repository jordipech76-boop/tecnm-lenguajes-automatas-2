# Bitácora de Requerimientos y Auditoría IA — Sesión 18
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Arreglos y Verificación de Límites  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Cálculo de tamaño de arreglo en memoria  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **El índice más alto es `elementos - 1`:** en un arreglo de 5 el último es el 4, así que la condición es `indice < elementos`, no `<=`. Ese era el error clásico que buscaba el harness.
> - **Cero elementos pesa cero:** `elementos < 0` es inválido (-1), pero 0 es válido y da 0 bytes.
> - **El tipo de la multiplicación:** la función regresa `long`, pero si multiplico dos `int` el resultado se desborda antes de convertirse. Por eso hago `(long)elementos * ancho`.
> - **Comparar el tipo:** el nombre del tipo es texto, así que va con `strcmp`.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Indexación entera no negativa |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion18.h ya existe y no
   se puede modificar.
R: Ingeniero de compiladores experto en C y organización de memoria.
E: Implementa long bytes_arreglo(const char *tipo, int elementos) e
   int indice_en_rango(int indice, int elementos):
   1. Anchos: char 1, short 2, int 4, float 4, double 8, puntero 8; otro -> -1.
   2. Elementos negativos -> -1; cero elementos pesa cero bytes.
   3. indice_en_rango devuelve 1 si 0 <= indice < elementos, si no 0.
O: Solo el .c, empezando con #include "api_sesion18.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion18.h"
#include <stddef.h>
#include <string.h>

static int ancho_tipo(const char *tipo) {
    if (strcmp(tipo, "char") == 0)    return 1;
    if (strcmp(tipo, "short") == 0)   return 2;
    if (strcmp(tipo, "int") == 0)     return 4;
    if (strcmp(tipo, "float") == 0)   return 4;
    if (strcmp(tipo, "double") == 0)  return 8;
    if (strcmp(tipo, "puntero") == 0) return 8;
    return -1;
}

long bytes_arreglo(const char *tipo, int elementos) {
    if (tipo == NULL || elementos < 0) {
        return -1;
    }
    int ancho = ancho_tipo(tipo);
    if (ancho < 0) {
        return -1;
    }
    return elementos * ancho;
}

int indice_en_rango(int indice, int elementos) {
    return indice >= 0 && indice < elementos;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Límites estáticos y dimensiones inválidas

### Salida del harness
```text
=== AUDITORIA SESION 18 - Tamano de arreglo y limites ===
Caso                               esperado:              obtenido:              resultado
[ 1] int[10]                            esperado: 40           obtenido: 40           OK
[ 2] double[3]                          esperado: 24           obtenido: 24           OK
[ 3] int[-4]                            esperado: -1           obtenido: -1           OK
[ 4] indice 0 de 5                      esperado: 1            obtenido: 1            OK
[ 5] indice 5 de 5                      esperado: 0            obtenido: 0            OK
[ 6] indice negativo                    esperado: 0            obtenido: 0            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion18.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. La IA regresa `long`, pero **multiplica en `int`**: `return elementos * ancho;` con los dos `int` se desborda antes de convertirse a `long`. El harness no lo ve porque usa arreglos chicos:
>
> | Caso | IA | Mi artesano |
> | :--- | :--- | :--- |
> | `double[300000000]` | -1894967296 | 2400000000 |
> | `int[INT_MAX]` | -4 | 8589934588 |
>
> Un tamaño negativo haría creer al compilador que el arreglo no ocupa espacio. Lo solucioné convirtiendo antes de multiplicar:
>
> ```c
> return (long)elementos * ancho;
> ```
>
> `indice_en_rango` está bien en las dos versiones (`0 <= indice < elementos`), incluso con 0 elementos.
>
> **Complejidad:** O(1); `bytes_arreglo` compara contra una lista fija de 6 tipos.

---
**Firma digital:** jordipech76-boop