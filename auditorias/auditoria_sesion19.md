# Bitácora de Requerimientos y Auditoría IA — Sesión 19
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Estructuras (struct) y Miembros  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Offsets de miembros en estructuras  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **El padding:** cada campo empieza en un múltiplo de su propio ancho. En `{char a; int b;}` el `char` ocupa el byte 0, pero el `int` no puede empezar en el 1, así que se dejan 3 bytes vacíos y `b` queda en 4.
> - **Redondear hacia arriba:** hice una función `alinear(x, a)` que hace `(x + a - 1) / a * a`; la uso antes de cada campo y al final.
> - **El padding del final:** el tamaño total se redondea al ancho del campo más grande. `{double; char}` no pesa 9, pesa 16, para que en un arreglo el siguiente `double` quede alineado.
> - **Struct vacío:** con 0 campos el tamaño es 0, no se redondea.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Alineación de memoria por palabras |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion19.h ya existe y no
   se puede modificar: struct Campo { const char *nombre; const char *tipo; }.
R: Ingeniero de compiladores experto en C y organización de memoria.
E: Implementa int offset_campo(const Campo *campos, int total, const char *nombre)
   e int tamano_struct(const Campo *campos, int total):
   1. Anchos: char 1, short 2, int 4, float 4, long 8, double 8, puntero 8.
   2. Cada campo se alinea a su propio ancho antes de asignarle offset.
   3. El tamaño total se redondea al múltiplo del campo más grande; 0 campos = 0.
   4. Campo inexistente, lista NULL o tipo desconocido devuelven -1.
O: Solo el .c, empezando con #include "api_sesion19.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion19.h"
#include <stddef.h>
#include <string.h>

static int tamano_tipo(const char *tipo) {
    static const struct { const char *nombre; int ancho; } tabla[] = {
        {"char", 1}, {"short", 2}, {"int", 4}, {"float", 4},
        {"long", 8}, {"double", 8}, {"puntero", 8}
    };
    for (size_t i = 0; i < sizeof(tabla) / sizeof(tabla[0]); i++) {
        if (strcmp(tabla[i].nombre, tipo) == 0) {
            return tabla[i].ancho;
        }
    }
    return -1;
}

int offset_campo(const Campo *campos, int total, const char *nombre) {
    if (campos == NULL || nombre == NULL) {
        return -1;
    }
    int offset = 0;
    for (int i = 0; i < total; i++) {
        int ancho = tamano_tipo(campos[i].tipo);
        if (ancho < 0) {
            return -1;
        }
        offset = (offset + ancho - 1) / ancho * ancho;
        if (strcmp(campos[i].nombre, nombre) == 0) {
            return offset;
        }
        offset += ancho;
    }
    return -1;
}

int tamano_struct(const Campo *campos, int total) {
    if (campos == NULL) {
        return -1;
    }
    int offset = 0;
    int max_alineacion = 1;
    for (int i = 0; i < total; i++) {
        int ancho = tamano_tipo(campos[i].tipo);
        if (ancho < 0) {
            return -1;
        }
        offset = (offset + ancho - 1) / ancho * ancho + ancho;
        if (ancho > max_alineacion) {
            max_alineacion = ancho;
        }
    }
    return (offset + max_alineacion - 1) / max_alineacion * max_alineacion;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Acceso a miembros inexistentes

### Salida del harness
```text
=== AUDITORIA SESION 19 - Offsets de miembros y tamano de struct ===
Caso                               esperado:              obtenido:              resultado
[ 1] struct char e int: b               esperado: 4            obtenido: 4            OK
[ 2] struct de tres campos: c           esperado: 8            obtenido: 8            OK
[ 3] tamano total con padding           esperado: 12           obtenido: 12           OK
[ 4] double seguido de char             esperado: 16           obtenido: 16           OK
[ 5] miembro inexistente                esperado: -1           obtenido: -1           OK
[ 6] struct sin campos                  esperado: 0            obtenido: 0            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion19.md
```

* [ ] Caso base y límites superados sin *Crash*. (La versión de la IA se cae con un campo sin nombre o sin tipo; ver reporte.)
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6 y los dos coinciden con lo que calcula `gcc` (por ejemplo `{double; char; char}` = 16). La IA **no revisa las entradas raras** que el harness no prueba:
>
> | Caso | IA | Mi artesano |
> | :--- | :--- | :--- |
> | `tamano_struct(campos, -1)` | 0 | -1 |
> | campo con nombre `NULL` (buscar otro) | se cae (*Segmentation fault*) | 4 |
> | campo con tipo `NULL` | se cae | -1 |
>
> 1. **Total negativo:** la IA no lo revisa, el ciclo no corre y regresa 0, como si fuera un struct vacío válido.
> 2. **`NULL` en el nombre o el tipo:** la IA llama `strcmp` con `NULL` y truena. En mi versión `ancho()` regresa -1 si el tipo es `NULL`, y salto los campos sin nombre al buscar.
>
> **Complejidad:** O(n) en tiempo, con n el número de campos (un recorrido), y O(1) de memoria.

---
**Firma digital:** jordipech76-boop