# Bitácora de Requerimientos y Auditoría IA — Sesión 17
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Comprobación Semántica de Retornos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Validación de tipo de sentencia return  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Orden de los tipos:** para saber si se puede convertir hice un arreglo `void, char, short, int, long, float, double`; la posición es el "ancho". Se acepta si el tipo de la expresión no es más ancho que el de la función.
> - **Primero lo incoherente:** cero rutas o más returns que rutas regresan -1 antes de revisar tipos.
> - **Las funciones `void` son distintas:** devolver un valor es error (1), pero **no** necesitan `return` en todas las rutas, así que ahí no aplica el error 2.
> - **`return;` sin valor:** lo tomo como `tipo_expresion` en `NULL` (o `"void"`). En una función `int` eso es error de tipo.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Rutas de ejecución con retorno garantizado |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion17.h ya existe y no
   se puede modificar.
R: Ingeniero de compiladores experto en C y análisis semántico.
E: Implementa int diagnosticar_retorno(const char *tipo_funcion,
        const char *tipo_expresion, int rutas_totales, int rutas_con_return):
   1. Devuelve 0 si todo está bien, 1 si el tipo es incompatible y 2 si una ruta
      queda sin retorno.
   2. La conversión solo se acepta si el destino es igual o más ancho
      (char < short < int < long < float < double).
   3. Devolver un valor en una función void es siempre error de tipo.
   4. tipo_funcion NULL, cero rutas o más retornos que rutas devuelven -1.
O: Solo el .c, empezando con #include "api_sesion17.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion17.h"
#include <stddef.h>
#include <string.h>

static int rango_tipo(const char *tipo) {
    static const char *const tipos[] = {"char", "short", "int", "long", "float", "double"};
    for (int i = 0; i < 6; i++) {
        if (strcmp(tipo, tipos[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int diagnosticar_retorno(const char *tipo_funcion, const char *tipo_expresion, int rutas_totales, int rutas_con_return) {
    if (tipo_funcion == NULL || tipo_expresion == NULL) {
        return -1;
    }
    if (rutas_totales <= 0 || rutas_con_return < 0 || rutas_con_return > rutas_totales) {
        return -1;
    }
    if (strcmp(tipo_funcion, "void") == 0) {
        return 1;   /* devolver un valor en una funcion void es un error */
    }
    int destino = rango_tipo(tipo_funcion);
    int origen = rango_tipo(tipo_expresion);
    if (destino < 0 || origen < 0 || origen > destino) {
        return 1;
    }
    if (rutas_con_return < rutas_totales) {
        return 2;
    }
    return 0;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Funciones sin retorno o retornos mixtos

### Salida del harness
```text
=== AUDITORIA SESION 17 - Validacion de retornos y rutas ===
Caso                               esperado:              obtenido:              resultado
[ 1] int con int y dos rutas            esperado: 0            obtenido: 0            OK
[ 2] void devuelve valor                esperado: 1            obtenido: 1            OK
[ 3] ruta sin retorno                   esperado: 2            obtenido: 2            OK
[ 4] double acepta int                  esperado: 0            obtenido: 0            OK
[ 5] int no acepta double               esperado: 1            obtenido: 1            OK
[ 6] cero rutas de ejecucion            esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion17.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. La IA no se cae, pero **trata mal las funciones `void`** y los `return` sin valor, que el harness no prueba:
>
> | Caso | IA | Mi artesano | Correcto |
> | :--- | :--- | :--- | :--- |
> | `void`, `return;` sin valor, 2 rutas, 0 returns | -1 | 0 | 0: un `void` no necesita `return` |
> | `void`, `return` de tipo `"void"` | 1 | 0 | 0: no devuelve ningún valor |
> | `int`, `return;` sin valor | -1 | 1 | 1: falta el valor |
> | `int`, devuelve tipo `"bool"` (no existe) | 1 | -1 | -1: tipo desconocido |
>
> La IA regresa 1 para **cualquier** función `void`, aunque no devuelva nada, y toma `tipo_expresion = NULL` como entrada inválida en lugar de "return sin valor". Lo solucioné poniendo `void` en la posición 0 del orden de tipos y tratando `NULL` como `void`; así una función `void` solo da error cuando de verdad devuelve un valor.
>
> **Complejidad:** O(1): compara contra una lista fija de 7 tipos.

---
**Firma digital:** jordipech76-boop