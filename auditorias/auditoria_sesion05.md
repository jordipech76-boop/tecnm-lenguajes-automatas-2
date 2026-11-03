# Bitácora de Requerimientos y Auditoría IA — Sesión 05
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Recorridos del AST y Notación Polaca  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Impresión en Postorden / RPN  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Dónde poner el código:** al principio no sabía qué parte de la plantilla cambiar. Hay que dejar los `#include` y reemplazar solo la función con el `TODO` y los `(void)n; (void)cap;`.
> - **Escribir sin `printf`:** cada token se pega al final del buffer usando `strlen(buf)` para saber dónde voy y `snprintf` con el espacio que queda (`cap - usado`) para no pasarme.
> - **Los espacios:** si pones espacio después de cada token sobra uno al final (`"8 3 - "`) y el harness compara el texto exacto, así que solo pongo espacio si el buffer ya tiene algo.
> - **La recursión:** como `postorden` limpia el buffer al empezar, hice una función auxiliar recursiva para que no se borre en cada llamada.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Invariantes de recorrido recursivo |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion05.h ya existe y
   no se puede modificar; ahí está el struct ASTNode (tipo, valor, op, izq, der)
   con TipoNodo NODO_NUM / NODO_BINOP.
R: Ingeniero de software experto en C y compiladores, enfocado en código seguro.
E: Implementa void postorden(ASTNode *n, char *buf, int cap):
   1. Recorrer en postorden (izq -> der -> raíz) de forma recursiva.
   2. Escribir la RPN en buf, tokens separados por un solo espacio.
   3. Sin espacios al inicio ni al final (ej. "3 5 2 * +").
   4. Árbol NULL -> cadena vacía.
   5. Nunca más de cap - 1 caracteres y siempre terminar en '\0'.
   6. No usar printf.
O: Solo el .c en C11, empezando con #include "api_sesion05.h", sin main()
   y sin printf.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion05.h"
#include <stdio.h>
#include <string.h>

static void postorden_recursivo(ASTNode *n, char *buf, int cap) {
    if (n == NULL) return;
    postorden_recursivo(n->izq, buf, cap);
    postorden_recursivo(n->der, buf, cap);

    char temp[32];
    if (n->tipo == NODO_NUM) {
        snprintf(temp, sizeof(temp), "%d ", n->valor);
    } else if (n->tipo == NODO_BINOP) {
        snprintf(temp, sizeof(temp), "%c ", n->op);
    } else {
        return;
    }

    int len = strlen(buf);
    int temp_len = strlen(temp);
    if (len + temp_len < cap) {
        strcat(buf, temp);
    }
}

void postorden(ASTNode *n, char *buf, int cap) {
    if (buf == NULL || cap <= 0) return;
    buf[0] = '\0';
    if (n == NULL) return;
    postorden_recursivo(n, buf, cap);

    int len = strlen(buf);
    if (len > 0 && buf[len - 1] == ' ') {
        buf[len - 1] = '\0';
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Validación contra desbordamiento de pila

### Salida del harness
```text
=== AUDITORIA SESION 05 - Recorrido postorden y RPN ===
Caso                               esperado:              obtenido:              resultado
[ 1] hoja 42                            esperado: 42           obtenido: 42           OK
[ 2] 3+(5*2)                            esperado: 3 5 2 * +    obtenido: 3 5 2 * +    OK
[ 3] arbol NULL                         esperado:              obtenido:              OK
[ 4] (1+2)*(3+4)
     esperado: 1 2 + 3 4 + *
     obtenido: 1 2 + 3 4 + *
     -> OK
[ 5] 8-3                                esperado: 8 3 -        obtenido: 8 3 -        OK

RESULTADO: 5/5 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion05.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 5/5 y ninguno se sale del buffer. El problema aparece cuando **la RPN no cabe**, algo que el harness no prueba porque siempre usa buffers de 32 o 64. Lo probé con `1+1000` en un buffer de 6:
>
> | Versión | Resultado | Qué pasa |
> | :--- | :--- | :--- |
> | IA | `"1 +"` | se salta el `1000` y sigue con el `+` |
> | Mi artesano | `"1 100"` | corta el `1000` a la mitad |
>
> Las dos entregan una RPN incorrecta sin avisar. La solución es revisar si el token cabe **completo** antes de escribirlo y, si no, dejar de escribir:
>
> ```c
> if (usado + necesito > cap - 1) { *lleno = 1; return; }
> ```
>
> Así queda `"1"`: incompleto, pero sin tokens saltados ni números cortados.
>
> **Complejidad:** O(n) en tiempo (cada nodo se visita una vez) y O(h) de pila por la recursión, donde h es la altura del árbol.

---
**Firma digital:** jordipech76-boop
