# Bitácora de Requerimientos y Auditoría IA — Sesión 24
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Control de Flujo: Sentencia if-else  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Saltos condicionales y evaluación booleana  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Cortocircuito:** en un AND, si el primero es falso ya no hace falta revisar el segundo, por eso es `IFFALSE ... GOTO` salida. En un OR es al revés: si el primero es verdadero ya se cumple, por eso `IFTRUE`.
> - **El mismo formato para los dos operandos:** primero se emite el salto de `a` y luego el de `b`, con la misma instrucción.
> - **Segundo operando NULL:** solo sale un salto; no invento una comparación.
> - **Operador que no existe:** si no es `&` ni `|`, regreso 0 líneas en lugar de adivinar.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Cortocircuito lógico en expresiones |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion24.h ya existe y no
   se puede modificar.
R: Ingeniero de compiladores experto en C y control de flujo.
E: Implementa int emitir_condicion(char lineas[][64], int max, const char *a,
                                    const char *b, char op, const char *etiqueta):
   1. Para & escribe "IFFALSE x GOTO etiqueta" por cada operando.
   2. Para | escribe "IFTRUE x GOTO etiqueta" por cada operando.
   3. Si b es NULL emite un solo salto; devuelve cuántas líneas escribió.
   4. Nunca más de max líneas ni de 63 caracteres por línea.
O: Solo el .c, empezando con #include "api_sesion24.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion24.h"
#include <stdio.h>
#include <stddef.h>

int emitir_condicion(char lineas[][64], int max, const char *a, const char *b, char op, const char *etiqueta) {
    if (lineas == NULL || max <= 0 || a == NULL) {
        return 0;
    }
    const char *instruccion = (op == '&') ? "IFFALSE" : "IFTRUE";
    int n = 0;
    snprintf(lineas[n], 64, "%s %s GOTO %s", instruccion, a, etiqueta);
    n++;
    if (b != NULL && n < max) {
        snprintf(lineas[n], 64, "%s %s GOTO %s", instruccion, b, etiqueta);
        n++;
    }
    return n;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Condiciones compuestas con AND y OR

### Salida del harness
```text
=== AUDITORIA SESION 24 - Saltos condicionales con cortocircuito ===
Caso                               esperado:              obtenido:              resultado
[ 1] a && b (primer salto)
     esperado: IFFALSE a GOTO L9
     obtenido: IFFALSE a GOTO L9
     -> OK
[ 2] a && b (segundo salto)
     esperado: IFFALSE b GOTO L9
     obtenido: IFFALSE b GOTO L9
     -> OK
[ 3] a && b (dos saltos)                esperado: 2            obtenido: 2            OK
[ 4] a || b (primer salto)
     esperado: IFTRUE a GOTO L9
     obtenido: IFTRUE a GOTO L9
     -> OK
[ 5] a || b (segundo salto)
     esperado: IFTRUE b GOTO L9
     obtenido: IFTRUE b GOTO L9
     -> OK
[ 6] condicion simple (b NULL)
     esperado: IFFALSE a GOTO L0
     obtenido: IFFALSE a GOTO L0
     -> OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion24.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. La IA decide con `(op == '&') ? "IFFALSE" : "IFTRUE"`, así que **todo lo que no es `&` lo toma como OR**, y tampoco revisa la etiqueta:
>
> | Caso | IA | Mi artesano |
> | :--- | :--- | :--- |
> | operador `'^'` (no existe) | 2 líneas: `IFTRUE a GOTO L1` | 0 líneas |
> | etiqueta `NULL` | `IFFALSE a GOTO (null)` | 0 líneas |
>
> 1. **Operador inválido:** la IA genera código como si fuera un OR, cuando debería rechazarlo. Un error en el parser pasaría como código válido. Yo reviso `&` y `|` por separado y cualquier otro regresa 0.
> 2. **Etiqueta NULL:** pasar `NULL` a un `%s` es comportamiento indefinido; aquí glibc escribe `(null)`, pero en otro sistema podría tronar. Yo regreso 0 si falta la etiqueta.
>
> **Complejidad:** O(1): a lo mucho dos líneas, cada una de máximo 63 caracteres.

---
**Firma digital:** jordipech76-boop