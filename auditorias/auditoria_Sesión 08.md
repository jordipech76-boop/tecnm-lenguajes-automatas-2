# Bitácora de Requerimientos y Auditoría IA — Sesión 08
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Inferencia y Promoción de Tipos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Reglas de casteo implícito  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **TIPO_ERROR vale 0:** si nomás comparas, `promover(ERROR, INT)` daría `int`. Por eso primero reviso con `es_valido()` que los dos tipos sean CHAR, INT o FLOAT.
> - **El orden de los parámetros:** en `hay_perdida(destino, origen)` me confundía cuál iba primero. Hay pérdida cuando el **destino** es más angosto: `destino < origen`.
> - **Usar Gemini en Colab:** la respuesta salió como una propuesta en verde y rojo dentro de la celda; no se aplica hasta darle "Aceptar" y luego correr la celda.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Promoción entera y ensanchamiento |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion08.h ya existe y no
   se puede modificar:
   typedef enum { TIPO_ERROR = 0, TIPO_CHAR = 1, TIPO_INT = 2, TIPO_FLOAT = 3 } Tipo;
R: Ingeniero de compiladores experto en C y sistemas de tipos.
E: Implementa exactamente
     Tipo promover(Tipo a, Tipo b);
     int hay_perdida(Tipo destino, Tipo origen);
     const char *nombre_tipo(Tipo t);
   1. Jerarquía CHAR < INT < FLOAT.
   2. promover devuelve el tipo más ancho; si uno es TIPO_ERROR, devuelve TIPO_ERROR.
   3. hay_perdida devuelve 1 si el destino es más angosto que el origen, 0 si no.
   4. nombre_tipo devuelve "char", "int", "float" o "error".
O: Solo el .c, empezando con #include "api_sesion08.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Gemini en Colab)
```c
#include "api_sesion08.h"
#include <stddef.h>

static int es_valido(Tipo t) {
    return (t == TIPO_CHAR || t == TIPO_INT || t == TIPO_FLOAT);
}

Tipo promover(Tipo a, Tipo b) {
    if (!es_valido(a) || !es_valido(b)) {
        return TIPO_ERROR;
    }
    return (a > b) ? a : b;
}

int hay_perdida(Tipo destino, Tipo origen) {
    if (!es_valido(destino) || !es_valido(origen)) {
        return 1;
    }
    return (destino < origen);
}

const char *nombre_tipo(Tipo t) {
    switch (t) {
        case TIPO_CHAR:  return "char";
        case TIPO_INT:   return "int";
        case TIPO_FLOAT: return "float";
        case TIPO_ERROR: return "error";
        default:         return "desconocido";
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Mezcla de tipos enteros y flotantes

### Salida del harness
```text
=== AUDITORIA SESION 08 - Reglas de promocion y perdida ===
Caso                               esperado:              obtenido:              resultado
[ 1] promover(CHAR,INT)                 esperado: int          obtenido: int          OK
[ 2] promover(INT,FLOAT)                esperado: float        obtenido: float        OK
[ 3] promover(FLOAT,FLOAT)              esperado: float        obtenido: float        OK
[ 4] promover(ERROR,INT)                esperado: error        obtenido: error        OK
[ 5] hay_perdida(CHAR,INT)              esperado: 1            obtenido: 1            OK
[ 6] hay_perdida(INT,CHAR)              esperado: 0            obtenido: 0            OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion08.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6 y el código de Gemini es casi igual al mío (hasta la función `es_valido()`, que el prompt no pedía). Puede ser porque el prompt era muy específico o porque Gemini en Colab ve el notebook completo, incluida mi celda del artesano. Lo único extra es un `#include <stddef.h>` que no usa.
>
> Lo que **omitieron los dos** es que pasar de `int` a `float` **sí puede perder datos**. Según la jerarquía, `hay_perdida(FLOAT, INT)` regresa 0, pero un `float` solo guarda enteros exactos hasta 16,777,216 (2²⁴). Lo comprobé:
>
> ```text
> hay_perdida(FLOAT,INT)=0   int 16777217 -> float 16777216
> ```
>
> El número cambia y la función no avisa. No lo cambié porque el harness y el contrato esperan la jerarquía CHAR < INT < FLOAT; para corregirlo habría que revisar el **valor** (no solo el tipo) o usar `double`.
>
> **Complejidad:** O(1) en tiempo y memoria; solo comparaciones, sin ciclos ni `malloc`.

---
**Firma digital:** jordipech76-boop
