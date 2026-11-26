# Bitácora de Requerimientos y Auditoría IA — Sesión 21
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Fundamentos de Código de 3 Direcciones  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Representación en Cuádruplos (op,a,b,r)  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Usar y luego incrementar:** con `(*contador)++` después de escribir, la primera llamada da `_t0`. Si incremento antes, empieza en `_t1`.
> - **El puntero del contador:** el número se cambia con `*contador`, porque la función recibe la dirección, no el valor.
> - **NULL como `_`:** en `printf` un `%s` con `NULL` no está permitido, así que cambio cada `NULL` por `"_"` antes de formatear.
> - **El formato exacto:** el harness compara el texto completo, así que va `(op, a1, a2, res)` con coma y un espacio.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Generación de temporales únicos (_t0..) |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion21.h ya existe y no
   se puede modificar.
R: Ingeniero de compiladores experto en C y código de tres direcciones.
E: Implementa void nuevo_temporal(char *dst, int cap, int *contador) y
   void emitir_cuadruplo(char *dst, int cap, const char *op, const char *a1,
                         const char *a2, const char *res):
   1. nuevo_temporal escribe "_tK" con el valor actual y después incrementa el
      contador (la primera llamada da _t0).
   2. emitir_cuadruplo escribe "(op, a1, a2, res)"; cada NULL se escribe "_".
   3. Escribe solo en dst, sin pasar de cap; sin malloc ni variables globales.
O: Solo el .c, empezando con #include "api_sesion21.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion21.h"
#include <stdio.h>
#include <stddef.h>

static const char *o_guion(const char *s) {
    return s != NULL ? s : "_";
}

void nuevo_temporal(char *dst, int cap, int *contador) {
    if (dst == NULL || cap <= 0 || contador == NULL) {
        return;
    }
    snprintf(dst, (size_t)cap, "_t%d", *contador);
    (*contador)++;
}

void emitir_cuadruplo(char *dst, int cap, const char *op, const char *a1, const char *a2, const char *res) {
    if (dst == NULL || cap <= 0) {
        return;
    }
    snprintf(dst, (size_t)cap, "(%s, %s, %s, %s)",
             o_guion(op), o_guion(a1), o_guion(a2), o_guion(res));
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Expresiones aritméticas compuestas

### Salida del harness
```text
=== AUDITORIA SESION 21 - Cuadruplos y temporales unicos ===
Caso                               esperado:              obtenido:              resultado
[ 1] nuevo_temporal(_t0)                esperado: _t0          obtenido: _t0          OK
[ 2] nuevo_temporal(_t7)                esperado: _t7          obtenido: _t7          OK
[ 3] cuadruplo de a + b
     esperado: (+, a, b, _t0)
     obtenido: (+, a, b, _t0)
     -> OK
[ 4] cuadruplo de _t0 * c
     esperado: (*, _t0, c, _t1)
     obtenido: (*, _t0, c, _t1)
     -> OK
[ 5] asignacion simple                  esperado: (=, a, _, x) obtenido: (=, a, _, x) OK
[ 6] goto sin operandos
     esperado: (goto, L1, _, _)
     obtenido: (goto, L1, _, _)
     -> OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion21.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6 y los dos usan `snprintf`, así que nunca escriben fuera de `cap`. El problema está en **qué pasa cuando el nombre no cabe**: la IA corta el texto pero **igual avanza el contador**, y salen dos temporales con el mismo nombre:
>
> | Caso (buffer de 4, contador en 10) | IA | Mi artesano |
> | :--- | :--- | :--- |
> | dos llamadas a `nuevo_temporal` | `"_t1"` y `"_t1"`, contador 12 | `""` y `""`, contador 10 |
>
> `_t10` y `_t11` se cortan en `_t1`, y en el TAC serían la misma variable, aunque eran dos temporales distintos. Lo solucioné revisando lo que regresa `snprintf`: si el nombre no cupo completo, dejo la cadena vacía y **no** avanzo el contador.
>
> **Complejidad:** O(L) en tiempo, con L el largo del texto escrito, y O(1) de memoria.

---
**Firma digital:** jordipech76-boop