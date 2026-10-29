# Bitácora de Requerimientos y Auditoría IA — Sesión 04
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Anatomía de un Árbol Sintáctico (AST)  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Estructura de nodo binario (ASTNode)  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Tuve dos errores al compilar:
>
> 1. **`implicit declaration of function 'malloc'`**: usé `malloc` pero dejé los includes de la plantilla (`<stdio.h>` y `<string.h>`), y `malloc` está en `<stdlib.h>`. Como el harness compila con `-Werror`, ese aviso se volvió error. Lo arreglé agregando `#include <stdlib.h>`.
> 2. **`undefined reference to 'crear_numero'`** (y `crear_binario` y `evaluar`): me pasó al compilar la versión de la IA; el enlazador no encontraba las funciones porque había pegado el código en la celda `%%writefile ia_sesion04_c.c`, pero no la había ejecutado, y el archivo seguía siendo el viejo. Aprendí que en Colab hay que correr esa celda (debe salir `Overwriting ...`) antes de compilar.
>
> Del código en sí, lo más importante fue llenar todos los campos del nodo (los hijos en `NULL` si es número), usar `->` porque los nodos son punteros, y en `evaluar` respetar el orden: primero los dos hijos y luego el operador, porque `10-4` no es lo mismo que `4-10`.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Gramática E -> E + T | T con tipos de nodo |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -Wall -Wextra -Werror, Linux. El archivo api_sesion04.h ya existe
   y no se puede modificar; ahí están TipoNodo (NODO_NUM, NODO_BINOP) y el
   struct ASTNode (tipo, valor, op, izq, der).
R: Eres un ingeniero de compiladores con experiencia en C y árboles sintácticos.
E: Implementa exactamente
     ASTNode *crear_numero(int v);
     ASTNode *crear_binario(char op, ASTNode *izq, ASTNode *der);
     int evaluar(ASTNode *n);
   Reglas: cada nodo se reserva en el heap con malloc; usa TipoNodo para
   distinguir número de operación; crear_binario respeta el orden de los
   hijos; evaluar evalúa ambos hijos y luego aplica + - o *; evaluar(NULL)
   devuelve -1 sin caerse.
O: Solo el contenido del .c, con #include "api_sesion04.h", sin main()
   y sin printf.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion04.h"
#include <stdlib.h>

ASTNode *crear_numero(int v) {
    ASTNode *nodo = malloc(sizeof(ASTNode));
    if (nodo == NULL) return NULL;
    nodo->tipo = NODO_NUM;
    nodo->valor = v;
    nodo->op = '\0';
    nodo->izq = NULL;
    nodo->der = NULL;
    return nodo;
}

ASTNode *crear_binario(char op, ASTNode *izq, ASTNode *der) {
    ASTNode *nodo = malloc(sizeof(ASTNode));
    if (nodo == NULL) return NULL;
    nodo->tipo = NODO_BINOP;
    nodo->valor = 0;
    nodo->op = op;
    nodo->izq = izq;
    nodo->der = der;
    return nodo;
}

int evaluar(ASTNode *n) {
    if (n == NULL) return -1;
    if (n->tipo == NODO_NUM) return n->valor;

    int izquierdo = evaluar(n->izq);
    int derecho = evaluar(n->der);

    switch (n->op) {
        case '+': return izquierdo + derecho;
        case '-': return izquierdo - derecho;
        case '*': return izquierdo * derecho;
        default:  return -1;
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Árbol vacío y evaluación de nodo raíz

### Salida del harness
```text
=== AUDITORIA SESION 04 - Nodo binario de AST ===
Caso                               esperado:              obtenido:              resultado
[ 1] evaluar(nodo 42)                   esperado: 42           obtenido: 42           OK
[ 2] evaluar(3+5*2)                     esperado: 13           obtenido: 13           OK
[ 3] hijo izquierdo de (* 5 2)          esperado: 5            obtenido: 5            OK
[ 4] operador guardado                  esperado: *            obtenido: *            OK
[ 5] evaluar(NULL)                      esperado: -1           obtenido: -1           OK
[ 6] evaluar(10-4)                      esperado: 6            obtenido: 6            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion04.md
```

* [x] Caso base y límites superados sin *Crash*.
* [ ] No existen accesos fuera de límites ni *Memory Leaks*. (No hay accesos fuera de límites, pero sí fugas: el contrato no trae función para liberar el árbol y el harness nunca hace `free`. Valgrind marca 480 bytes perdidos.)
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Las dos versiones sacaron 6/6. La IA sí cubre `evaluar(NULL)`, pero **no revisa que un nodo binario tenga sus dos hijos**. Si armas `crear_binario('+', crear_numero(3), NULL)` (o si un `malloc` de un hijo falla y regresa `NULL`), el hijo vacío vale -1 y la suma da **2** sin ningún aviso, como si fuera un resultado válido. El harness no lo prueba porque siempre arma árboles completos; lo vi leyendo `evaluar` y lo comprobé con un `main` de prueba: la versión de la IA regresa 2 y la mía -1. Lo solucioné agregando antes de evaluar los hijos:
>
> ```c
> if (n->izq == NULL || n->der == NULL) return -1;
> ```
>
> Otra cosa que noté: usar -1 como error choca con resultados reales, porque `1-2` también da -1 y no se puede saber si fue error o no. Para arreglarlo bien haría falta un parámetro `int *error` (como el que viene en la sección 2 de la guía de C), pero eso cambiaría la firma del contrato.
>
> Complejidad: crear un nodo es O(1); `evaluar` es O(n) en tiempo (visita cada nodo una vez) y O(h) de pila por la recursión, donde h es la altura del árbol.

---
**Firma digital:** jordipech76-boop
