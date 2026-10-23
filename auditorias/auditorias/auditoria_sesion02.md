# Bitácora de Requerimientos y Auditoría IA — Sesión 02
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Gestión en GitHub Web y Nodos Dinámicos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Lista enlazada de tokens en Heap  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Me atoré porque dejé pegados restos del TODO (`return NULL; }`) y esa `}` cerraba la función antes de tiempo, así que mi código quedó afuera. También me faltaba la `}` del `while` y tenía funciones repetidas. Lo arreglé borrando lo del TODO y contando llaves. En `liberar_lista` guardé el `siguiente` antes del `free` para no hacer use-after-free.

```text
error: initializer element is not constant
error: 'lexema' undeclared here (not in a function)
error: redefinition of 'nuevo'
```

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C11 / GCC con `-Wall -Wextra -Werror`, sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Lista enlazada en heap con malloc/free, sin fugas ni use-after-free, aguantar NULL |
| **Output (O)** | Solo código modular (sin main) y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
Actúa como ingeniero de software de sistemas y compiladores.
En C11 con GCC (-Wall -Wextra -Werror) y sin bibliotecas no estándar, implementa
estas funciones para una lista enlazada de tokens en el heap, usando este struct:

typedef struct NodoToken {
    char lexema[16];
    struct NodoToken *siguiente;
} NodoToken;

NodoToken *crear_nodo(const char *lexema);
void agregar_token(NodoToken **cabeza, const char *lexema);
int contar_tokens(NodoToken *cabeza);
void liberar_lista(NodoToken *cabeza);

Cada nodo con malloc, el último nodo con siguiente = NULL, agregar al final,
contar_tokens(NULL) debe dar 0 y liberar_lista(NULL) no debe tronar.
Sin fugas de memoria ni use-after-free. Incluye "api_sesion02.h" y no pongas main().
Devuelve solo el código y un análisis breve de complejidad.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion02.h"
#include <stdlib.h>
#include <string.h>

/* Crea un nodo en el heap con una copia del lexema. O(1). */
NodoToken *crear_nodo(const char *lexema) {
    NodoToken *nodo = (NodoToken *)malloc(sizeof(NodoToken));
    if (nodo == NULL) {
        return NULL;
    }
    strncpy(nodo->lexema, lexema, sizeof(nodo->lexema));
    nodo->siguiente = NULL;
    return nodo;
}

/* Agrega un token al final de la lista. O(n). */
void agregar_token(NodoToken **cabeza, const char *lexema) {
    NodoToken *nuevo = crear_nodo(lexema);
    if (nuevo == NULL) {
        return;
    }
    if (*cabeza == NULL) {
        *cabeza = nuevo;
        return;
    }
    NodoToken *actual = *cabeza;
    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }
    actual->siguiente = nuevo;
}

/* Cuenta los nodos de la lista. O(n). */
int contar_tokens(NodoToken *cabeza) {
    int contador = 0;
    for (NodoToken *p = cabeza; p != NULL; p = p->siguiente) {
        contador++;
    }
    return contador;
}

/* Libera todos los nodos guardando el siguiente antes de cada free. O(n). */
void liberar_lista(NodoToken *cabeza) {
    NodoToken *actual = cabeza;
    while (actual != NULL) {
        NodoToken *temp = actual->siguiente;
        free(actual);
        actual = temp;
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Prevención de Use-After-Free y cadenas nulas

### Salida del harness
Versión Artesano:
```text
=== AUDITORIA SESION 02 - Lista enlazada de tokens en heap ===
Caso                               esperado:              obtenido:              resultado
[ 1] crear_nodo("if")->lexema           esperado: if           obtenido: if           OK
[ 2] contar_tokens(3 tokens)            esperado: 3            obtenido: 3            OK
[ 3] orden: segundo token               esperado: dos          obtenido: dos          OK
[ 4] ultimo->siguiente == NULL          esperado: 1            obtenido: 1            OK
[ 5] contar_tokens(NULL)                esperado: 0            obtenido: 0            OK
[ 6] liberar_lista(NULL)                esperado: sin crash    obtenido: sin crash    OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion02.md
```

Versión IA:
```text
=== AUDITORIA SESION 02 - Lista enlazada de tokens en heap ===
Caso                               esperado:              obtenido:              resultado
[ 1] crear_nodo("if")->lexema           esperado: if           obtenido: if           OK
[ 2] contar_tokens(3 tokens)            esperado: 3            obtenido: 3            OK
[ 3] orden: segundo token               esperado: dos          obtenido: dos          OK
[ 4] ultimo->siguiente == NULL          esperado: 1            obtenido: 1            OK
[ 5] contar_tokens(NULL)                esperado: 0            obtenido: 0            OK
[ 6] liberar_lista(NULL)                esperado: sin crash    obtenido: sin crash    OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion02.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> La IA copió los 16 bytes con `strncpy` y no puso el `\0`. Con un lexema largo como `"identificador_largo"` la cadena quedó sin terminar y salió basura (`strlen` dio 22 en vez de 15). El harness no lo detecta porque sus lexemas son cortos. Lo arreglé copiando solo 15 y poniendo el `\0` a mano, como en mi versión.

---
**Firma digital:** jordipech76-boop
