# Bitácora de Requerimientos y Auditoría IA — Sesión 07
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Evaluación de Expresiones Aritméticas  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Evaluador recursivo de operaciones  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Revisar antes de dividir:** si haces `a / b` y luego preguntas si `b` es 0, el programa ya se cayó. También el `%` divide, así que lleva la misma revisión.
> - **Propagar el error:** cuando un hijo marca error hay que regresar de inmediato, por eso después de cada `evaluar()` reviso `*error`.
> - **El puntero del error:** se escribe con `*error = 1`, no con `error = 1`.
> - **Error al pegar en Colab:** en la celda de la IA dejé la plantilla arriba y quedaron dos `%%writefile`. Luego quedó un pedazo de comentario suelto (`* Implementacion ... */`) fuera del `/* */` y el `#include` repetido. Lo arreglé dejando la celda solo con un `%%writefile` y el código limpio.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Manejo estricto de división entre cero |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion07.h ya existe y
   no se puede modificar; ahí está el struct ASTNode (tipo, valor, op, izq,
   der) con TipoNodo NODO_NUM / NODO_BINOP.
R: Ingeniero de compiladores experto en C y evaluación de expresiones.
E: Implementa int evaluar(ASTNode *n, int *error):
   1. Evalúa el árbol de forma recursiva con + - * / %.
   2. División o módulo entre cero: *error = 1 sin ejecutar la división.
   3. Nodo NULL u operador desconocido: *error = 2.
   4. El error se propaga sin seguir calculando a los padres.
   5. Nunca debe caerse.
O: Solo el .c, empezando con #include "api_sesion07.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion07.h"
#include <limits.h>
#include <stddef.h>

int evaluar(ASTNode *n, int *error) {
    if (error != NULL && *error != 0) return 0;

    if (n == NULL) {
        if (error != NULL) *error = 2;
        return 0;
    }
    if (n->tipo == NODO_NUM) return n->valor;

    if (n->tipo == NODO_BINOP) {
        int izq_val = evaluar(n->izq, error);
        if (error != NULL && *error != 0) return 0;
        int der_val = evaluar(n->der, error);
        if (error != NULL && *error != 0) return 0;

        switch (n->op) {
            case '+': return izq_val + der_val;
            case '-': return izq_val - der_val;
            case '*': return izq_val * der_val;
            case '/':
                if (der_val == 0) { if (error != NULL) *error = 1; return 0; }
                if (izq_val == INT_MIN && der_val == -1) { if (error != NULL) *error = 1; return 0; }
                return izq_val / der_val;
            case '%':
                if (der_val == 0) { if (error != NULL) *error = 1; return 0; }
                if (izq_val == INT_MIN && der_val == -1) { if (error != NULL) *error = 1; return 0; }
                return izq_val % der_val;
            default:
                if (error != NULL) *error = 2;
                return 0;
        }
    }
    if (error != NULL) *error = 2;
    return 0;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Expresiones complejas y división por 0

### Salida del harness
```text
=== AUDITORIA SESION 07 - Evaluador recursivo con deteccion de errores ===
Caso                               esperado:              obtenido:              resultado
[ 1] (2+3)*4                            esperado: 20           obtenido: 20           OK
[ 2] 7/0 -> error 1                     esperado: 1            obtenido: 1            OK
[ 3] 7%0 -> error 1                     esperado: 1            obtenido: 1            OK
[ 4] arbol NULL -> error 2              esperado: 2            obtenido: 2            OK
[ 5] 10-2-3                             esperado: 5            obtenido: 5            OK
[ 6] (8/2)+1                            esperado: 5            obtenido: 5            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion07.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. La IA sí cubre división entre 0, `INT_MIN / -1` (que también tumba el programa) y `error` en `NULL`. Lo que **omitió** es el **desbordamiento** en `+`, `-` y `*`, que el harness no prueba:
>
> | Caso | Resultado de la IA | Real |
> | :--- | :--- | :--- |
> | `INT_MAX + 1` | `-2147483648`, `error = 0` | 2147483648 |
> | `100000 * 100000` | `1410065408`, `error = 0` | 10000000000 |
>
> No se cae, pero entrega un número equivocado sin avisar (mi artesano tiene el mismo problema). La solución es revisar con `<limits.h>` antes de operar, por ejemplo en la suma:
>
> ```c
> if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) { *error = 1; return 0; }
> ```
>
> No la apliqué porque el contrato no tiene un código de error para desbordamiento; lo dejo como propuesta.
>
> **Complejidad:** O(n) en tiempo (visita cada nodo una vez) y O(h) de pila por la recursión. Sin memoria dinámica.

---
**Firma digital:** jordipech76-boop
