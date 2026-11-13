# Bitácora de Requerimientos y Auditoría IA — Sesión 14
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Declaración vs Uso de Variables  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Detección de variables no declaradas  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Comparación exacta:** con `strncmp` o comparando solo el inicio, `"x"` y `"xy"` salen iguales. Con `strcmp` tienen que ser idénticos.
> - **Reusar `usar()`:** para saber si es redeclaración, dentro de `declarar()` llamo a `usar()` y así no repito el ciclo.
> - **`TABLA_MAX`:** el contrato no lo dice, pero el arreglo solo tiene 16 lugares. Si no reviso `total`, la declaración 17 escribe fuera del arreglo.
> - **Nombre vacío:** se revisa con `nombre[0] == '\0'`, después de comprobar que no sea `NULL`.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Invariante de definición previa |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion14.h ya existe y no
   se puede modificar; ahí está struct Tabla { const char *nombres[TABLA_MAX];
   int total; } con TABLA_MAX = 16.
R: Ingeniero de compiladores experto en C y análisis semántico.
E: Implementa int declarar(Tabla *tabla, const char *nombre) e
   int usar(const Tabla *tabla, const char *nombre):
   1. declarar devuelve 1 si insertó, 0 si el nombre ya estaba y -1 si recibe
      NULL o un nombre vacío.
   2. usar devuelve 1 solo si el nombre ya fue declarado; si no, 0.
   3. La comparación es exacta: "x" y "xy" son distintos.
O: Solo el .c, empezando con #include "api_sesion14.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion14.h"
#include <stddef.h>
#include <string.h>

static int buscar_indice(const Tabla *tabla, const char *nombre) {
    for (int i = 0; i < tabla->total; i++) {
        if (strcmp(tabla->nombres[i], nombre) == 0) {
            return i;
        }
    }
    return -1;
}

int declarar(Tabla *tabla, const char *nombre) {
    if (tabla == NULL || nombre == NULL || nombre[0] == '\0') {
        return -1;
    }
    if (buscar_indice(tabla, nombre) >= 0) {
        return 0;
    }
    if (tabla->total >= TABLA_MAX) {
        return -1;
    }
    tabla->nombres[tabla->total++] = nombre;
    return 1;
}

int usar(const Tabla *tabla, const char *nombre) {
    if (tabla == NULL || nombre == NULL) {
        return 0;
    }
    return buscar_indice(tabla, nombre) >= 0 ? 1 : 0;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Uso antes de declarar y redeclaración

### Salida del harness
```text
=== AUDITORIA SESION 14 - Declaracion previa y redeclaracion ===
Caso                               esperado:              obtenido:              resultado
[ 1] usar sin declarar                  esperado: 0            obtenido: 0            OK
[ 2] usar despues de declarar           esperado: 1            obtenido: 1            OK
[ 3] redeclarar el mismo nombre         esperado: 0            obtenido: 0            OK
[ 4] total tras redeclarar              esperado: 1            obtenido: 1            OK
[ 5] prefijo no es el mismo nombre      esperado: 0            obtenido: 0            OK
[ 6] declarar nombre vacio              esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion14.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6 y la IA hizo prácticamente lo mismo que yo (hasta revisó la tabla llena: la declaración 17 regresa -1 y `total` se queda en 16, sin escribir fuera del arreglo).
>
> Lo que **omitimos los dos** es que la tabla guarda el **puntero** al nombre, no una copia. Si el que llama reutiliza su buffer, la tabla cambia sola:
>
> ```text
> declarar(&t, buf) con buf = "x";  luego buf = "y"
> usar("x") -> 0     usar("y") -> 1     (nunca se declaró "y")
> ```
>
> En un compilador real el lexer reutiliza buffers todo el tiempo, así que esto daría errores `E01` falsos. La solución sería copiar el nombre (con `malloc` + `memcpy` o un arreglo `char nombres[TABLA_MAX][32]`), pero eso cambia el struct del `.h`, así que lo dejo como propuesta.
>
> **Complejidad:** `usar` y `declarar` son O(n), con n ≤ 16 nombres (búsqueda lineal).

---
**Firma digital:** jordipech76-boop