# Bitácora de Requerimientos y Auditoría IA — Sesión 15
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Bloques Anidados y Ciclo de Vida  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Operaciones Push y Pop de ámbito  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Guardar el padre antes de liberar:** si haces `free(actual)` y luego lees `actual->padre`, ya estás leyendo memoria liberada. Primero guardo el padre en una variable y después libero.
> - **La raíz no se libera:** el ámbito global lo crea el que llama (no viene de `malloc`), así que si `padre` es `NULL` regreso `NULL` sin hacer `free`.
> - **Revisar el `malloc`:** si no hay memoria regresa `NULL`, y hay que revisarlo antes de escribir en el nodo.
> - **Push y pop en pareja:** cada `push` reserva memoria, así que si no se hace su `pop` queda una fuga.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Liberación de memoria al cerrar bloque |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion15.h ya existe y no
   se puede modificar: struct Ambito { const char *nombre; struct Ambito *padre; }.
R: Ingeniero de compiladores experto en C y manejo de memoria.
E: Implementa Ambito *push_ambito(Ambito *actual, const char *nombre) y
   Ambito *pop_ambito(Ambito *actual):
   1. push reserva un ámbito nuevo con malloc, guarda el nombre y lo enlaza al
      actual; devuelve NULL si actual o nombre son NULL.
   2. pop libera el ámbito actual y devuelve su padre.
   3. pop sobre la raíz (padre NULL) devuelve NULL y no libera nada.
O: Solo el .c, empezando con #include "api_sesion15.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion15.h"
#include <stdlib.h>

Ambito *push_ambito(Ambito *actual, const char *nombre) {
    if (actual == NULL || nombre == NULL) {
        return NULL;
    }
    Ambito *nuevo = (Ambito *)malloc(sizeof(Ambito));
    if (nuevo == NULL) {
        return NULL;
    }
    nuevo->nombre = nombre;
    nuevo->padre = actual;
    return nuevo;
}

Ambito *pop_ambito(Ambito *actual) {
    if (actual == NULL || actual->padre == NULL) {
        return NULL;
    }
    Ambito *padre = actual->padre;
    free(actual);
    return padre;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Aislamiento de variables fuera de bloque

### Salida del harness
```text
=== AUDITORIA SESION 15 - Push y pop de ambitos ===
Caso                               esperado:              obtenido:              resultado
[ 1] nombre del ambito nuevo            esperado: if           obtenido: if           OK
[ 2] profundidad con dos push           esperado: 3            obtenido: 3            OK
[ 3] pop devuelve el padre              esperado: 1            obtenido: 1            OK
[ 4] pop sobre la raiz                  esperado: 1            obtenido: 1            OK
[ 5] push con padre NULL                esperado: 1            obtenido: 1            OK
[ 6] profundidad tras pop               esperado: 2            obtenido: 2            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion15.md
```

* [ ] Caso base y límites superados sin *Crash*. (Los dos se caen si se hace `pop` de un ámbito que no vino de `push`; ver reporte.)
* [x] No existen accesos fuera de límites ni *Memory Leaks*. (Valgrind sin errores ni fugas en el harness.)
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6 y el código de la IA es prácticamente igual al mío: guarda el padre antes del `free` y no libera la raíz. Valgrind no marca errores ni fugas.
>
> Lo que **omitimos los dos**: `pop_ambito` cree que todo ámbito con padre vino de `push`. Si alguien arma un ámbito en la pila (`Ambito local = {"bloque", &raiz};`) y le hace `pop`, el `free` truena:
>
> ```text
> pop de un ambito que no vino de push -> free(): invalid pointer  (el programa aborta)
> ```
>
> El contrato dice "no libera memoria que no le pertenece", pero con el struct del `.h` no hay forma de saber de dónde vino un ámbito. La solución sería agregar un campo como `int es_heap` que `push` pone en 1 y `pop` revisa antes del `free`; no lo apliqué porque cambia el contrato.
>
> **Complejidad:** `push` y `pop` son O(1) en tiempo y memoria; recorrer la cadena completa es O(p), con p la profundidad.

---
**Firma digital:** jordipech76-boop