# Bitácora de Requerimientos y Auditoría IA — Sesión 23
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Etiquetas y Saltos Incondicionales  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Generador de etiquetas e instrucciones goto  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Igual que los temporales:** la etiqueta usa el valor actual y después incrementa, así la primera es `L0`.
> - **Salto o marca:** con `es_salto` distinto de 0 se escribe `GOTO L2`, y con 0 la marca `L2:`. Cualquier número que no sea 0 cuenta como verdadero, no solo el 1.
> - **Etiqueta NULL:** el contrato pide cadena vacía, así que pongo `dst[0] = '\0'` y salgo.
> - **Nunca pasar de `cap`:** todo va con `snprintf`.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Trazabilidad del flujo de control |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion23.h ya existe y no
   se puede modificar.
R: Ingeniero de compiladores experto en C y control de flujo.
E: Implementa void nueva_etiqueta(char *dst, int cap, int *contador) y
   void emitir_flujo(char *dst, int cap, const char *etiqueta, int es_salto):
   1. nueva_etiqueta escribe "LK" con el valor actual y después incrementa
      (la primera es L0).
   2. emitir_flujo con es_salto distinto de cero escribe "GOTO etiqueta";
      con cero escribe "etiqueta:".
   3. Una etiqueta NULL produce cadena vacía; nunca escribas fuera de cap.
O: Solo el .c, empezando con #include "api_sesion23.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion23.h"
#include <stdio.h>
#include <stddef.h>

void nueva_etiqueta(char *dst, int cap, int *contador) {
    if (dst == NULL || cap <= 0 || contador == NULL) {
        return;
    }
    snprintf(dst, (size_t)cap, "L%d", *contador);
    (*contador)++;
}

void emitir_flujo(char *dst, int cap, const char *etiqueta, int es_salto) {
    if (dst == NULL || cap <= 0) {
        return;
    }
    if (etiqueta == NULL) {
        dst[0] = '\0';
        return;
    }
    if (es_salto != 0) {
        snprintf(dst, (size_t)cap, "GOTO %s", etiqueta);
    } else {
        snprintf(dst, (size_t)cap, "%s:", etiqueta);
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Saltos adelante y atrás estructurados

### Salida del harness
```text
=== AUDITORIA SESION 23 - Etiquetas y saltos incondicionales ===
Caso                               esperado:              obtenido:              resultado
[ 1] nueva_etiqueta(L0)                 esperado: L0           obtenido: L0           OK
[ 2] nueva_etiqueta(L5)                 esperado: L5           obtenido: L5           OK
[ 3] emitir_flujo(GOTO L2)              esperado: GOTO L2      obtenido: GOTO L2      OK
[ 4] emitir_flujo(marca L2)             esperado: L2:          obtenido: L2:          OK
[ 5] emitir_flujo(NULL)                 esperado:              obtenido:              OK
[ 6] nueva_etiqueta(L10)                esperado: L10          obtenido: L10          OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion23.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6 y `emitir_flujo` está bien en los dos. El problema es el mismo de la sesión 21, pero con etiquetas: si `L10` no cabe en el buffer, la IA lo corta y **avanza el contador igual**:
>
> | Caso (buffer de 3, contador en 10) | IA | Mi artesano |
> | :--- | :--- | :--- |
> | dos llamadas a `nueva_etiqueta` | `"L1"` y `"L1"`, contador 12 | `""` y `""`, contador 10 |
>
> Dos etiquetas iguales son grave: un `GOTO L1` ya no sabría a cuál de los dos saltar. Lo solucioné revisando lo que regresa `snprintf`: si no cupo completa, la cadena queda vacía y el contador no avanza, así el que llama se da cuenta.
>
> **Complejidad:** O(L) en tiempo, con L el largo de la etiqueta, y O(1) de memoria.

---
**Firma digital:** jordipech76-boop