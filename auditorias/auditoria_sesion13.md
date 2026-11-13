# Bitácora de Requerimientos y Auditoría IA — Sesión 13
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Manejo de Ámbitos (Scope Global/Local)  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Pila de tablas de símbolos (Stack Scopes)  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Dos tipos de búsqueda:** `declarar()` solo busca en el ámbito actual (por eso se permite el *shadowing*), pero `buscar()` sube por `padre` hasta la raíz. Hice una función `esta_aqui()` para no repetir el ciclo.
> - **Qué regresa `buscar()`:** no regresa el símbolo, regresa el **nombre del ámbito** donde lo encontró (`"global"`, `"bloque"`).
> - **El orden de las revisiones:** primero reviso si el nombre ya estaba (0) y después si el ámbito está lleno (-1).
> - **`AMBITO_MAX`:** el arreglo solo tiene 8 lugares, así que hay que revisar `total` antes de escribir.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Ocultamiento de variables (Shadowing) |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion13.h ya existe y no
   se puede modificar; ahí está struct Ambito { const char *nombre;
   const char *nombres[AMBITO_MAX]; int total; struct Ambito *padre; } con
   AMBITO_MAX = 8.
R: Ingeniero de compiladores experto en C y manejo de ámbitos.
E: Implementa int declarar(Ambito *ambito, const char *nombre) y
   const char *buscar(const Ambito *ambito, const char *nombre):
   1. declarar solo revisa el ámbito actual; devuelve 1 si insertó, 0 si ya
      estaba ahí y -1 si está lleno, recibe NULL o un nombre vacío.
   2. buscar empieza en el ámbito actual y sube por padre; devuelve el nombre
      del ámbito más cercano donde está, o NULL.
O: Solo el .c, empezando con #include "api_sesion13.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion13.h"
#include <stddef.h>
#include <string.h>

int declarar(Ambito *ambito, const char *nombre) {
    if (ambito == NULL || nombre == NULL || nombre[0] == '\0') {
        return -1;
    }
    if (ambito->total >= AMBITO_MAX) {
        return -1;
    }
    for (int i = 0; i < ambito->total; i++) {
        if (strcmp(ambito->nombres[i], nombre) == 0) {
            return 0;
        }
    }
    ambito->nombres[ambito->total] = nombre;
    ambito->total++;
    return 1;
}

const char *buscar(const Ambito *ambito, const char *nombre) {
    if (nombre == NULL) {
        return NULL;
    }
    for (const Ambito *actual = ambito; actual != NULL; actual = actual->padre) {
        for (int i = 0; i < actual->total; i++) {
            if (strcmp(actual->nombres[i], nombre) == 0) {
                return actual->nombre;
            }
        }
    }
    return NULL;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Búsqueda jerárquica de identificadores

### Salida del harness
```text
=== AUDITORIA SESION 13 - Pila de ambitos y busqueda jerarquica ===
Caso                               esperado:              obtenido:              resultado
[ 1] declarar(&g, "x")                  esperado: 1            obtenido: 1            OK
[ 2] buscar en el mismo ambito          esperado: global       obtenido: global       OK
[ 3] buscar subiendo al padre           esperado: global       obtenido: global       OK
[ 4] shadowing en el hijo               esperado: bloque       obtenido: bloque       OK
[ 5] tres niveles hasta la raiz         esperado: raiz         obtenido: raiz         OK
[ 6] nombre no declarado                esperado: (nulo)       obtenido: (nulo)       OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion13.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. Encontré dos cosas que el harness no prueba:
>
> 1. **Ámbito lleno y nombre repetido:** la IA revisa primero si está lleno y regresa -1, aunque el nombre ya estuviera declarado. El contrato dice 0 cuando "ya estaba en ese ámbito":
>
>    | Caso (ámbito con 8 nombres) | IA | Mi artesano |
>    | :--- | :--- | :--- |
>    | redeclarar `"a"`, que ya estaba | -1 | 0 |
>    | declarar `"z"`, nuevo | -1 | -1 |
>
>    Lo solucioné revisando el duplicado **antes** que la capacidad.
> 2. **Se guarda el puntero, no una copia (los dos):** si declaro con un `char buf[]` y luego cambio `buf`, el nombre en la tabla también cambia y `buscar("tmp")` ya no lo encuentra. El struct del contrato guarda `const char *`, así que el que llama debe mantener vivo el texto; no lo pude cambiar sin tocar el `.h`.
>
> **Complejidad:** `declarar` es O(k), con k ≤ 8 nombres por ámbito; `buscar` es O(p·k), donde p es la profundidad de ámbitos.

---
**Firma digital:** jordipech76-boop